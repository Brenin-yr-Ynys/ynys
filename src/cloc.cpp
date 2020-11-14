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

	enum State {
		STOP,
		PLAY,
		PAUSE
	};



	midi::InputQueue midiInput;


	dsp::PulseGenerator bpmPulse;
	dsp::PulseGenerator runPulse;
	dsp::PulseGenerator resetPulse;

	dsp::SchmittTrigger stopTrig, playTrig;

	State state;

	ArtStopSwitch* artStopSwitch;
	ArtPlaySwitch* artPlaySwitch;

	Cloc() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		onReset();
	}

	void setWidgets(ArtStopSwitch* stopSw, ArtPlaySwitch* playSw) {
		artPlaySwitch = playSw;
		artStopSwitch = stopSw;
	}

	void onReset() override {
		panic();
		midiInput.reset();
		state = STOP;
		if (artStopSwitch) artStopSwitch->on();
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
		processButtons();
	}

	void processTiming(midi::Message msg) {
		switch (msg.getChannel()) {
			// Clock
			case 0x8: {
				bpmPulse.trigger(1e-3);
			} break;
			// Start
			case 0xa: {
				artStopSwitch->off();
				state = PLAY;
			} break;
			// Continue
			case 0xb: {
				state = PLAY;
			} break;
			// Stop
			case 0xc: {
				artStopSwitch->on();
				if (state == PLAY) state = PAUSE;
				else state = STOP;
			} break;
			default: break;
		}
	}

	void processButtons() {

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

		ArtStopSwitch* artStopSwitch = createParam<ArtStopSwitch>(mm2px(Vec(2.2, 55.0)), module, Cloc::STOP_PARAM);
		ArtPlaySwitch* artPlaySwitch = createParam<ArtPlaySwitch>(mm2px(Vec(19.2, 55.0)), module, Cloc::PLAY_PARAM);
		addParam(artStopSwitch);
		addParam(artPlaySwitch);
		module->setWidgets(artStopSwitch, artPlaySwitch);

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
