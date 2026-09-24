// SPDX-License-Identifier: GPL-2.0-or-later
#include "GUI/Widgets/PluginClass.h"
#include <cassert>
using namespace SSMPlugins;
int main()
{
	PluginRegistry registry;
	PluginDefinition dsp(PluginTypes::DSP, 0, "Output", PluginID(PluginTypes::Root, 0), "Output", "Audio");
	PluginID deps[] = {PluginID(PluginTypes::DSP, 0)};
	PluginDefinition gui(PluginTypes::GUI, 0, "Output", PluginID(PluginTypes::Root, 0), "Output", "Audio", NULL, deps, 1);
	assert(registry.Register(gui) == -1 && registry.WaitingForDependencies());
	assert(registry.Register(dsp) == 0);
	assert(registry.Register(gui) == 0);
	assert(registry.Find(gui.Identity())->Parent()->Subclasses().size() == 2);
	assert(!registry.Discard(dsp.Identity()));
	assert(registry.Discard(gui.Identity()));
	assert(registry.Discard(dsp.Identity()));
	PluginID self[] = {PluginID(PluginTypes::DSP, 5)};
	PluginDefinition invalid(PluginTypes::DSP, 5, "Self", PluginID(PluginTypes::Root, 0), "Self", "", NULL, self, 1);
	assert(registry.Register(invalid) == -1 && !registry.WaitingForDependencies());
	registry.Reset();
	assert(registry.Register(dsp) == 0);
	assert(registry.Register(dsp) == 0);
	PluginDefinition duplicate(PluginTypes::DSP, 0, "Other", PluginID(PluginTypes::Root, 0), "Other", "");
	assert(registry.Register(duplicate) == -1);
	return 0;
}
