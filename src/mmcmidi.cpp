#include <rtmidi.hpp>
#include <midi.hpp>
#include <map>

#include "mmcmidi.hpp"

#pragma GCC diagnostic push
#ifndef __clang__
	#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif
#include <rtmidi/rtMidi.h>
#pragma GCC diagnostic pop


namespace rack {


struct MmcMidiInputDevice : midi::InputDevice {
	RtMidiIn* mmcMidiIn;

	MmcMidiInputDevice(int driverId, int deviceId) {
		mmcMidiIn = new RtMidiIn((RtMidi::Api) driverId, "VCV Rack");
		assert(mmcMidiIn);
		mmcMidiIn->ignoreTypes(false, false, false);
		mmcMidiIn->setCallback(midiInputCallback, this);
		mmcMidiIn->openPort(deviceId, "VCV Rack input");
	}

	~MmcMidiInputDevice() {
		mmcMidiIn->closePort();
		delete mmcMidiIn;
	}

	static void midiInputCallback(double timeStamp, std::vector<unsigned char>* message, void* userData) {
		if (!message)
			return;
		if (!userData)
			return;

		MmcMidiInputDevice* midiInputDevice = (MmcMidiInputDevice*) userData;
		if (!midiInputDevice)
			return;

		// allow MMC commands (for
		MMCMessage msg;
		if (message->size() == 6 && (*message)[0] == 0xf0 && (*message)[1] == 0x7f) {
				//DEBUG("Got MMC MIDI Message");
				//DEBUG("Setting value as: %x", (*message)[4]);
				msg.bytes[2] = (*message)[4];
				msg.bytes[1] = (*message)[1];
				msg.bytes[0] = (*message)[0];
		}
		else if (message->size() > 3) {
			//DEBUG("Dropping too long message");
			return;
		}
		else {

			msg.size = message->size();
			for (int i = 0; i < msg.size; i++) {
				msg.bytes[i] = (*message)[i];
			}
		}
		midiInputDevice->onMessage(msg);
	}
};




struct MmcMidiOutputDevice : midi::OutputDevice {
	RtMidiOut* mmcMidiOut;

	MmcMidiOutputDevice(int driverId, int deviceId) {
		mmcMidiOut = new RtMidiOut((RtMidi::Api) driverId, "VCV Rack");
		assert(mmcMidiOut);
		mmcMidiOut->openPort(deviceId, "VCV Rack output");
	}

	~MmcMidiOutputDevice() {
		mmcMidiOut->closePort();
		delete mmcMidiOut;
	}

	void sendMessage(midi::Message message) override {
		mmcMidiOut->sendMessage(message.bytes, message.size);
	}
};


struct MmcMidiDriver : midi::Driver {
	int driverId;
	/** Just for querying MIDI driver information */
	RtMidiIn* mmcMidiIn;
	RtMidiOut* mmcMidiOut;
	std::map<int, MmcMidiInputDevice*> inputDevices;
	std::map<int, MmcMidiOutputDevice*> outputDevices;

	MmcMidiDriver(int driverId) {
		this->driverId = driverId;
		mmcMidiIn = new RtMidiIn((RtMidi::Api) driverId);
		assert(mmcMidiIn);
		mmcMidiOut = new RtMidiOut((RtMidi::Api) driverId);
		assert(mmcMidiOut);
	}

	~MmcMidiDriver() {
		delete mmcMidiIn;
		delete mmcMidiOut;
	}

	std::string getName() override {
		switch (driverId) {
			case RtMidi::UNSPECIFIED: return "Unspecified";
			case RtMidi::MACOSX_CORE: return "Core MIDI MMC";
			case RtMidi::LINUX_ALSA: return "ALSA MMC";
			case RtMidi::UNIX_JACK: return "JACK MMC";
			case RtMidi::WINDOWS_MM: return "Windows MIDI MMC";
			case RtMidi::RTMIDI_DUMMY: return "Dummy MIDI MMC";
			case 666: return "Joes Super MMC";
			default: return "";
		}
	}
	std::vector<int> getInputDeviceIds() override {
		// TODO The IDs unfortunately jump around in MmcMidi. Is there a way to keep them constant when a MIDI device is added/removed?
		int count = mmcMidiIn->getPortCount();
		std::vector<int> deviceIds;
		for (int i = 0; i < count; i++)
			deviceIds.push_back(i);
		return deviceIds;
	}

	std::string getInputDeviceName(int deviceId) override {
		if (deviceId >= 0) {
			return mmcMidiIn->getPortName(deviceId);
		}
		return "";
	}

	midi::InputDevice* subscribeInput(int deviceId, midi::Input* input) override {
		if (!(0 <= deviceId && deviceId < (int) mmcMidiIn->getPortCount()))
			return NULL;
		MmcMidiInputDevice* device = inputDevices[deviceId];
		if (!device) {
			inputDevices[deviceId] = device = new MmcMidiInputDevice(driverId, deviceId);
		}

		device->subscribe(input);
		return device;
	}

	void unsubscribeInput(int deviceId, midi::Input* input) override {
		auto it = inputDevices.find(deviceId);
		if (it == inputDevices.end())
			return;
		MmcMidiInputDevice* device = it->second;
		device->unsubscribe(input);

		// Destroy device if nothing is subscribed anymore
		if (device->subscribed.empty()) {
			inputDevices.erase(it);
			delete device;
		}
	}

	std::vector<int> getOutputDeviceIds() override {
		int count = mmcMidiOut->getPortCount();
		std::vector<int> deviceIds;
		for (int i = 0; i < count; i++)
			deviceIds.push_back(i);
		return deviceIds;
	}

	std::string getOutputDeviceName(int deviceId) override {
		if (deviceId >= 0) {
			return mmcMidiOut->getPortName(deviceId);
		}
		return "";
	}

	midi::OutputDevice* subscribeOutput(int deviceId, midi::Output* output) override {
		if (!(0 <= deviceId && deviceId < (int) mmcMidiOut->getPortCount()))
			return NULL;
		MmcMidiOutputDevice* device = outputDevices[deviceId];
		if (!device) {
			outputDevices[deviceId] = device = new MmcMidiOutputDevice(driverId, deviceId);
		}

		device->subscribe(output);
		return device;
	}

	void unsubscribeOutput(int deviceId, midi::Output* output) override {
		auto it = outputDevices.find(deviceId);
		if (it == outputDevices.end())
			return;
		MmcMidiOutputDevice* device = it->second;
		device->unsubscribe(output);

		// Destroy device if nothing is subscribed anymore
		if (device->subscribed.empty()) {
			outputDevices.erase(it);
			delete device;
		}
	}
};


void mmcmidiInit() {
	//std::vector<RtMidi::Api> rtApis;
//	RtMidi::getCompiledApi(rtApis);
	//for (RtMidi::Api api : rtApis) {
		int driverId = 666;
		midi::Driver* driver = new MmcMidiDriver(driverId);
		midi::addDriver(driverId, driver);
//	}
}

}
// // namespace rack
