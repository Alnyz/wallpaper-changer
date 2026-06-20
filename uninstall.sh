#!/usr/bin/env bash
#
# Wallpaper Changer uninstaller.
#
#   ./uninstall.sh
#
# Removes the app safely. Your wallpapers are NEVER touched. Dependencies stay
# unless you explicitly ask for them, and even then only the packages the
# installer recorded as having installed are considered; pacman itself refuses
# to remove anything other software still needs.

set -euo pipefail
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

trap 'die "unexpected failure on line $LINENO. Stopping before any further change."' ERR

DATA_DIR="${XDG_DATA_HOME:-$HOME/.local/share}/wallpaper-changer"
CONFIG="${XDG_CONFIG_HOME:-$HOME/.config}/wallpaperchangerrc"

[[ $EUID -ne 0 ]] || die "don't run as root. Run as your normal user; it'll ask for sudo when it needs it."

# --- find the installed app files ---
say "Looking for Wallpaper Changer"
manifest=""
if   [[ -f "$DATA_DIR/install_manifest.txt" ]]; then manifest="$DATA_DIR/install_manifest.txt"
elif [[ -f build/install_manifest.txt       ]]; then manifest="build/install_manifest.txt"
fi

declare -a candidates=()
if [[ -n "$manifest" ]]; then
    mapfile -t candidates < <(grep -v '^[[:space:]]*$' "$manifest")
else
    candidates=(
        /usr/bin/wallpaper-changer
        /usr/share/applications/io.github.alnyz.WallpaperChanger.desktop
        /usr/share/dbus-1/services/io.github.alnyz.WallpaperChanger.service
        /usr/share/icons/hicolor/scalable/apps/io.github.alnyz.WallpaperChanger.svg
    )
fi

# keep only paths that actually exist (validate before deletion)
declare -a files=()
for f in "${candidates[@]}"; do [[ -e "$f" ]] && files+=("$f"); done

have_config=false
[[ -f "$CONFIG" ]] && have_config=true

if (( ${#files[@]} == 0 )) && ! $have_config; then
    ok "Wallpaper Changer doesn't appear to be installed. Nothing to do."
    exit 0
fi

# --- find dependencies the installer recorded (if any) ---
declare -a deps=()
if [[ -f "$DATA_DIR/installed-packages.txt" ]] && command -v pacman >/dev/null; then
    while IFS= read -r p; do
        [[ -n "$p" ]] && pacman -Qq "$p" &>/dev/null && deps+=("$p")
    done < "$DATA_DIR/installed-packages.txt"
fi

# --- show exactly what will be removed ---
say "The following will be removed"
for f in "${files[@]}"; do printf '    %s\n' "$f"; done
if $have_config; then printf '    %s  %s\n' "$CONFIG" "${DIM}(your settings, not wallpapers)${R}"; fi
if pgrep -f /usr/bin/wallpaper-changer >/dev/null 2>&1; then
    printf '    %s\n' "${DIM}(the running app will be closed first)${R}"
fi
printf '\n%s\n' "  ${GREEN}Your wallpapers are NOT touched.${R} ${DIM}~/Pictures/Wallpapers and its wallhaven/ folder are left exactly as they are.${R}"

# --- dependency choice (default No) ---
remove_deps=false
if (( ${#deps[@]} )); then
    say "Dependencies installed by this project"
    printf '    %s\n' "${deps[*]}"
    printf '%s\n' "  ${DIM}pacman will refuse to remove any that other software still needs.${R}"
    if ask "Do you want to uninstall these dependencies as well?"; then
        remove_deps=true
    else
        ok "keeping all dependencies."
    fi
else
    say "Dependencies"
    ok "no packages were recorded as installed by this project, so none will be removed."
fi

# --- final confirmation before anything is deleted ---
say "Confirm"
if ! ask "Remove Wallpaper Changer now?"; then
    warn "cancelled, nothing was changed."
    exit 0
fi

# --- remove ---
pkill -f /usr/bin/wallpaper-changer 2>/dev/null || true

say "Removing the app"
for f in "${files[@]}"; do
    [[ -e "$f" ]] || continue
    if [[ "$f" == /usr/* || "$f" == /opt/* ]]; then sudo rm -f "$f"; else rm -f "$f"; fi
    ok "removed $f"
done
if $have_config; then rm -f "$CONFIG"; ok "removed settings ($CONFIG)"; fi
rm -rf "$DATA_DIR" 2>/dev/null || true
command -v kbuildsycoca6 >/dev/null && kbuildsycoca6 >/dev/null 2>&1 || true

if $remove_deps && (( ${#deps[@]} )); then
    say "Removing dependencies"
    if sudo pacman -R "${deps[@]}"; then
        ok "dependencies removed"
    else
        warn "pacman left some packages in place; they're still required by other software, which is fine."
    fi
fi

printf '\n%s\n' "${GREEN}${B}Done.${R} Wallpaper Changer has been removed."
