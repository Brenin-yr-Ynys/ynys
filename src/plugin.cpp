#include "plugin.hpp"
#include "mmcmidi.hpp"



Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	// p->addModel(modelMyModule);
	p->addModel(modelKami);
	p->addModel(modelStyleGuide);
	p->addModel(modelBlank);
	p->addModel(modelCloc);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
	mmcmidiInit();

}
