
struct ModalSwitch : SvgSwitch {

};

struct ArtPlaySwitch : ModalSwitch {

  ArtPlaySwitch() {
		shadow->opacity = 0;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_play_off.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_play_on.svg")));
	}
};

struct ArtStopSwitch : ModalSwitch {

  bool lit;

  ArtStopSwitch() {
		shadow->opacity = 0;
    lit = false;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_stop_off.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_stop_on.svg")));
	}

  void on() {
    if (frames.size()>1) {
      sw->setSvg(frames[1]);
      fb->dirty = true;
      lit = true;
    }
  }

  void off() {
    if (!frames.empty()) {
      sw->setSvg(frames[0]);
      fb->dirty = true;
      lit = false;
    }
  }

  bool isOn() {
    return lit;
  }
};
