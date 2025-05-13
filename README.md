# Cubos Demo Repository

This repository holds the games we built with **Cubos**, both internally, or while participating in external events or game jams.
Most of these games can be played on the browser or downloaded on our [itch.io](https://cubos-engine.itch.io/).

Currently, this repo holds the following games:
- **Scraps vs Zombies** - [Released](https://riscadoa.itch.io/scraps-vs-zombies).
- **Ondisseia** - [Released](https://riscadoa.itch.io/ondisseia).
- **Roll Racers** - [Released](https://riscadoa.itch.io/roll-racers).
- **Airships** - unfinished, work in progress!
- **Cars** - the first game we made with **Cubos**, a very basic racing game.

## Building

To build the demos you need to first separately install the **Cubos Engine** using *CMake*.
Then, when building this repository, you might need to help *CMake* find the installation directory of **Cubos**.
By default, all demos are compiled - you might disable any of them using their respective *CMake* option.

We recommend building both the engine and the demos in Release - when developing, Release with Debug Information is usually enough to debug any errors that show up.

## Distributing

When compiling the game during development, paths to assets directories are hardcoded into the produced binaries.
This means that the produced binaries are not portable and won't work on other machines.
To create a portable installation of the game, enable the *CMake* option `DISTRIBUTE`, and then install the project through *CMake*.
