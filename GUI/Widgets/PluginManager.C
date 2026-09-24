/*  SpiralSound
 *  Copyleft (C) 2002 David Griffiths <dave@pawfal.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <config.h>
#include <cstring>
#include <dlfcn.h>
#include <stdio.h>
#include <set>
#include <algorithm>
#include <sys/stat.h>
#include "../../JSON/PluginManifest.h"
#include "SpiralInfo.h"
#include "PluginManager.h"
#include "SpiralGUI.H"

using namespace std;

PluginManager *PluginManager::m_Singleton = NULL;

PluginManager::PluginManager()
{
}

PluginManager::~PluginManager()
{
	UnloadAll();
}

static void ClearDSP(HostsideInfo *p)
{
	p->DSPClass = NULL;
	p->dsp.Handle = NULL;
	p->dsp.GetIcon = NULL;
}

static void ClearGUI(HostsideInfo *p)
{
	p->GUIClass = NULL;
	p->gui.Handle = NULL;
	p->gui.GetIcon = NULL;
}

static void UpdatePairedType(HostsideInfo *p)
{
	if (p->dsp.Handle && p->gui.Handle)
		p->type = SPIRAL_PLUGIN_TYPE_PAIRED;
	else if (p->dsp.Handle)
		p->type = SPIRAL_PLUGIN_TYPE_DSP;
	else if (p->gui.Handle)
		p->type = SPIRAL_PLUGIN_TYPE_GUI;
	else
		p->type = -1;
}

HostsideInfo *PluginManager::NewSlot(int ID)
{
	HostsideInfo *p = new HostsideInfo;
	p->ID = ID;
	p->type = -1;
	ClearDSP(p);
	ClearGUI(p);
	m_PluginVec.push_back(p);
	return p;
}

PluginID PluginManager::TryLoad(const string &path, const PluginManifest *manifest)
{
	m_LoadError.clear();
	m_Waiting = false;
	// Public GUI modules import native DSP methods. Preserve their global
	// symbol visibility; typed registration still precedes instance creation.
	void *handle = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
	if (!handle)
	{
		m_LoadError = dlerror();
		m_Waiting = true; // A bare native dependency may be discovered later.
		return PluginError;
	}
	typedef const char *(*TextFn)();
	TextFn abi = (TextFn)dlsym(handle, "SpiralPlugin_GetHostABI");
	const char *hostABI = abi ? abi() : NULL;
	SSMPlugins::InitializePlugin initialize =
		(SSMPlugins::InitializePlugin)dlsym(handle, "SpiralPlugin_Initialize");
	if (!hostABI || strcmp(hostABI, SSM_HOST_ABI) || !initialize)
	{
		m_LoadError = "Missing or incompatible plugin ABI/initializer";
		dlclose(handle);
		return PluginError;
	}
	SSMPlugins::PluginID id = initialize(&m_Registry);
	const SSMPlugins::PluginClass *type = m_Registry.Find(id);
	if (id.id < 0 || !type)
	{
		m_LoadError = m_Registry.Error();
		if (m_LoadError.empty()) m_LoadError = "Initializer did not register a concrete class";
		m_Waiting = m_Registry.WaitingForDependencies();
		dlclose(handle);
		return PluginError;
	}
	const SSMPlugins::PluginDefinition &info = type->Info();
	const SSMPlugins::DeviceDefinition *dsp = dynamic_cast<const SSMPlugins::DeviceDefinition *>(&info);
	const SSMPlugins::PluginUIDefinition *gui = dynamic_cast<const SSMPlugins::PluginUIDefinition *>(&info);
	bool valid = (id.type == SSMPlugins::PluginTypes::DSP && dsp && dsp->create) ||
		(id.type == SSMPlugins::PluginTypes::GUI && gui && gui->createUI);
	if (manifest)
	{
		valid = valid && manifest->id == id.id && manifest->type == SSMPlugins::PluginTypeName(id.type) &&
			manifest->name == info.name && manifest->category == info.category &&
			manifest->dependencies.size() == info.dependencyCount;
		for (size_t d = 0; valid && d < info.dependencyCount; ++d)
			valid = find(manifest->dependencies.begin(), manifest->dependencies.end(), info.dependencies[d]) != manifest->dependencies.end();
	}
	bool owned = false;
	for (size_t i = 0; i < m_Modules.size(); ++i)
		if (m_Modules[i].handle && m_Modules[i].id == id) owned = true;
	if (!valid)
	{
		m_LoadError = "Class definition does not match plugin family or manifest";
		if (!owned) m_Registry.Discard(id);
		dlclose(handle);
		return PluginError;
	}
	HostsideInfo *slot = GetPlugin_i(id.id);
	if (!slot) slot = NewSlot(id.id);
	if ((dsp && slot->dsp.Handle) || (gui && slot->gui.Handle))
	{
		dlclose(handle);
		return id.id;
	}
	const char **(*icon)() = (const char **(*)())dlsym(handle, "SpiralPlugin_GetIcon");
	if (dsp)
	{
		slot->dsp.Handle = handle;
		slot->dsp.GetIcon = icon;
		slot->DSPClass = type;
		slot->Name = info.name;
		slot->Category = info.category;
	}
	else
	{
		slot->gui.Handle = handle;
		slot->gui.GetIcon = icon;
		slot->GUIClass = gui;
	}
	m_Modules.push_back(Module(handle, id));
	UpdatePairedType(slot);
	return id.id;
}

PluginID PluginManager::LoadPlugin(const char *path)
{
	int id = TryLoad(path, NULL);
	if (id == PluginError) SpiralInfo::Alert(string(path) + ": " + m_LoadError);
	return id;
}

std::vector<int> PluginManager::LoadPlugins(const string &root, const vector<string> &modules)
{
	vector<int> result;
	vector<PluginManifest> manifests(modules.size());
	vector<bool> described(modules.size(), false), done(modules.size(), false);
	vector<string> errors(modules.size());
	string prefix = root;
	if (!prefix.empty() && prefix[prefix.size()-1] != '/') prefix += '/';
	for (size_t i = 0; i < modules.size(); ++i)
	{
		string path = prefix + modules[i];
		string::size_type slash = path.find_last_of('/');
		string directory = slash == string::npos ? "" : path.substr(0, slash + 1);
		string filename = slash == string::npos ? path : path.substr(slash + 1);
		string manifestPath = directory + "info.json";
		struct stat status;
		if (stat(manifestPath.c_str(), &status)) continue;
		described[i] = true;
#ifdef HAVE_YAJL
		if (!manifests[i].Read(manifestPath, errors[i]) ||
			!manifests[i].Compatible(PACKAGE_VERSION, SSM_HOST_ABI, "FLTK", "1", errors[i]) ||
			manifests[i].registration != "module" || manifests[i].module != filename)
#else
		errors[i] = "Manifest found, but this build has no YAJL support";
#endif
		{
			done[i] = true;
			if (errors[i].empty()) errors[i] = "Manifest module does not match binary";
			SpiralInfo::Alert(path + ": " + errors[i]);
		}
	}
	// Complete the manifest phase before attempting unmanifested binaries.
	for (int phase = 0; phase < 2; ++phase)
	{
		bool progress;
		do
		{
			progress = false;
			for (size_t i = 0; i < modules.size(); ++i)
			{
				if (done[i] || described[i] != (phase == 0)) continue;
				bool ready = true;
				for (size_t d = 0; described[i] && d < manifests[i].dependencies.size(); ++d)
					if (!m_Registry.Find(manifests[i].dependencies[d])) ready = false;
				if (!ready)
				{
					errors[i] = "Missing or cyclic manifest dependency";
					continue;
				}
				int id = TryLoad(prefix + modules[i], described[i] ? &manifests[i] : NULL);
				if (id != PluginError)
				{
					done[i] = true;
					progress = true;
					if (find(result.begin(), result.end(), id) == result.end()) result.push_back(id);
				}
				else
				{
					errors[i] = m_LoadError;
					if (!m_Waiting)
					{
						done[i] = true;
						SpiralInfo::Alert(prefix + modules[i] + ": " + errors[i]);
					}
				}
			}
		} while (progress);
		for (size_t i = 0; i < modules.size(); ++i)
			if (!done[i] && described[i] == (phase == 0))
			{
				done[i] = true;
				SpiralInfo::Alert(prefix + modules[i] + ": " + errors[i]);
			}
	}
	return result;
}

void PluginManager::UnLoadPlugin(PluginID ID)
{
	// Callers must have destroyed all instances. Refuse dependency consumers.
	HostsideInfo *slot = GetPlugin_i(ID);
	if (!slot) return;
	for (vector<Module>::reverse_iterator i = m_Modules.rbegin(); i != m_Modules.rend(); ++i)
	{
		if (i->id.id != ID || !i->handle) continue;
		if (!m_Registry.Discard(i->id))
		{
			SpiralInfo::Alert("Plugin still has registered dependents");
			return;
		}
		if (i->id.type == SSMPlugins::PluginTypes::GUI) ClearGUI(slot);
		else ClearDSP(slot);
		UpdatePairedType(slot);
		dlclose(i->handle);
		i->handle = NULL;
	}
	UpdatePairedType(slot);
}

void PluginManager::UnloadAll()
{
	// Registry views must disappear while their definitions are still mapped.
	m_Registry.Clear();
	for (vector<Module>::reverse_iterator i = m_Modules.rbegin(); i != m_Modules.rend(); ++i)
		if (i->handle) dlclose(i->handle);
	m_Modules.clear();
	m_Registry.Reset();
	for (size_t i = 0; i < m_PluginVec.size(); ++i) delete m_PluginVec[i];
	m_PluginVec.clear();
}

const HostsideInfo *PluginManager::GetPlugin(PluginID ID)
{
	HostsideInfo *ret = GetPlugin_i(ID);
	if (!ret)
	{
		char t[256];
		sprintf(t,"%d",ID);
		SpiralInfo::Alert("Plugin "+string(t)+" not found.");
	}
	return ret;
}

HostsideInfo *PluginManager::GetPlugin_i(PluginID ID)
{
	for (vector<HostsideInfo*>::iterator i=m_PluginVec.begin();
	     i!=m_PluginVec.end(); i++)
	{
		if ((*i)->ID==ID)
			return *i;
	}
	return NULL;
}

bool PluginManager::IsValid(PluginID ID)
{
	const HostsideInfo *t = GetPlugin(ID);
	return (t && t->HasDSP());
}

int PluginManager::GetIdByName(string Name)
{
	/* Declared on UA PluginManager.h but never defined. Looking up by
	   PluginInfo.Name would mean constructing a DSP instance at scan
	   time; the FLTK host never calls this. */
	(void)Name;
	return PluginError;
}

