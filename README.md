# FrameTimingTest

[![Windows (MSVC)](https://github.com/Casqade/FrameTimingTest/actions/workflows/windows-build-msvc.yml/badge.svg)](https://github.com/Casqade/FrameTimingTest/actions/workflows/windows-build-msvc.yml)
[![Windows (MSYS2)](https://github.com/Casqade/FrameTimingTest/actions/workflows/windows-build-msys2.yml/badge.svg)](https://github.com/Casqade/FrameTimingTest/actions/workflows/windows-build-msys2.yml)
[![Ubuntu](https://github.com/Casqade/FrameTimingTest/actions/workflows/ubuntu-build.yml/badge.svg)](https://github.com/Casqade/FrameTimingTest/actions/workflows/ubuntu-build.yml)
[![macOS](https://github.com/Casqade/FrameTimingTest/actions/workflows/macos-build.yml/badge.svg)](https://github.com/Casqade/FrameTimingTest/actions/workflows/macos-build.yml)
[![GitHub Releases](https://img.shields.io/github/release/Casqade/FrameTimingTest.svg)](https://github.com/Casqade/FrameTimingTest/releases/latest)

---


## About the project

Ever since I started programming videogames, one of my biggest concerns
has always been about handling frame delta time properly. When done incorrectly,
it inevitably leads to very annoying stuttering, as described in
[Elusive Frame Timing](https://medium.com/@alen.ladavac/the-elusive-frame-timing-168f899aec92)
and
[Myths and Misconceptions of Frame Pacing](https://youtu.be/_zpS1p0_L_o).

Unfortunately, I still haven't found any solution which can guarantee
smooth experience on any system & hardware combination. And I probably
never will, in light of the conclusions from above-mentioned sources.

Nevertheless, I wrote a little program for
experimenting with different frame timing techniques.


## Demo

This simple demo visualses how different combinations of delta time calculations
and window & vsync modes can impact perceived framerate smoothness.

Parameters for the demo are passed as command line arguments, so it's best
to just edit & execute a corresponding
[script](https://github.com/Casqade/FrameTimingTest/tree/main/scripts).

In terms of visuals, my program is similar to
[UFO Test stuttering demo](https://www.testufo.com/stutter),
where you see a rectangle/line moving across the screen.
If you experience stuttering even with
[smooth option](https://www.testufo.com/stutter#demo=smooth)
enabled, you probably won't be able to achieve smooth
rectangle movement on your hardware even in my demo.
