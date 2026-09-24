// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef SSM_PLUGIN_GUI_EXPORTS_H
#define SSM_PLUGIN_GUI_EXPORTS_H
#include "PluginDefinitions.h"

// Each FLTK module uses its own internal factory, avoiding C-export interposition.
#define SSM_EXPORT_GUI_CLASS(ID, KEY, NAME, CATEGORY, FACTORY) \
namespace \
{ \
	const SSMPlugins::PluginUIDefinition &ClassDefinition() \
	{ \
		static const SSMPlugins::PluginID dependencies[] = \
		{ \
			SSMPlugins::PluginID(SSMPlugins::PluginTypes::DSP, ID) \
		}; \
		static const SSMPlugins::PluginUIDefinition definition(ID, KEY, NAME, CATEGORY, \
			FACTORY, dependencies, sizeof(dependencies) / sizeof(dependencies[0])); \
		return definition; \
	} \
} \
extern "C" SSMPlugins::PluginID SpiralPlugin_Initialize(SSMPlugins::PluginRegistry *registry) \
{ \
	if (!registry || registry->Register(SSMPlugins::PluginUIDefinition::RootClass()) == -1 || \
		registry->Register(ClassDefinition()) == -1) \
		return SSMPlugins::PluginID(); \
	return ClassDefinition().Identity(); \
}
#endif
