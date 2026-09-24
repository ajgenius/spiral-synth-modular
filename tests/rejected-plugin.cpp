// SPDX-License-Identifier: GPL-2.0-or-later
#include "GUI/Widgets/PluginClass.h"
#include <cstdlib>
extern "C" const char *SpiralPlugin_GetHostABI()
{
	return "incompatible-test-abi";
}
extern "C" SSMPlugins::PluginID SpiralPlugin_Initialize(SSMPlugins::PluginRegistry *)
{
	// The loader must reject this module without calling its initializer.
	std::abort();
	return SSMPlugins::PluginID();
}
