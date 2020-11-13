#include "plugin.hpp"

#include "app/MidiWidget.hpp"

using namespace ::rack;

struct Cloc : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		RUN_OUTPUT,
		BPM_OUTPUT,
		RESET_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		RESET_LIGHT,
		RUN_LIGHT,
		BPM_LIGHT,
		NUM_LIGHTS
	};

	midi::InputQueue midiInput;

	Cloc() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs& args) override {
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "midi", midiInput.toJson());
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* midiJ = json_object_get(rootJ, "midi");
		if (midiJ)
			midiInput.fromJson(midiJ);
	}
};



struct ClocWidget : ModuleWidget {
	ClocWidget(Cloc* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Cloc.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.028, 110.552)), module, Cloc::RUN_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.6, 110.715)), module, Cloc::BPM_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.383, 110.93)), module, Cloc::RESET_OUTPUT));

		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(7.383, 96.0)), module, Cloc::RESET_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(20.028, 96.0)), module, Cloc::RUN_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(32.6, 96.0)), module, Cloc::BPM_LIGHT));

		MidiWidget* midiWidget = createWidget<MidiWidget> (mm2px (Vec(3.41891, 21.917)));
		midiWidget->box.size = mm2px (Vec (33.840, 28));
		midiWidget->setMidiPort(module ? &module->midiInput : NULL);
		addChild (midiWidget);
		//return midiAInWidget;


		// mm2px(Vec(34.396, 15.875))
		addChild(createWidget<Widget>(mm2px(Vec(3.122, 21.917))));
		// mm2px(Vec(34.396, 15.875))
		addChild(createWidget<Widget>(mm2px(Vec(3.122, 72.188))));
	}
};


Model* modelCloc = createModel<Cloc, ClocWidget>("Cloc");
