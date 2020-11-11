#include "plugin.hpp"


struct Kami : Module {
	enum ParamIds {
		PATH1766_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PATH1762_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		PATH1764_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		PATH1768_LIGHT,
		PATH1772_LIGHT,
		NUM_LIGHTS
	};

	Kami() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PATH1766_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct KamiWidget : ModuleWidget {
	KamiWidget(Kami* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/kami.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(12.7, 85.417)), module, Kami::PATH1766_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 13.979)), module, Kami::PATH1762_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 111.875)), module, Kami::PATH1764_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(7.408, 98.646)), module, Kami::PATH1768_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.992, 98.646)), module, Kami::PATH1772_LIGHT));
	}
};


Model* modelKami = createModel<Kami, KamiWidget>("kami");