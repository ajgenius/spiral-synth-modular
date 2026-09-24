// SPDX-License-Identifier: GPL-2.0-or-later
#include "PluginDefinitions.h"
#include "SpiralPlugin.h"

const SSMPlugins::DeviceDefinition &SpiralPlugin::StaticClass()
{
	static const SSMPlugins::DeviceDefinition definition(-2, "Device", "Device", "", NULL,
		NULL, 0, SSMPlugins::PluginID(SSMPlugins::PluginTypes::Root, 0));
	return definition;
}

const SSMPlugins::PluginUIDefinition &SSMPlugins::PluginUIDefinition::RootClass()
{
	static const PluginUIDefinition definition(-2, "PluginUI", "Plugin UI", "", NULL,
		NULL, 0, PluginID(PluginTypes::Root, 0));
	return definition;
}

bool SSMPlugins::DeviceDefinition::Validate(std::string &error) const
{
	if (portCount && !defaultPorts)
	{
		error = "Missing default ports";
		return false;
	}
	for (size_t i = 0; i < portCount; ++i)
		if (!defaultPorts[i].name || !*defaultPorts[i].name)
		{
			error = "Missing default port name";
			return false;
		}
	return true;
}

void SpiralPlugin::CreateDefaultPorts()
{
	const SSMPlugins::DeviceDefinition *definition =
		dynamic_cast<const SSMPlugins::DeviceDefinition *>(&ClassInfo());
	if (!definition) return;
	m_PluginInfo.NumInputs = m_PluginInfo.NumOutputs = 0;
	m_PluginInfo.PortTips.clear();
	// Public PluginInfo stores inputs before outputs.
	for (int direction = 0; direction < 2; ++direction)
		for (size_t i = 0; i < definition->portCount; ++i)
		{
			const SSMPlugins::PortDefinition &port = definition->defaultPorts[i];
			if (port.input != (direction == 0)) continue;
			m_PluginInfo.PortTips.push_back(port.name);
			if (port.input) ++m_PluginInfo.NumInputs;
			else ++m_PluginInfo.NumOutputs;
		}
}
