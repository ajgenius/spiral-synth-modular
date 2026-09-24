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

#ifndef SPIRAL_PLUGIN_MANAGER_H
#define SPIRAL_PLUGIN_MANAGER_H

#include <string>
#include <vector>
#include "SpiralPlugin.h"

class SpiralGUIType;

/* Paired DSP+GUI plugin (0.3.1 split). type is SPIRAL_PLUGIN_TYPE_DSP/GUI
   while only one side is loaded, SPIRAL_PLUGIN_TYPE_PAIRED once both .so
   files for the same ID are in. GUI<->DSP traffic stays on ChannelHandler
   (libspiralcore mutex trylock); this is not master's Device/Property
   split and not the lock-free queues from PR #11. */
struct HostsideInfo
{
	int   ID;
	int   type;
	std::string Name;
	std::string Category;
	const SSMPlugins::PluginClass *DSPClass;
	const SSMPlugins::PluginUIDefinition *GUIClass;

	struct {
		void *Handle;
		const char **(*GetIcon)(void);
	} dsp;

	struct {
		void *Handle;
		const char **(*GetIcon)(void);
	} gui;

	SpiralPlugin *CreateDSPInstance() const
	{
		return DSPClass ? dynamic_cast<SpiralPlugin *>(DSPClass->Create()) : NULL;
	}

	SpiralGUIType *CreateGUI(SpiralPlugin *plugin) const
	{
		return (GUIClass && GUIClass->createUI && plugin) ? GUIClass->createUI(plugin) : NULL;
	}

	const char **Icon() const
	{
		if (dsp.GetIcon) return dsp.GetIcon();
		if (gui.GetIcon) return gui.GetIcon();
		return NULL;
	}

	std::string GroupName() const
	{
		return Category;
	}

	bool HasDSP() const { return dsp.Handle != NULL && DSPClass != NULL; }
};

//////////////////////////////////////////////////////////

typedef int PluginID;
#define     PluginError -1

class PluginManager
{
public:
	static PluginManager *Get() { if(!m_Singleton) m_Singleton=new PluginManager; return m_Singleton; }
	static void         PackUpAndGoHome() { if(m_Singleton) delete m_Singleton; }

	PluginID            LoadPlugin(const char *PluginName);
	std::vector<int>     LoadPlugins(const std::string &root, const std::vector<std::string> &modules);
	const SSMPlugins::PluginRegistry &Registry() const { return m_Registry; }
	void                UnLoadPlugin(PluginID ID);
	void                UnloadAll();
	const HostsideInfo* GetPlugin(PluginID ID);
	bool                IsValid(PluginID ID);
	int                 GetIdByName(std::string Name);

private:

	PluginManager();
	~PluginManager();
	HostsideInfo *GetPlugin_i(PluginID ID);
	HostsideInfo *NewSlot(int ID);

	std::vector<HostsideInfo*> m_PluginVec;
	SSMPlugins::PluginRegistry m_Registry;
	struct Module
	{
		void *handle;
		SSMPlugins::PluginID id;
		Module(void *h, SSMPlugins::PluginID i): handle(h), id(i) {}
	};
	std::vector<Module> m_Modules;
	std::string m_LoadError;
	bool m_Waiting;
	PluginID TryLoad(const std::string &path, const struct PluginManifest *manifest);
	static PluginManager *m_Singleton;
};

#endif
