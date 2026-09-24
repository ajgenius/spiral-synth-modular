// SPDX-License-Identifier: GPL-2.0-or-later
#include "PluginClass.h"
#include <set>
#include <sstream>
#include <algorithm>

namespace SSMPlugins
{
	const char *PluginTypeName(PluginType type)
	{
		switch (type)
		{
			case PluginTypes::Root: return "root";
			case PluginTypes::DSP: return "dsp";
			case PluginTypes::GUI: return "gui";
			case PluginTypes::Audio: return "audio";
			case PluginTypes::MIDI: return "midi";
			case PluginTypes::Host: return "host";
		}
		return NULL;
	}

	static std::string IdentityText(PluginID id)
	{
		std::ostringstream text;
		text << PluginTypeName(id.type) << ":" << id.id;
		return text.str();
	}

	const PluginDefinition &Plugin::StaticClass()
	{
		static const PluginDefinition root(PluginTypes::Root, 0, "SpiralPlugin", PluginID(), "Plugin", "");
		return root;
	}

	Plugin *PluginClass::Create(const PluginContext *context) const
	{
		return definition.Create(context);
	}

	PluginRegistry::PluginRegistry(): waiting(false)
	{
		classes[Plugin::StaticClass().Identity()] = new PluginClass(Plugin::StaticClass(), NULL);
	}

	PluginRegistry::~PluginRegistry()
	{
		Clear();
	}

	void PluginRegistry::Reset()
	{
		Clear();
		classes[Plugin::StaticClass().Identity()] = new PluginClass(Plugin::StaticClass(), NULL);
	}

	void PluginRegistry::Clear()
	{
		for (Classes::iterator i = classes.begin(); i != classes.end(); ++i)
			delete i->second;
		classes.clear();
	}

	const PluginClass *PluginRegistry::Find(PluginID id) const
	{
		Classes::const_iterator found = classes.find(id);
		return found == classes.end() ? NULL : found->second;
	}

	const PluginClass *PluginRegistry::Find(PluginType type, const std::string &key) const
	{
		for (Classes::const_iterator entry = classes.begin(); entry != classes.end(); ++entry)
			if (type == entry->first.type && key == entry->second->Info().key)
				return entry->second;
		return NULL;
	}

	bool PluginRegistry::Discard(PluginID id)
	{
		Classes::iterator found = classes.find(id);
		if (found == classes.end() || !found->second->parent || !found->second->subclasses.empty())
			return false;
		for (Classes::const_iterator entry = classes.begin(); entry != classes.end(); ++entry)
		{
			const PluginDefinition &definition = entry->second->Info();
			for (size_t i = 0; i < definition.dependencyCount; ++i)
				if (definition.dependencies[i] == id)
					return false;
		}
		std::vector<const PluginClass *> &siblings = classes[found->second->parent->Info().Identity()]->subclasses;
		siblings.erase(std::remove(siblings.begin(), siblings.end(), found->second), siblings.end());
		delete found->second;
		classes.erase(found);
		return true;
	}

	int PluginRegistry::Register(const PluginDefinition &definition)
	{
		error.clear();
		waiting = false;
		if (!PluginTypeName(definition.type) || ((definition.type == PluginTypes::Root && definition.id == 0) || definition.id == -1) ||
			!definition.key || !*definition.key ||
			!definition.name || !*definition.name || !definition.category ||
			!PluginTypeName(definition.parent.type) || definition.parent.id == -1 ||
			(definition.dependencyCount && !definition.dependencies))
		{
			error = "Invalid plugin class definition";
			return -1;
		}

		const PluginClass *existing = Find(definition.Identity());
		if (existing && &existing->Info() == &definition)
			return definition.id;
		if (existing || Find(definition.type, definition.key))
		{
			error = "Duplicate plugin ID or class key: " + std::string(definition.key);
			return -1;
		}

		std::set<PluginID> dependencies;
		for (size_t i = 0; i < definition.dependencyCount; ++i)
		{
			PluginID dependency = definition.dependencies[i];
			if (!PluginTypeName(dependency.type) || dependency.id == -1 || dependency == definition.Identity() ||
				!dependencies.insert(dependency).second)
			{
				error = "Invalid, duplicate or self dependency";
				return -1;
			}
		}
		for (std::set<PluginID>::const_iterator i = dependencies.begin(); i != dependencies.end(); ++i)
		{
			if (!Find(*i))
			{
				waiting = true;
				error = "Missing dependency: " + IdentityText(*i);
				return -1;
			}
		}

		const PluginClass *parent = Find(definition.parent);
		if (!parent)
		{
			waiting = true;
			error = "Missing parent class: " + IdentityText(definition.parent);
			return -1;
		}

		if (!definition.Validate(error))
			return -1;

		PluginClass *type = new PluginClass(definition, parent);
		// Parents must already exist, so registration cannot introduce an ancestry cycle.
		classes[parent->Info().Identity()]->subclasses.push_back(type);
		classes[definition.Identity()] = type;
		return definition.id;
	}
}
