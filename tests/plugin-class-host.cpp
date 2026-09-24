// SPDX-License-Identifier: GPL-2.0-or-later
#include "GUI/Widgets/PluginManager.h"
#include "GUI/Widgets/SpiralPluginGUI.h"
#include "SpiralInfo.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
	if (argc != 5)
	{
		std::cerr << "Usage: plugin-class-host ROOT MODULE-LIST EXPECTED-DSP-COUNT EXPECTED-GUI-COUNT\n";
		return 2;
	}
	std::vector<std::string> modules;
	std::ifstream list(argv[2]);
	std::string line;
	while (std::getline(list, line)) if (!line.empty()) modules.push_back(line);
	PluginManager *manager = PluginManager::Get();
	std::cerr << "Loading " << modules.size() << " modules\n";
	std::vector<int> ids = manager->LoadPlugins(argv[1], modules);
	HostInfo host;
	host.BUFSIZE = 64; host.SAMPLERATE = 44100; host.POLY = 1;
	host.FRAGSIZE = 64; host.FRAGCOUNT = 2; host.PAUSED = true;
	host.GUI_COLOUR = host.GUICOL_Device = 179; host.GUIDEVICE_Box = FL_UP_BOX;
	host.SCOPE_BG_COLOUR = host.SCOPE_FG_COLOUR = host.SCOPE_SEL_COLOUR = 0;
	host.SCOPE_IND_COLOUR = host.SCOPE_MRK_COLOUR = 0;
	host.AUDIOCLIENT = "dummy";
	const bool createGUI = std::getenv("SSM_TEST_NO_DISPLAY") == NULL;
	if (createGUI) Fl::w(); // Open the display before requesting fonts or pixmaps.
	unsigned dspCount = 0, guiCount = 0, createdDSP = 0, createdGUI = 0;
	for (size_t i = 0; i < ids.size(); ++i)
	{
		const HostsideInfo *info = manager->GetPlugin(ids[i]);
		if (!info || !info->HasDSP()) continue;
		++dspCount;
		if (info->GUIClass) ++guiCount;
		if (std::getenv("SSM_TEST_NO_HARDWARE") && (info->Name == "Midi" || info->Name == "Jack"))
			continue;
		std::cerr << "Creating " << info->Name << "\n";
		SpiralPlugin *plugin = info->CreateDSPInstance();
		assert(plugin);
		plugin->SetBlockingCallback(NULL);
		const SSMPlugins::DeviceDefinition *definition =
			dynamic_cast<const SSMPlugins::DeviceDefinition *>(&plugin->ClassInfo());
		assert(definition && definition->id == ids[i]);
		const PluginInfo &defaults = plugin->GetPluginInfo();
		assert(definition->portCount == defaults.PortTips.size());
		for (size_t p = 0; p < definition->portCount; ++p)
		{
			assert(defaults.PortTips[p] == definition->defaultPorts[p].name);
			assert(definition->defaultPorts[p].input == (p < static_cast<size_t>(defaults.NumInputs)));
		}
		// Hardware connection tests belong on a machine with those devices.
		if (info->Name == "Midi" || info->Name == "Jack") plugin->SpiralPlugin::Initialise(&host);
		else plugin->Initialise(&host);
		if (info->Name == "Mixer")
		{
			assert(definition->defaultPorts[0].immutable && definition->defaultPorts[1].immutable);
			assert(!definition->defaultPorts[2].immutable && !definition->defaultPorts[3].immutable);
			std::istringstream saved("2 6 1 1 1 1 1 1 ");
			plugin->StreamIn(saved);
			assert(plugin->GetPluginInfo().NumInputs == 6 && definition->portCount == 5);
		}
		if (info->Name == "Amp" || info->Name == "Mixer")
		{
			Sample signal(host.BUFSIZE);
			for (int n = 0; n < host.BUFSIZE; ++n) signal.Set(n, 0.25f);
			assert(plugin->SetInput(0, &signal));
			plugin->Execute();
			Sample *output = NULL;
			assert(plugin->GetOutput(0, &output) && output);
			assert(std::fabs((*output)[0] - 0.25f) < 0.0001f);
			plugin->SetInput(0, NULL);
		}
		SpiralGUIType *gui = createGUI ? info->CreateGUI(plugin) : NULL;
		if (info->GUIClass)
		{
			assert(info->GUIClass->createUI);
			if (createGUI) assert(gui);
		}
		if (gui)
		{
			++createdGUI;
			SpiralPluginGUI *panel = dynamic_cast<SpiralPluginGUI *>(gui);
			assert(panel);
			panel->UpdateValues(plugin);
			delete gui;
		}
		delete plugin;
		++createdDSP;
	}
	manager->UnloadAll();
	std::cout << "Registered " << dspCount << " DSP and " << guiCount << " GUI; created "
		<< createdDSP << " DSP and " << createdGUI << " GUI instances\n";
	assert(dspCount == static_cast<unsigned>(std::atoi(argv[3])));
	assert(guiCount == static_cast<unsigned>(std::atoi(argv[4])));
	return 0;
}
