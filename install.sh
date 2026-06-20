#!/usr/bin/env bash
#
# Wallpaper Changer installer (Arch / CachyOS).
#
#   git clone https://github.com/Alnyz/wallpaper-changer.git
#   cd wallpaper-changer
#   ./install.sh
#
# It checks for the packages it needs, asks before installing anything, builds
# and tests the project, then installs it (asking again first). Nothing on your
# system is changed without a [y/N] confirmation.

set -euo pipefail

# Run from the repo root no matter where it was called from.
cd "$(dirname "$(realpath "$0")")"

# --- output helpers ---
if [[ -t 1 ]]; then
    B=$'\e[1m'; GREEN=$'\e[32m'; YELLOW=$'\e[33m'; RED=$'\e[31m'; DIM=$'\e[2m'; R=$'\e[0m'
else
    B=""; GREEN=""; YELLOW=""; RED=""; DIM=""; R=""
fi
say()  { printf '\n%s\n' "${B}==>${R} $*"; }
ok()   { printf '%s\n' "  ${GREEN}✓${R} $*"; }
warn() { printf '%s\n' "  ${YELLOW}!${R} $*"; }
die()  { printf '\n%s\n' "${RED}==> error:${R} $*" >&2; exit 1; }
ask()  { local a=""; read -rp "${B}?${R} $1 [y/N] " a || true; [[ "${a,,}" == y* ]]; }

trap 'die "unexpected failure on line $LINENO. Nothing further was changed."' ERR

# Where we record what got installed, so uninstall.sh can undo exactly this.
DATA_DIR="${XDG_DATA_HOME:-$HOME/.local/share}/wallpaper-changer"

# --- preflight ---
say "Checking your system"
[[ -f CMakeLists.txt ]] || die "run this from the cloned repo root (no CMakeLists.txt here)."
[[ $EUID -ne 0 ]]       || die "don't run as root. Run as your normal user; it'll ask for sudo when it needs it."
command -v pacman >/dev/null || die "this installer is for Arch / CachyOS (pacman not found). See the README for manual steps on other distros."
if [[ "${XDG_CURRENT_DESKTOP:-}" != *KDE* ]]; then
    warn "you don't seem to be in a KDE Plasma session; it'll still build, but the app only runs on KDE Plasma 6 (Wayland)."
fi
ok "Arch-based system"

# --- dependencies ---
say "Checking dependencies"
needed=()
# build toolchain (base-devel ships gcc + make)
{ command -v gcc && command -v make; } >/dev/null 2>&1 || needed+=(base-devel)
for pkg in cmake extra-cmake-modules \
           qt6-base qt6-declarative qt6-svg \
           kconfig kcoreaddons kdbusaddons ki18n kio kwindowsystem \
           layer-shell-qt plasma-workspace; do
    pacman -Qq "$pkg" &>/dev/null || needed+=("$pkg")
done

if (( ${#needed[@]} )); then
    warn "not installed: ${needed[*]}"
    if ask "Install them now with 'sudo pacman -S --needed ${needed[*]}'?"; then
        sudo pacman -S --needed "${needed[@]}"
        mkdir -p "$DATA_DIR"
        printf '%s\n' "${needed[@]}" >> "$DATA_DIR/installed-packages.txt"
        sort -u "$DATA_DIR/installed-packages.txt" -o "$DATA_DIR/installed-packages.txt"
        ok "dependencies installed (recorded for uninstall)"
    else
        die "those packages are required. Install them and run this again."
    fi
else
    ok "everything required is already installed"
fi

# --- build ---
say "Building (this takes a minute)"
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -Wno-dev
cmake --build build -j "$(nproc)"
ok "built"

# --- sanity check ---
say "Running tests"
ctest --test-dir build --output-on-failure
ok "tests passed"

# --- install ---
say "Installing"
if ask "Install to /usr with 'sudo cmake --install build'?"; then
    sudo cmake --install build
    mkdir -p "$DATA_DIR"
    cp -f build/install_manifest.txt "$DATA_DIR/install_manifest.txt" 2>/dev/null || true
else
    warn "built but not installed. Run 'sudo cmake --install build' whenever you're ready."
    exit 0
fi

# --- validate + refresh ---
[[ -x /usr/bin/wallpaper-changer ]] || die "install finished but /usr/bin/wallpaper-changer is missing."
ok "installed: /usr/bin/wallpaper-changer"
[[ -f /usr/share/applications/io.github.alnyz.WallpaperChanger.desktop ]] \
    && ok "desktop entry + Meta+O shortcut registered" \
    || warn "desktop entry not found; the Meta+O shortcut may not register."

# Refresh KDE's app/service cache so the launcher entry and shortcut are picked up.
if command -v kbuildsycoca6 >/dev/null; then
    kbuildsycoca6 >/dev/null 2>&1 || true
    ok "refreshed KDE application cache"
fi

printf '\n%s\n' "${GREEN}${B}Done.${R} Press ${B}Meta+O${R} to open Wallpaper Changer."
printf '%s\n' "${DIM}If the hotkey doesn't respond yet, log out and back in once. KDE registers global shortcuts at login.${R}"
