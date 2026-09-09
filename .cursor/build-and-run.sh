#!/usr/bin/env bash
#
# Build and headlessly smoke-test the PvPArena project.
#
# Requires an installed Unreal Engine 5.8 (see fetch-unreal-engine.sh) located
# at $UE_ROOT. This is intentionally NOT part of the install step: it needs the
# engine and is heavy. Run it manually once the engine is available:
#
#     UE_ROOT=$HOME/UnrealEngine .cursor/build-and-run.sh
#
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
UPROJECT="$REPO_ROOT/PvPArena.uproject"
UE_ROOT="${UE_ROOT:-$HOME/UnrealEngine}"

log() { printf '\033[1;36m[build-run]\033[0m %s\n' "$*"; }
die() { printf '\033[1;31m[build-run] ERROR:\033[0m %s\n' "$*" >&2; exit 1; }

[[ -d "$UE_ROOT/Engine" ]] || die "No engine at UE_ROOT=$UE_ROOT. Run .cursor/fetch-unreal-engine.sh first."

BUILD="$UE_ROOT/Engine/Build/BatchFiles/Linux/Build.sh"
UE_CMD="$UE_ROOT/Engine/Binaries/Linux/UnrealEditor-Cmd"

# 1. Compile the editor target for this project.
log "Building PvPArenaEditor (Development, Linux) ..."
"$BUILD" PvPArenaEditor Linux Development -Project="$UPROJECT" -WaitMutex -FromMsBuild

# 2. Headless smoke test: cook/validate assets without a display, under Xvfb,
#    forcing the software Vulkan device so no physical GPU is required.
log "Running headless commandlet smoke test ..."
export VK_ICD_FILENAMES="${VK_ICD_FILENAMES:-/usr/share/vulkan/icd.d/lvp_icd.json}"
export XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/tmp/xdg-runtime}"
mkdir -p "$XDG_RUNTIME_DIR" && chmod 700 "$XDG_RUNTIME_DIR"
xvfb-run -a -s "-screen 0 1280x720x24" \
  "$UE_CMD" "$UPROJECT" \
    -run=CompileAllBlueprints \
    -unattended -nop4 -nosplash -stdout -nullrhi \
  || die "Headless commandlet failed."

log "Build + headless smoke test succeeded."
