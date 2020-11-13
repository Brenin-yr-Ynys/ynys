#include "plugin.hpp"

#include "app/MidiWidget.hpp"
#include "ynys.hpp"


using namespace ::rack;

struct Cloc : Module {
	enum ParamIds {
		STOP_PARAM,
		PLAY_PARAM,
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

	uint32_t clock = 0;
	int clockDivision = 24;

	dsp::PulseGenerator bpmPulse;
	dsp::PulseGenerator runPulse;
	dsp::PulseGenerator resetPulse;

	Cloc() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		onReset();
	}

	void onReset() override {
		//channels = 1;
		//polyMode = ROTATE_MODE;
		clockDivision = 24;
		panic();
		midiInput.reset();
	}

	/** Resets performance state */
	void panic() {

	}

	void process(const ProcessArgs& args) override {
		midi::Message msg;
		while (midiInput.shift(&msg)) {
			if (msg.getStatus() == 0xf) {
				processTiming(msg);

			}
		}

		outputs[BPM_OUTPUT].setVoltage(bpmPulse.process(args.sampleTime)? 10.f : 0.f);
	}

	void processTiming(midi::Message msg) {
		switch (msg.getChannel()) {
			// Timing - need to chnage all this TODO
			case 0x8: {
				if (clock % clockDivision == 0) {
				//	bpmPulse.trigger(1e-3);
				}
				bpmPulse.trigger(1e-3);
				clock++;
			} break;
			// Start
			case 0xa: {
				//startPulse.trigger(1e-3);
				clock = 0;
			} break;
			// Continue
			case 0xb: {
				//continuePulse.trigger(1e-3);
			} break;
			// Stop
			case 0xc: {
				//stopPulse.trigger(1e-3);
				clock = 0;
			} break;
			default: break;
		}
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

		addParam(createParam<ArtStopSwitch>(mm2px(Vec(2.2, 55.0)), module, Cloc::STOP_PARAM));
		addParam(createParam<ArtPlaySwitch>(mm2px(Vec(19.2, 55.0)), module, Cloc::PLAY_PARAM));

		MidiWidget* midiWidget = createWidget<MidiWidget> (mm2px (Vec(3.41891, 21.917)));
		midiWidget->box.size = mm2px (Vec (33.840, 28));
		midiWidget->setMidiPort(module ? &module->midiInput : NULL);
		addChild (midiWidget);
		//return midiAInWidget;


		// mm2px(Vec(34.396, 15.875))
		//addChild(createWidget<Widget>(mm2px(Vec(3.122, 21.917))));
		// mm2px(Vec(34.396, 15.875))
		//addChild(createWidget<Widget>(mm2px(Vec(3.122, 72.188))));
	}
};


Model* modelCloc = createModel<Cloc, ClocWidget>("Cloc");
