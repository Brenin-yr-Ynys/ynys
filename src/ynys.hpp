
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

struct FlashingSwitch : ModalSwitch {
  bool flashing = false;
  unsigned int count = 0;
  unsigned int nextFrame = offFrame;

  void step() override {
    ModalSwitch::step();
    if (flashing && (count > APP->window->getLastFrameRate()/3)) {
        count = 0;
        sw->setSvg(frames[nextFrame]);
        fb->dirty = true;
        nextFrame = nextFrame ? 0 : 1;
        count = 0;
    }
    count++;
  }

  void on() {
    ModalSwitch::on();
    flashing = false;
  }

  void off() {
    ModalSwitch::off();
    flashing = false;
  }

  void flash() {
    nextFrame = offFrame;
    count = 0;
    flashing = true;
  }

  bool isFlashing() {
    return flashing;
  }

};

struct ArtPlaySwitch : FlashingSwitch {

  ArtPlaySwitch() {
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
