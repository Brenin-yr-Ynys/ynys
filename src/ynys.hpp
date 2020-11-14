
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
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_stop_on.svg")));
    addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_stop_off.svg")));
	}

  void off() {
    if (frames.size()>1) {
      sw->setSvg(frames[1]);
      fb->dirty = true;
      lit = false;
    }
  }

  void on() {
    if (!frames.empty()) {
      sw->setSvg(frames[0]);
      fb->dirty = true;
      lit = true;
    }
  }

  bool isOn() {
    return lit;
  }
};
