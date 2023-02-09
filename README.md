
![Logo](https://github.com/LunaLeTuna/Kitsune-Engine/blob/main/kitsune_logo.png?raw=true)

Originally programmed in c++, but now redid in rust because of a huge mem leak that slowly creeped up on me for months; it suddenly became too bad to work with.
Now it's easy to compile on linux and windows.

## Building

install le dependencies:
`$pacman -S cargo`

verify that it is installed:
`$rustc --version`

then clone this git & cd in

run this:
`$cargo run`

## Credits

- [glium](https://github.com/glium/glium)
- [nalgebra](https://nalgebra.org/)
- [Rusty-V8](https://github.com/denoland/rusty_v8)
- [Ty for pig "Ellie Pig" model](https://github.com/TyThePotato)

## Project Ideas with KE
- plug in the file system in to a git like repository & make a game like S&Box or Brick-Hill
- use wasm to allow for other programming languages