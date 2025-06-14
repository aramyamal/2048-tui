# 2048-tui

A simple terminal-based 2048 game written in C that uses the built-in 16-color scheme.

<img src="screenshots/start-up.png" alt="Game start" width="350"/><img src="screenshots/in-game.png" alt="Game in progress" width="350"/>

## Usage

Run the game with:
```sh
$ 2048-tui
```

### Flag options

- `-d n`, `--dimension n`  
Set the board size (minimum is 3, default is 4).  
Example:  
```sh
  $ 2048-tui --dimension 5
```

- `-u n`, `--undos n`  
Set the number of allowed undos (minimus is 0, default is 3).  
Example:  
```sh
  $ 2048-tui --undos 10
```

You can also combine both options:
```sh
$ 2048-tui -d 5 -u 10
```

## Installation

### Arch

You can install directly from the AUR using an AUR helper like `yay` or `paru`:
```sh
$ yay -S 2048-tui
```
or
```sh
$ paru -S 2048-tui
```

Or manually:
```sh
$ git clone https://aur.archlinux.org/2048-tui.git
$ cd 2048-tui
$ makepkg -si
```

### Build and install from Source


To install :
```sh
$ make
$ sudo make install
```

To uninstall:
```sh
$ sudo make uninstall
```

**Dependencies:**
- **ncurses** 

On Debian/Ubuntu:
```sh
$ sudo apt install libncurses5-dev
```

