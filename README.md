# Rasterized Shadows

Check out my implementations of various rasterized shadowing algorithms, and learn how they work (good luck reading my spaghetti code, though!). These are by no means the most efficient or even correct approaches, but that's what you get with research code.

## Building / Running

To view the demo, generate build files using `cmake`:

```bash
git clone https://github.com/qtf0x/shadows
cd shadows/
cmake -B build/
```

and build using `make`:

```bash
cd build/
make
cd FP/
./vmarias_FP
```

Required libraries:

- [OpenGL Mathematics (GLM)](https://github.com/g-truc/glm)
- [Graphics Library FrameWork (GLFW3)](https://github.com/glfw/glfw)
- [OpenGL Development Library Files (4.6 Core Profile)](https://www.opengl.org/)

Though these libraries are cross-platform, I develop on Linux, and don't know enough about CMake (yet) to write a robust cross-platform build script. So your mileage may vary on Windows (solution: switch to a good operating system).

## Controls

I use like every key on the entire keyboard, so I'll get around to writing it all down at some point. For now, read the code if you wanna know what they do.

Happy coding! <3 <3
