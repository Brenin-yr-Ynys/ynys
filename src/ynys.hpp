struct ArtPlaySwitch : SVGSwitch {
  ArtPlaySwitch() {
		shadow->opacity = 0;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_play_off.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_play_on.svg")));
	}
};

struct ArtStopSwitch : SVGSwitch {
  ArtStopSwitch() {
		shadow->opacity = 0;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_stop_off.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_stop_on.svg")));
	}
};
