# Zoxnoxious VCV Rack Native Modules

The Zoxnoxious "Beyond Obnoxious" VCV Rack-native modules live here.  These provide alternatives to the hardware versions in the [Zoxnoxious](https://github.com/brer-rabbit/zoxnoxious) github repo.

<a id="modules"></a>
## Modules

The TL;DR description of the modules:

* Pole Dancer: a multimode/morphing 4-pole filter

* Personality: defines how the filter behaves

* Architect: Expander for Personality to define/explore new filters

* Morphscope: Expander for Pole Dancer to show filter response

## Quick Start

* Add Pole Dancer
* Add Personality
* Load a preset on Personality
* Patch Personality's Filter Topology output to Pole Dancer
* Send audio into Pole Dancer
* Add additional Personality modules, and processing, for morphing responses

## Detailed Descriptions

### Pole Dancer

The Pole Dancer is not your ordinary filter.  Nor an ordinary multimode filter.  The Pole Dancer is able to continuously morph filter responses: go from a growl of a 4-pole lowpass to a notched highpass to a phaser-effect with an allpass filter.  All sequenced in VCV Rack.

![IM](doc/PoleDancer.png)

The Pole Dancer is a 4-pole filter.  By itself, it's a 4-pole lowpass filter.  Controls are very similar to any other filter.  The notable difference with the filter is the "Filter Topology" input.  This is intended to be patched from a Personality module: either directly or indirectly via other VCV Rack modules.

Style note: these modules style were inspired by Nuclear Instrumentation Modules (NIM).  The frequency cutoff is a turns counter knob, displaying the current octave, with one rotation being exactly one octave.  Connectors are BNC styled.  Slotted knurled screws round out the hardware styling.

### Personality

![IM](doc/Personality.png)

The Personality module defines the filter type for the Pole Dancer.  Many filter types are available as presets: right-click the module to access presets.  Once loaded the filter name is displayed on the module (this is an editable field).  The output can be patched straight to the Pole Dancer or through other modules to affect crossfade, switching, or otherwise morphing filter mode.

### Morphscope

![IM](doc/Morphscope.png)

The Morphscope is available via right-click menu on the Pole Dancer.  This module provides visual feedback on the filter state.  Frequency and phase response are available via the "Bode" setting on the analyzer.  A pole/zero s-plane plot is available as well.

### Architect

![IM](doc/Architect.png)

The Architect is available via right-click menu on the Personality.  Architect provides a visual feedback of filter response.  Usage is different from the Morphscope: Architect provides five trim pots to control filter pole mix levels.  The levels are sent back to Personality where they can drive a Pole Dancer filter.  The intended *filter design* workflow is to set controls in Architect, name the field in Personality, and save the result as a preset.  For typical use the Architect's controls may be left as-is.  Do note each channel has a massive 32X gain: don't blow your speakers out!  A little does a lot.


### Usage

The Pole Dancer and the Personality introduce a new patch point via a 5-channel polycable.  This patch point is intended to be manipulated by other VCV Rack modules: because the signal is polyphonic CV, standard Rack utility modules can transform filter responses in ways impossible with traditional filters.  Crossfaders, switches, lag processors, etc are all tools you use to manipulate and morph the filter.

The intended signal flow is "filter topology" originates in one or more Personality modules, may cross through processing, and terminates at the Pole Dancer module, morphing and shaping the filter behavior.

Details on the 5-channel patch cord: each channel provides the level for a specific aspect of filter response.

* Dry signal level
* Filter Pole 1 level
* Filter Pole 2 level
* Filter Pole 3 level
* Filter Pole 4 level

Here are a couple ways one can use the signal with Rack modules.  I'll focus on using Fundamental module for processing since those are standard in VCV Rack.

#### Single Filter Mode

![IM](doc/SetupBasic.png)

Instantiate both a Pole Dancer and a Personality module from the module browser.  Right-click on Personality to find a preset.  Patch the Personality's Filter Topology straight to the Pole Dancer's Personality.


#### Crossfade Filters

![IM](doc/Patch_Crossfade.png)

A crossfade is very easy to setup.  Patch two Personality modules to a crossfade, and the output to the Pole Dancer.

![IM](doc/Patch_Oberheim.png)

One feature of the Oberheim SEM was crossfading between 2-pole highpass to 2-pole lowpass.  That's very easy to emulate with the Pole Dancer.  Just choose the appropriate Personality presets and wire up a crossfade.

#### Switches

![IM](doc/Patch_Switch4.png)

Switching can be used to jump between filter modes.  The Fundamental switch supports 4 inputs; if four is a limitation many switches in the VCV Rack library support more.

![IM](doc/Patch_Switch_Slew.png)

Jumping filter modes might be a bit much.  Including a slew limiter will smooth the transitions between filter modes.

#### Advanced Awesomeness

![IM](doc/Patch_Slew_Single_Pole.png)

To take things to the extreme, instead of slew limiting the entire filter morph, only slew limit a single filter pole's movement.  Use Split to access the individual channels, slew limit one or more, and merge the result back to the Pole Dancer.  Or, slew limit multiple filter poles at different rates.  The Befaco Slew Limiter is a very good option here as well.

#### Audio Rate Modulation

Yes.  At audio rate the Pole Dancer is less of a filter and more of a waveshaper.  Use a VCO to morph filter modes at audio rate and you can get some unique results.

