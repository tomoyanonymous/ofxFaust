# ofxFaust(ALPHA)

Faust addon for Openframeworks

FAUST (Functional Audio Stream) is a functional programming language specifically designed for real-time signal processing and synthesis.
[http://faust.grame.fr/](http://faust.grame.fr/about/)

## Dependency

### libfaust.a
This repository contains libfaust binary built on Mac OS X10.11.4 with LLVM3.5 in libs/libfaust/lib.
The binary is actually hosted via git-lfs. After install of git lfs,

```bash
$ git lfs pull
```
to download it.(Also you can replace text pointer to your own binary by hand.)

### libLLVM

For live compilation.
add to "Link with binaries" together with libfaust.

### ofxGui

to build GUI.

## Environment

tested on only Mac OSX10.11.4,Xcode7.3.1

## current feature

Integration with ofxGui.
Signal Processing is now via raw libfaust api.

## Todo

- Wrap signal processing part.
- Crossfade in live recompilation like FaustLive.
- Showing SVG diagram via ofxSVG.
- Write examples.


## author

[Matsuura Tomoya](matsuuratomoya.com)

MIT License.
