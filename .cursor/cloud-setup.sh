#!/usr/bin/env bash
#
# Cloud Agent install step for the PvPArena Unreal Engine 5.8 project.
#
# This script prepares everything about the development environment that does
# NOT require Epic Games' proprietary engine binaries:
#   * the Linux host toolchain UE5 uses (clang/lld, cmake, ninja, .NET SDK),
#   * Git LFS (the repo stores *.uasset / *.umap through LFS),
#   * headless rendering prerequisites (Vulkan + a software rasterizer, Xvfb),
#     so the editor/commandlets can run without a physical GPU.
#
# Acquiring Unreal Engine 5.8 itself is gated behind an Epic Games account
# (both the EpicGames/UnrealEngine GitHub source and the Linux binary
# downloads require authentication). That step is delegated to
# fetch-unreal-engine.sh and only runs when credentials are provided, so this
# install step always terminates successfully and is safe to re-run.
#
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

log()  { printf '\033[1;36m[cloud-setup]\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33m[cloud-setup] WARNING:\033[0m %s\n' "$*"; }

SUDO=""
if [[ "$(id -u)" -ne 0 ]]; then
  SUDO="sudo"
fi

# ---------------------------------------------------------------------------
# 1. System packages
# ---------------------------------------------------------------------------
# Pinned to what UE5 needs on Ubuntu 24.04. apt-get install is idempotent, so
# re-running the script is a fast no-op once the packages are present.
APT_PACKAGES=(
  build-essential          # gcc/g++, make, libc headers
  clang                    # UE5's supported C++ compiler on Linux (18.x on 24.04)
  lld                      # LLVM linker used by UnrealBuildTool
  cmake
  ninja-build
  make
  git
  git-lfs                  # *.uasset / *.umap are stored via LFS
  python3
  python3-pip
  python3-dev
  curl
  wget
  unzip
  ca-certificates
  dotnet-sdk-8.0           # UnrealBuildTool / AutomationTool (C#/.NET) host runtime
  libvulkan1               # Vulkan loader (UE5 renders through Vulkan on Linux)
  vulkan-tools             # vulkaninfo, for verifying the software renderer
  mesa-vulkan-drivers      # lavapipe: CPU/software Vulkan device for headless GPUs
  xvfb                     # virtual framebuffer for headless editor/commandlets
  x11-xserver-utils
  libsdl2-2.0-0            # editor windowing/input dependency
  libicu74                 # ICU runtime used by the engine
)

log "Installing system packages (${#APT_PACKAGES[@]} packages)..."
export DEBIAN_FRONTEND=noninteractive
$SUDO apt-get update -qq
$SUDO apt-get install -y --no-install-recommends "${APT_PACKAGES[@]}"

# ---------------------------------------------------------------------------
# 2. Git LFS
# ---------------------------------------------------------------------------
log "Configuring Git LFS..."
# --skip-repo installs the global LFS smudge/clean filters without rewriting the
# repo's git hooks, which are managed by Cursor and would otherwise conflict.
# Keep the whole LFS step non-fatal so install always terminates.
git lfs install --skip-repo || warn "git lfs install reported a non-zero status; continuing."
# Best-effort: pull the binary assets. Succeeds only if the LFS remote holds
# the objects; never fail the install if it cannot (e.g. no LFS quota/access).
if ! git lfs pull 2>/dev/null; then
  warn "git lfs pull did not fetch objects (asset .uasset/.umap files may be pointers)."
fi

# ---------------------------------------------------------------------------
# 3. Unreal Engine 5.8 (credential-gated, optional)
# ---------------------------------------------------------------------------
log "Checking for an Unreal Engine 5.8 installation..."
if [[ -x "${UE_ROOT:-}/Engine/Binaries/Linux/UnrealEditor" ]]; then
  log "Found existing engine at UE_ROOT=$UE_ROOT"
else
  # Delegate acquisition; the helper no-ops with guidance if no creds exist.
  "$REPO_ROOT/.cursor/fetch-unreal-engine.sh" || \
    warn "Engine not acquired. Non-engine setup completed; see message above."
fi

# ---------------------------------------------------------------------------
# 4. Summary
# ---------------------------------------------------------------------------
log "Toolchain versions:"
printf '  clang    : %s\n' "$(clang --version | head -n1)"
printf '  lld      : %s\n' "$(ld.lld --version 2>/dev/null | head -n1 || echo 'installed')"
printf '  cmake    : %s\n' "$(cmake --version | head -n1)"
printf '  ninja    : %s\n' "$(ninja --version 2>/dev/null || echo '?')"
printf '  dotnet   : %s\n' "$(dotnet --version 2>/dev/null || echo '?')"
printf '  git-lfs  : %s\n' "$(git lfs version 2>/dev/null || echo '?')"
# Probe the software Vulkan device under Xvfb (no physical GPU needed). This is
# the same rendering path a headless editor/commandlet run would take.
VK_DEV="$(
  set +e +o pipefail
  XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/tmp/xdg-runtime}"; mkdir -p "$XDG_RUNTIME_DIR"; chmod 700 "$XDG_RUNTIME_DIR"
  VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/lvp_icd.json \
    xvfb-run -a -s "-screen 0 1280x720x24" vulkaninfo --summary 2>/dev/null \
    | awk -F'= ' '/deviceName/{gsub(/^ +/,"",$2); print $2; exit}'
)" || true
printf '  vulkan   : %s\n' "${VK_DEV:-loader installed (no software device detected)}"

log "Install step complete."
