### ofxDatGui Changelog

--
**v1.20** –– 04-07-16

* fonts are now managed by [ofxSmartFont](https://github.com/braitsch/ofxSmartFont)
* fix for [2dPad setValue & setPosition not working as expected](https://github.com/braitsch/ofxDatGui/issues/45)
* fix for [folders not retaining color state after a theme is applied](https://github.com/braitsch/ofxDatGui/issues/36)
* fix for [ambiguous placeholder conflict with boost](https://github.com/braitsch/ofxDatGui/issues/49)
* fix for [sliders dispatching events on mousedown & mousedrag](https://github.com/braitsch/ofxDatGui/issues/63)
* fix for [ofGetWidth & ofGetHeight returning incorrect values on retina displays](https://github.com/openframeworks/openFrameworks/pull/4858)
* fix for [label components not respecting alignment settings](https://github.com/braitsch/ofxDatGui/commit/3bbb1591c01f87960e3a15aa62ea5ab16d10fa07)
* fix for [scrollview not reseting itself properly after items deleted](https://github.com/braitsch/ofxDatGui/commit/9ab25457cac1d59975cc3e330beaf9f620970aef)
* adding [gui->isMouseDown()](https://github.com/braitsch/ofxDatGui/issues/50)
* adding [ofxDatGui::setAssetPath()](https://github.com/braitsch/ofxDatGui/issues/51) to allow for [configurable asset directory](https://github.com/braitsch/ofxDatGui/issues/53)
* adding [folder->addSlider(ofParameter \<float\>)](https://github.com/braitsch/ofxDatGui/pull/43)
* adding [gui->addFolder()](https://github.com/braitsch/ofxDatGui/pull/40)
* adding [gui->setPosition(ofxDatGuiAnchor anchor)](https://github.com/braitsch/ofxDatGui/commit/eefc6bdfa79489fb9a152fa7b979a2192e560816)
* adding [gui anchors BOTTOM LEFT & BOTTOM RIGHT](https://github.com/braitsch/ofxDatGui/pull/61)
* adding [gui->collapse, gui->expand and gui->toggle](https://github.com/braitsch/ofxDatGui/issues/57)
* adding [dropdown->toggle() & folder->toggle()](https://github.com/braitsch/ofxDatGui/commit/815c8fd4e678c0068430fb7a58555097deb958de)
* [gui & component update loop optimizations](https://github.com/braitsch/ofxDatGui/commit/75ad03c6a0065a957c8105799b32717d83ba5344)
* addToggle() now returns an [ofxDatGuiToggle instance instead of ofxDatGuiButton](https://github.com/braitsch/ofxDatGui/pull/43)
* gui mouse events are now [detected before the app receives them](https://github.com/braitsch/ofxDatGui/issues/50)
* slider->setPrecision() now allows [flag to not truncate precision of underlying value](https://github.com/braitsch/ofxDatGui/issues/42)
* [all icons are now shared_ptrs](https://github.com/braitsch/ofxDatGui/pull/52) thanks [@mattfelsen!](https://github.com/mattfelsen)

--
**v1.10** –– 01-28-16

* adding [scrollview](http://braitsch.github.io/ofxDatGui/index.html#scrollviews) component
* [ofParameter](http://openframeworks.cc/documentation/types/ofParameter/) support (sliders only)
* slider->bind() pass by reference
* slider->setMin & slider->setMax()
* draw methods refactored
* ofxDatGuiTextInput->setInputType()
* adding support for [component borders](http://braitsch.github.io/ofxDatGui/index.html#api)
* assets moved into **/bin/data/ofxbraitsch**

--
**v1.00** –– 01-04-16

* style templates rewritten into much simpler [themes](http://braitsch.github.io/ofxDatGui/themes.html)
* setOrigin -> setPosition
* setAlignment -> setLabelAlignment
* setWidth now also takes a value for the label width
* eliminating resolution specific themes
* textfields & fonts are now created on the stack
* adding component styling convenience methods

--
**v0.90** –– 11-29-15

* cursor added to ofxDatGuiInputField (thanks Jesse!)
* range sliders: limit range to integers
* range sliders: specify custom floating point precisions
* ofxDatGuiTextInputField->isKeyValid() rewrite

--
**v0.80** –– 10-23-15

* new documentation site!
* support for overlapping guis
* gui focus management rewrite
* matrices can now function as radio button groups

--
**v0.70** –– 10-10-15

* adding value plotters & waveform monitors
* introducing templates for component customization
* experimental variable binding on range sliders

--
**v0.60** –– 09-16-15

* color pickers!
* adding button matrices and 2d coordinate pads
* all components decoupled from gui container and can now stand alone

--
**v0.50** –– 08-31-15

* support for multiple dropdowns
* adding folders to group related components

--
**v0.40** –– 08-28-15

* range sliders now accept keyboard input
* text rendering broken out into its own font class
* support for window resizing
* adding draggable header to allow gui to be repositioned

--
**v0.30** –– 08-24-15

* adding text input fields
* support for transparent gui backgrounds
* adding footer to expand/collapse controls
* adding toggle button and support for retina displays

--
**v0.20** –– 08-21-15

* experimenting with dropdown menus
* adding ofxDatGuiEvents to dispatch state changes

--
**v0.10** –– 08-18-15

* capturing mouse events
* simple range sliders & buttons working 
* experimenting with anchors / corner registration

--