# grain
A modern C++ framework for building native, cross-platform applications with a clean platform-independent API.

## Linux backends

Linux builds use X11 by default. The optional **Linux GTK4** backend uses GTK4
windows and native GSK paths rendered through a custom widget's snapshot. Select
one backend per build; application code uses the same `App`, `Window`, `View`,
and `GraphicContext` APIs.

Install GTK4 4.14 or later development packages, CMake 3.30 or later, Ninja, and a
C++23 compiler. On Ubuntu/Debian, the GTK4 development package is `libgtk-4-dev`.

```sh
cmake --preset linux-gtk4
cmake --build --preset linux-gtk4
ctest --preset linux-gtk4
./build/linux-gtk4/examples/basic/grain_basic
```

For a custom build directory, use `-DGRAIN_LINUX_BACKEND=GTK4`; select `X11` for
the existing backend. GTK4 chooses the display backend at runtime; set
`GDK_BACKEND=wayland` or `GDK_BACKEND=x11` to select it explicitly when available.

The GTK4 backend supports native path fills and strokes (including Bézier curves),
transforms, saved graphics state, resize/focus/close
events, and exits the application loop when its last window closes. Keyboard,
text, and pointer event forwarding are not implemented yet. GTK initialization,
window operations, and the application loop must run on the main thread. GTK4
callback exceptions are rethrown by `App::run()`.

GTK4 receives GSK fill/stroke nodes and selects the renderer. Grain does not
rasterize GTK4 views with Cairo; the separate X11 backend still uses Cairo.

GTK4 integration tests run with Xvfb when `xvfb-run` is installed. They check
native render nodes and pixels for fills, strokes, curves, and transforms using
GTK's software renderer. They exercise the X11 display backend; testing native
Wayland requires a Wayland session. To also test the OpenGL renderer:

```sh
GDK_BACKEND=x11 GSK_RENDERER=gl GTK_A11Y=none xvfb-run -a ./build/linux-gtk4/tests/GrainGtk4Tests
```
