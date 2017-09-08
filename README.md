# ofxFaust(ALPHA)

__Now under construction on master. Example won't be built__

Faust addon for Openframeworks

FAUST (Functional Audio Stream) is a functional programming language specifically designed for real-time signal processing and synthesis.
[http://faust.grame.fr/](http://faust.grame.fr/about/)

## Dependency

### Faust

<https://github.com/grame-cncm/faust>

ofxFaustGui.h is not just header file but **Faust Architecture File**.

```sh
cd ~/openFrameworks/myApps/example_ofxFaustGui
faust sample.dsp -a ~/openFrameworks/addons/ofxFaust/src/ofxFaustGui.h -o mydsp.cpp
```

Faust compiler convert .dsp file into mydsp.cpp with ofxFaustGui.h.

### ofxGui

to build GUI.

## Environment

tested on only Mac OSX10.11.6,Xcode8.2.1

## current feature

Integration with ofxGui.

## Todo

- Showing SVG diagram via ofxSVG.
- Write examples.


## author

[Matsuura Tomoya](matsuuratomoya.com)

GPL License.
