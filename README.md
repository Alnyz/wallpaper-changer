# Wallpaper Changer

A quick wallpaper picker for KDE Plasma 6 (Wayland). Press a key, pick a picture, done.

[![Wallpaper Changer demo](assets/preview.gif)](assets/preview.webm)

## What it is

I got tired of digging through System Settings every time I wanted to change my wallpaper, so I built this. You press a hotkey, a row of wallpapers slides in over your desktop, you pick one, and it's set. If you've used KRunner, it feels a lot like that.

It shows the pictures already in your wallpapers folder, and you can also search [Wallhaven](https://wallhaven.cc) for new ones without leaving the picker. The look is inspired by the [NixOS Wallpaper Picker](https://github.com/ilyamiro/nixos-configuration).

## How to use it

Press **Meta + O** (the Super / Windows key, then O) to open it. It only shows up when you ask for it; press **Esc** or click an empty part of the screen to dismiss it.

Once it's open:

- Leave the search box **empty** to browse the wallpapers in your own folder (by default `~/Pictures/Wallpapers`).
- **Type** to search Wallhaven for something new. Clear the box to go back to your folder.
- **Pick one** by clicking it, or move along the row with the **left and right arrow keys** and press **Enter**. Either way it's applied to all your monitors right away.
- Anything you grab from Wallhaven is saved into a `wallhaven` folder inside your wallpapers folder, so you keep it.

## Settings

The **gear icon** on the left of the search box lets you change:

- **Wallpapers folder**, so it looks wherever you keep your pictures.
- **Category and sort** for Wallhaven searches.
- **Fit**: how a picture sits on the screen (Fill, Fit, Center, or Tile).

To change the **Meta + O** hotkey, open **System Settings → Shortcuts** and search for "Wallpaper Changer". (The hotkey box in Settings is only a label, not editable there.)

The **?** icon opens a short cheat sheet.

Wallhaven searches show safe-for-work images only and aim for desktop-sized 16:9 wallpapers by default. You don't need an account or an API key.

## Installing

You need **KDE Plasma 6 on Wayland**, on Arch or CachyOS. Clone the repo and run the installer:

```sh
git clone https://github.com/Alnyz/wallpaper-changer.git
cd wallpaper-changer
./install.sh
```

It checks for the packages it needs (and asks before installing anything), builds and tests the project, then installs it. When it's done, press **Meta + O**. If the hotkey doesn't respond yet, log out and back in once.

### Doing it by hand

If you'd rather not run the script, install the dependencies:

```sh
sudo pacman -S --needed base-devel cmake extra-cmake-modules \
  qt6-base qt6-declarative qt6-svg \
  kconfig kcoreaddons kdbusaddons ki18n kio kwindowsystem \
  layer-shell-qt plasma-workspace
```

then build and install:

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
```

On other distributions the steps are the same; install your system's equivalents of Qt 6, KDE Frameworks 6, layer-shell-qt, and plasma-workspace first.

## Uninstalling

From the repo, run:

```sh
./uninstall.sh
```

It removes the app, and optionally the packages it installed (it asks first, and only ever removes ones it added). It never touches your wallpapers.

## License

MIT, so do what you like with it. See [LICENSE](LICENSE).
