/*
    LADSPAInfo.h - Header file for LADSPA Plugin info class

    Copyright (C) 2002  Mike Rawes

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __ladspa_info_h__
#define __ladspa_info_h__

#include <string>
#include <vector>
#include <map>
#include <ladspa.h>

class LADSPAInfo
{
public:
	// Examine all valid paths in $LADSPA_PATH, and add
	// basic plugin information for later lookup, instantiation
	// and so on.
	LADSPAInfo();

	// Unload all loaded plugins and clean up
	~LADSPAInfo();

	// Rescan all paths in $LADSPA_PATH, as per constructor
	// This will also unload all libraries
	void                            RescanPlugins(void);

	// Unload all dlopened libraries
	void                            UnloadAllLibraries(void);

	// Unload library containing plugin id
	void                            UnloadLibraryByID(unsigned long unique_id);

	// Get descriptor of plugin in list, loading library if necessary
	// and optionally unloading previously loaded library (if different
	// from library containing requested plugin. Phew!)
	const LADSPA_Descriptor        *GetDescriptorByID(unsigned long unique_id,
	                                                  bool unload_previous_library);

	unsigned long                   GetIDFromFilenameAndLabel(std::string filename,
	                                                          std::string label);

	struct PluginEntry
	{
		unsigned long               UniqueID;
		std::string                 Name;
	};

	// Get a list of plugins ordered by name
	const std::vector<PluginEntry>  GetPluginList(void);

	// Get the index in the above list for given Unique ID
	// If not found, this returns the size of the above list
	unsigned long                   GetPluginListEntryByID(unsigned long unique_id);
	
	// Get the number of input ports for the plugin with the most
	// input ports
	unsigned long                   GetMaxInputPortCount(void) { return m_MaxInputPortCount; }

private:
	void                            CleanUp(void);
	void                            ExaminePath(const char *path);
	bool                            CheckPlugin(LADSPA_Descriptor_Function desc_func);
	LADSPA_Descriptor_Function      GetDescriptorFunctionForLibrary(unsigned long library_index);
	void                            UnloadLibraryByPlugin(unsigned long plugin_index);

	struct LibraryInfo
	{
		unsigned long           PathIndex;  // Index of path in m_Paths
		std::string             Basename;   // Filename
		void                   *Handle;     // DLL Handle, NULL
	};

	struct PluginInfo
	{
		unsigned long               LibraryIndex;   // Index of library in m_Libraries
		unsigned long               Index;          // Plugin index in library
		const LADSPA_Descriptor    *Descriptor;     // Descriptor, NULL
	};

	typedef std::map<unsigned long,
	                 unsigned long,
	                 std::less<unsigned long> >  IDMap;

	typedef std::map<std::string,
	                 unsigned long,
	                 std::less<std::string> >    StringMap;

	// For sorting vectors of PluginEntries
	struct PluginEntrySortAsc
	{
		bool operator()(const PluginEntry &begin, const PluginEntry &end)
		{
			return begin.Name < end.Name;
		}
	};

	std::vector<std::string>        m_Paths;
	std::vector<LibraryInfo>        m_Libraries;
	std::vector<PluginInfo>         m_Plugins;

	unsigned long                   m_LastLoadedLibraryIndex;
	unsigned long                   m_LastLoadedPluginIndex;

	IDMap                           m_IDLookup;
	StringMap                       m_FilenameLookup;

	std::vector<PluginEntry>        m_OrderedPluginList;

	unsigned long                   m_MaxInputPortCount;
};

#endif // __ladspa_info_h__
