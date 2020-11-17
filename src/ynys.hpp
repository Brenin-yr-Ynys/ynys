
struct ModalSwitch : SvgSwitch {
  bool lit = false;

  unsigned int offFrame = 0;
  unsigned int onFrame = 1;

  void on() {
    if (frames.size()>onFrame) {
      sw->setSvg(frames[onFrame]);
      fb->dirty = true;
      lit = true;
    }
  }

  void off() {
    if (frames.size()>offFrame) {
      sw->setSvg(frames[offFrame]);
      fb->dirty = true;
      lit = false;
    }
  }

  bool isOn() {
    return lit;
  }
};

struct ArtPlaySwitch : ModalSwitch {

  bool flashing;

  ArtPlaySwitch() {
  //  lit = false;
    flashing = false;
		shadow->opacity = 0;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_play_off.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_play_on.svg")));
	}
};

struct ArtStopSwitch : ModalSwitch {

  ArtStopSwitch() {
    lit = true;
    onFrame = 0;
    offFrame = 1;
		shadow->opacity = 0;
    addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_stop_on.svg")));
    addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/art_stop_off.svg")));
	}

};
