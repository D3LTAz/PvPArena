#!/usr/bin/env bash
#
# Acquire Unreal Engine 5.8 for the PvPArena project.
#
# Unreal Engine is NOT redistributable and cannot be downloaded anonymously:
# both the source (github.com/EpicGames/UnrealEngine) and Epic's Linux binary
# builds require a verified Epic Games account. This script therefore only runs
# when the user supplies credentials as Cloud Agent secrets, and otherwise
# prints guidance and exits 0 so environment setup still succeeds.
#
# Supported acquisition modes (checked in order):
#
#   1. Pre-hosted prebuilt engine  (RECOMMENDED for this VM's size)
#        UE_ENGINE_TARBALL_URL   URL to a .zip/.tar.gz of a Linux UE 5.8 build
#                                (e.g. a private bucket you control).
#        UE_ENGINE_TARBALL_TOKEN Optional bearer token sent as an auth header.
#
#   2. Build from source
#        UE_GITHUB_TOKEN         A GitHub token for an account that is a member
#                                of the @EpicGames org (i.e. Epic account linked
#                                to GitHub). Used to clone EpicGames/UnrealEngine.
#        UE_ENGINE_BRANCH        Optional; defaults to the 5.8 release branch.
#
# The chosen engine is installed under $UE_ROOT (default: $HOME/UnrealEngine).
#
set -euo pipefail

log()  { printf '\033[1;36m[fetch-ue]\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33m[fetch-ue] WARNING:\033[0m %s\n' "$*"; }

UE_ROOT="${UE_ROOT:-$HOME/UnrealEngine}"
UE_ENGINE_BRANCH="${UE_ENGINE_BRANCH:-5.8}"
JOBS="$(nproc)"

if [[ -x "$UE_ROOT/Engine/Binaries/Linux/UnrealEditor" ]]; then
  log "Engine already present at $UE_ROOT"
  exit 0
fi

# ---------------------------------------------------------------------------
# Mode 1: prebuilt tarball
# ---------------------------------------------------------------------------
if [[ -n "${UE_ENGINE_TARBALL_URL:-}" ]]; then
  log "Downloading prebuilt engine from UE_ENGINE_TARBALL_URL ..."
  mkdir -p "$UE_ROOT"
  tmp="$(mktemp -d)"
  hdr=()
  [[ -n "${UE_ENGINE_TARBALL_TOKEN:-}" ]] && hdr=(-H "Authorization: Bearer ${UE_ENGINE_TARBALL_TOKEN}")
  archive="$tmp/engine.archive"
  curl -fL "${hdr[@]}" -o "$archive" "$UE_ENGINE_TARBALL_URL"
  log "Extracting into $UE_ROOT ..."
  case "$UE_ENGINE_TARBALL_URL" in
    *.zip) unzip -q "$archive" -d "$UE_ROOT" ;;
    *)     tar -xf "$archive" -C "$UE_ROOT" --strip-components=1 ;;
  esac
  rm -rf "$tmp"
  # If the zip extracted into a single top-level dir, normalize it.
  if [[ ! -d "$UE_ROOT/Engine" ]]; then
    inner="$(find "$UE_ROOT" -maxdepth 2 -type d -name Engine | head -n1)"
    [[ -n "$inner" ]] && UE_ROOT="$(dirname "$inner")"
  fi
  log "Prebuilt engine installed at $UE_ROOT"
  exit 0
fi

# ---------------------------------------------------------------------------
# Mode 2: build from source
# ---------------------------------------------------------------------------
if [[ -n "${UE_GITHUB_TOKEN:-}" ]]; then
  warn "Building UE 5.8 from source. Epic recommends 32GB+ RAM and ~250GB+ disk;"
  warn "this may be slow or fail on smaller VMs. A prebuilt tarball is preferred."
  log "Cloning EpicGames/UnrealEngine (branch $UE_ENGINE_BRANCH) ..."
  git clone --depth 1 --branch "$UE_ENGINE_BRANCH" \
    "https://x-access-token:${UE_GITHUB_TOKEN}@github.com/EpicGames/UnrealEngine.git" \
    "$UE_ROOT"
  cd "$UE_ROOT"
  log "Running Setup.sh (downloads engine dependencies) ..."
  ./Setup.sh
  log "Generating project files ..."
  ./GenerateProjectFiles.sh
  log "Building UnrealEditor (this takes hours) ..."
  make -j"$JOBS" UnrealEditor
  log "Source build complete at $UE_ROOT"
  exit 0
fi

# ---------------------------------------------------------------------------
# No credentials: explain and succeed (do not block install).
# ---------------------------------------------------------------------------
cat <<'EOF'

  ────────────────────────────────────────────────────────────────────────
  Unreal Engine 5.8 was NOT installed.

  UE is proprietary and cannot be downloaded without an Epic Games account.
  To let this environment acquire the engine automatically, add ONE of the
  following as a Cloud Agent secret and re-run setup:

    Preferred (fits smaller VMs):
      UE_ENGINE_TARBALL_URL   = URL to a Linux UE 5.8 build you host privately
      UE_ENGINE_TARBALL_TOKEN = (optional) bearer token for that URL

    Or build from source (needs 32GB+ RAM, ~250GB+ disk, several hours):
      UE_GITHUB_TOKEN         = GitHub token for an account linked to @EpicGames

  Everything else (compiler toolchain, .NET, Vulkan, Git LFS) is already set
  up, so C++ editing works now; building/running the game needs the engine.
  ────────────────────────────────────────────────────────────────────────

EOF
exit 0
