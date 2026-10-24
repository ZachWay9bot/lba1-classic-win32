# LBA1 native Win32/OpenGL port – M1

This is **an initial Windows platform layer**, not yet a playable game.
No original DOS sources are changed. The independent M1 build animates a
640x480 indexed framebuffer, converted through the 256-colour VGA palette
to an OpenGL texture using nearest-neighbour scaling and letterboxing.

## Build 32-bit Windows binary (Visual Studio 2022)

Install Visual Studio Desktop development with C++, Git, and CMake.
On first configure, CMake will fetch pinned SDL3 3.4.18 if not installed.

```powershell
cmake -S port/win32 -B build-win32 -G "Visual Studio 17 2022" -A Win32
cmake --build build-win32 --config Release --parallel
.\build-win32\Release\LBA1-Win32-M1.exe
```

SDL3.dll is copied alongside the executable. The separate GitHub Actions
workflow builds a 32-bit EXE and uploads it as an artifact.
ESC exits; F11 switches desktop fullscreen.

## Engine integration in M2

The interface `video_gl.h` accepts the original indexed frame `Log` via
`lba_video_present()`; send palette changes to
`lba_video_set_palette(video, rgb_triplets, 1)` for 6-bit VGA DAC values.
Initialization takes `Screen_X` and `Screen_Y`.

The old CMakeLists creates DOS-only targets. Even its translated C build
still links a number of assembly graphics modules. Input, timers, path
handling, memory and audio also need Win32 implementations before game
entrypoint, menus, scenes and saving/loading can work.

## Scope and limitations

- SDL3 + **OpenGL 2.1 compatibility** for first milestone (not 3.3 core).
- Animating test pixels only; not yet a playable LBA1 executable.
- No original game data included; copyrighted game assets are separate.
- No DOS/WATCOM source edits, new graphics features or gameplay changes.
- The original project is released under GPL; retain licence in the fork.
