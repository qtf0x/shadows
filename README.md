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

I use like every single key on the entire keyboard, so get ready. To follow the order that different effects are meant to demonstrated:

1. First, move the camera around by left-clicking and dragging the mouse and using the scroll wheel to zoom, until you have a good view of the scene.
2. Press [`1`] to turn on the most primitive version of __planar projection shadows__. You should see a mess of Z-fighting.
3. Press [`1`] again to fix the Z-fighting and see perfectly black, perfectly hard shadows.
4. Press [`2`] to blend the shadows with the color of the receiver surfaces. Now you'll see the overlapping blending artifact.
5. Press [`3`] to fix this by using a fancy stencil buffer trick. Now we have nice blended shadows.
6. Press [`O`] to reveal a bunch more objects in the scene, which are also shadow casters. As such, with the current algorithm, they cannot also by shadow receivers.
7. Press [`4`] to switch to the next algorithm, __shadow textures__. Now objects with curved surfaces, like the outer ring of spheres, can be receivers. But they still can't be both receivers and blockers.
8. Repeatedly press [`Z`] to lower the texture resolution and improve performance, or [`X`] to increase the texture resolution and improve image quality.
9. Press [`5`] to toggle linear filtering on the shadow textures. This makes them look slightly better when they're really low-resolution, but is not a great solution and doesn't look good in motion. Maybe turn this off.
10. Press [`6`] to switch to the last algorithm, __shadow mapping__. You'll immediately see a lot of artifacts in the form of shadow acne.
11. Repeatedly press [`C`] to lower the shadow bias and make contact shadows more accurate, or [`V`] to increase the shadow bias and make the shadow acne less noticeable. If you increase this, you should now be able to clearly see that all objects in the scene can be both shadow casters and receivers - this enables self-shadowing on the teapots!
12. Press [`7`] to turn on __percentage-closer filtering (PCF)__. This will do a better job than linear filtering of softening the edges of low-resolution shadow maps and decreasing perceived aliasing. This is more visible if you lower the shadow map resolution with [`Z`].
13. Repeatedly press [`G`] to decrease the number of PCF samples and improve performance, or [`H`] to increase the number of PCF samples and improve image quality.

At this point you can adjust the settings and try different combinations of things to see what happens.

### Extra Controls

At any time, you may use these keys:

- [`L`] to stop the light from automatically moving up and down. While in this mode, press [`B`] to manually move the light down, [`N`] to move it up, or [`L`] to start it moving automatically again from it's current position.
- [`S`] to stop the objects in the scene from automatically spinning in a circle. While in this mode, press [`LEFT`] to manually spin the objects clockwise, [`RIGHT`] to spin them anti-clockwise, or [`S`] to start them moving automatically again from their current position.
- [`UP`] or [`DOWN`] to adjust the tessellation level of the teapots up or down, respectively. They default to the maximum (that my graphics driver supports, anyway) of 64. The current level is always displayed in the window title, along with the FPS (though this might be hard to see since the program attempts to launch in fullscreen).

Happy coding! <3 <3
