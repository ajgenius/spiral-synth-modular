// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef SSM_PLUGIN_DEFINITIONS_H
#define SSM_PLUGIN_DEFINITIONS_H
#include "PluginClass.h"
class SpiralPlugin;
class SpiralGUIType;

namespace SSMPlugins
{
	struct PortDefinition
	{
		const char *name;
		bool input;
		bool immutable;
	};

	struct DeviceDefinition: PluginDefinition
	{
		const PortDefinition *defaultPorts;
		size_t portCount;
		DeviceDefinition(int id, const char *key, const char *name, const char *category,
			Factory factory, const PortDefinition *ports = NULL, size_t count = 0,
			PluginID parent = PluginID(PluginTypes::DSP, -2),
			const PluginID *dependencies = NULL, size_t dependencyCount = 0):
			PluginDefinition(PluginTypes::DSP, id, key, parent, name, category, factory, dependencies, dependencyCount),
			defaultPorts(ports), portCount(count) {}
		virtual bool Validate(std::string &error) const;
	};

	// The FLTK family retains its widget hierarchy and receives the live DSP.
	struct PluginUIDefinition: PluginDefinition
	{
		typedef SpiralGUIType *(*UIFactory)(SpiralPlugin *);
		UIFactory createUI;
		PluginUIDefinition(int id, const char *key, const char *name, const char *category,
			UIFactory factory, const PluginID *dependencies, size_t count,
			PluginID parent = PluginID(PluginTypes::GUI, -2)):
			PluginDefinition(PluginTypes::GUI, id, key, parent, name, category, NULL, dependencies, count),
			createUI(factory) {}
		static const PluginUIDefinition &RootClass();
	};
}
#endif
