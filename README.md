# Arcade

This repository contains clones of popular arcade games written by me mainly for educational purposes and may be later rewritten in my engine

***

## Getting Started

Currently the only supported and tested platform is MacOS, however Window will be added in the near future

<ins>**1. Clone the repository:**</ins>

Clone this repository with `git clone https://github.com/TheMathy/Arcade`.

<ins>**2. Choose your game:**</ins>

Go to folder of your chosen game by `cd GameName`

<ins>**3. Use premake to create build files:**</ins>

This repository uses [Premake](https://premake.github.io/) to create build files.

Run `./vendor/premake/premake5 gmake2` to generate build files.

Note: You can use different [Action](https://premake.github.io/docs/Using-Premake/#using-premake-to-generate-project-files) to generate different type of project files (for example: xcode).

<ins>**4. Build:**</ins>

Use makefile to build your game. There are two configs: debug, relese

`make config=release`

<ins>**5. Run your game**</ins>

Binaries are built inside bin/Platform/GameName

Navigate into this directory by `cd bin/Platform/GameName`

Run the game by `./GameName`

***

## Planed Games

- [ ] Tetris
- [ ] Pac-Man
- [ ] Super Mario
