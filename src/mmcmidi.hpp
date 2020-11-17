#pragma once
#include <common.hpp>


namespace rack {

struct MMCMessage : midi::Message {
//	uint8_t mmc_byte = 0;
  uint8_t getMMC() {
    return bytes[2];
  }
	bool isMMC() {
    return (bytes[0] == 0xf0) && (bytes[1] == 0x7f);
  }
  bool isClock() {
    //status 0xf && channel 0x8
    return (getStatus() == 0xf) && (getChannel() == 0x8);
  }
};

void mmcmidiInit();

} // namespace rack
