# Zoxnoxious VCV Rack Native Modules

The Zoxnoxious "Beyond Obnoxious" VCV Rack-native modules live here.  These provide alternatives to the hardware versions in the [Zoxnoxious](https://github.com/brer-rabbit/zoxnoxious) github repo.

<a id="modules"></a>
## Modules

The TL;DR description of the modules:

* Pole Dancer: a multimode/morphing 4-pole filter

* Personality: filter definition for the Pole Dancer

* Architect: Expander for Personality to define/explore new filters

* Morphscope: Expander for Pole Dancer to show filter response

## Detailed Descriptions

### Pole Dancer

#### General Concept

The Pole Dancer and the Personality introduce a new patch point to VCV Rack: filter topology.  This is a 5-channel patch cord that is intended to be manipulated by other VCV Rack modules: crossfaders, switches, lag processors, etc.  The intended signal flow is "filter topology" originates in a Personality module, may cross through processing, and terminates in the Pole Dancer module.

![IM](doc/PoleDancer.png)

The Pole Dancer is a 4-pole filter.  By itself, it's a 4-pole lowpass filter.  Controls are very similar to any other filter.  The notable difference with the filter is the "Filter Topology" input.  This is intended to be patched from a Personality module: either directly or indirectly via other VCV Rack modules.

### Personality

![IM](doc/Personality.png)

The Personality module defines the filter type for the Pole Dancer.  Many filter types are available as presets: right-click the module to access presets.  Once loaded the filter name is displayed on the module (this is an editable field).  The Filter Topology output can be patched straight to the Pole Dancer or through other modules to affect crossfade, switching, or otherwise morphing filter mode.

### Morphscope

![IM](doc/Morphscope.png)

The Morphscope is available via right-click menu on the Pole Dancer.  This module provides visual feedback on the filter state.  Frequency and phase response are available via the "Bode" setting on the analyzer.  A pole/zero s-plane plot is available as well (more appropriate for this virtual version: Z-plane is missing and is on the TBD list).

### Architect

![IM](doc/Architect.png)

The Architect is available via right-click menu on the Personality.  Similar to Morphscope, a visual feedback of filter response is provided.  Notably different is Architect provides five trim pots to control filter pole mix levels.  The levels are sent back to Personality where they can drive Pole Dancer.  The intended workflow is to set controls in Architect, name the field in Personality, and save the result as a preset.

