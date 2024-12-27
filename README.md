# KOLOR EXPORTER

KDED (KDE Daemon) module that automatically exports the kde color scheme to use in other programs

for now it exports to:

- `~/.config/kde-colors.css`
- `~/.local/share/rofi/themes/kde-colors.rasi`
Vencord theme folders:
- `~/.config/Vencord/themes/kde-colors.css`
- `~/.var/app/com.discordapp.Discord/config/Vencord`
- `~/.config/vesktop/themes/kde-colors.css`
- `~/.var/app/dev.vencord.Vesktop/config/vesktop`

## Compiling and installing
Run:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

## Uninstalling
Run:
```bash
sudo rm /usr/lib/qt6/plugins/kf6/kded/kolor-exporter.so
```

or whatever the qt6 plugins path is in your distro, idk, i use arch btw
