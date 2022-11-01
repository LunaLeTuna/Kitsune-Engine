
![Logo](https://github.com/LunaLeTuna/Kitsune-Engine/blob/main/kitsune_logo.png?raw=true)

Basicly made this to help me quickly build aplications and games, mainly for a computer information sience class i'm taking in highschool.
It has very messy V8 implementation. :3
Also, to quickly get it up I kinda barrowed some code from learnopengl.

to test out the demos, if you can even call them that, change the diffirent js files to "index"
Also the current compiled version in the repository is compiled for linux, oof...

Another problem is that some computers may not load all the props, bigger oof



## Credits

- [Google's V8 Engine](https://v8.dev/)
- [Bullet Physics](https://pybullet.org/)
- [GLFW](https://www.glfw.org/)
- [GLM](https://github.com/g-truc/glm)
- [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
- [FreeType](https://freetype.org/)
- [learnopengl for lots of opengl code](https://learnopengl.com/)
- [Ty for pig "Ellie Pig" model](https://github.com/TyThePotato)

## Roadmap

- Add audio system

- Make robust obj importer, allowing for bones and auto triangulation

- finish map editor

- Make Documentation

- Make better demos

- ~~Allow for scripts to call other scripts (like "require" in node.js)~~
    I think I kinda got it implemented, just has some flaws probably; like requireing in a required js file
    will get the file from where the executable is, not the actuall js file is... I think havn't tried.
    also isn't exactly like node.

- Port to vulkan and jolt physics (long time before that)
