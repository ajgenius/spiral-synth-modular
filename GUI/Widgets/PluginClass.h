// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef SSM_PLUGIN_CLASS_H
#define SSM_PLUGIN_CLASS_H

#include <map>
#include <string>
#include <vector>

namespace SSMPlugins
{
	class Plugin;

	namespace PluginTypes { enum Type { Root, DSP, GUI, Audio, MIDI, Host }; }
	typedef PluginTypes::Type PluginType;
	const char *PluginTypeName(PluginType type);

	struct PluginID
	{
		PluginType type;
		int id;
		PluginID(PluginType type = PluginTypes::Root, int id = -1): type(type), id(id) {}
		bool operator==(const PluginID &other) const { return type == other.type && id == other.id; }
		bool operator!=(const PluginID &other) const { return !(*this == other); }
		bool operator<(const PluginID &other) const
		{
			return type != other.type ? type < other.type : id < other.id;
		}
	};

	struct PluginContext
	{
		virtual ~PluginContext() {}
	};

	// Common class metadata, independent of any plugin family.
	// The defining module must outlive the registry and instances.
	struct PluginDefinition
	{
		PluginType type;
		int id;
		const char *key;
		PluginID parent;
		const PluginID *dependencies;
		size_t dependencyCount;
		const char *name;
		const char *category;
		typedef Plugin *(*Factory)(const PluginContext *);
		Factory create;

		PluginDefinition(PluginType type, int id, const char *key, PluginID parent, const char *name,
			const char *category, Factory create = NULL, const PluginID *dependencies = NULL, size_t count = 0):
			type(type), id(id), key(key), parent(parent), dependencies(dependencies), dependencyCount(count),
			name(name), category(category), create(create) {}
		PluginID Identity() const { return PluginID(type, id); }

		virtual ~PluginDefinition() {}
		virtual bool Validate(std::string &) const { return true; }
		virtual Plugin *Create(const PluginContext *context) const
		{
			return create ? create(context) : NULL;
		}
	};

	class Plugin
	{
	public:
		virtual ~Plugin() {}
		static const PluginDefinition &StaticClass();
		virtual const PluginDefinition &ClassInfo() const { return StaticClass(); }
	};

	// Registered view of a definition, with links owned by this host's registry.
	class PluginClass
	{
	public:
		const PluginDefinition &Info() const { return definition; }
		const PluginClass *Parent() const { return parent; }
		const std::vector<const PluginClass *> &Subclasses() const { return subclasses; }
		Plugin *Create(const PluginContext *context = NULL) const;

	private:
		friend class PluginRegistry;
		PluginClass(const PluginDefinition &info, const PluginClass *base): definition(info), parent(base) {}
		const PluginDefinition &definition;
		const PluginClass *parent;
		std::vector<const PluginClass *> subclasses;
	};

	// Explicit startup registration; no dlopen-time constructor mutates this registry.
	class PluginRegistry
	{
	public:
		PluginRegistry();
		~PluginRegistry();
		void Clear();
		void Reset();
		int Register(const PluginDefinition &definition);
		// Startup rollback only: the caller must not have created instances.
		bool Discard(PluginID id);
		const PluginClass *Find(PluginID id) const;
		const PluginClass *Find(PluginType type, int id) const { return Find(PluginID(type, id)); }
		const PluginClass *Find(PluginType type, const std::string &key) const;
		const std::string &Error() const { return error; }
		bool WaitingForDependencies() const { return waiting; }

	private:
		PluginRegistry(const PluginRegistry &);
		PluginRegistry &operator=(const PluginRegistry &);
		typedef std::map<PluginID, PluginClass *> Classes;
		Classes classes;
		std::string error;
		bool waiting;
	};

	// Initializers only register definitions and must tolerate retries while dependencies are missing.
	// The returned identity includes type; id -1 means failure; root:0 is the root; other families may use ID zero; other negative IDs identify abstract families.
	typedef PluginID (*InitializePlugin)(PluginRegistry *);
}
#endif
