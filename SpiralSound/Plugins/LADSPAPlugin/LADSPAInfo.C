/*
    LADSPAInfo.C - Class for indexing information on LADSPA Plugins

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

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>

#include <cstdlib>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>

#include <ladspa.h>
#include "LADSPAInfo.h"

using namespace std;

LADSPAInfo::LADSPAInfo()
{
	RescanPlugins();
}

LADSPAInfo::~LADSPAInfo()
{
	CleanUp();
}

void
LADSPAInfo::RescanPlugins(void)
{
	char *ladspa_path;
	char *start;
	char *end;
	int extra;
	char *temp;

// Clear out what we've got
	CleanUp();

// Get $LADPSA_PATH, if available
	ladspa_path = getenv("LADSPA_PATH");
	if (!ladspa_path) {

	// Oops
		cerr << "WARNING: No LADSPA Path Set" << endl;
	}

// Extract path elements and add path
	if (ladspa_path) {
		start = ladspa_path;
		while (*start != '\0') {
			while (*start == ':') start++;
			end = start;
			while (*end != ':' && *end != '\0') end++;

			if (end - start > 0) {
				extra = (*(end - 1) == '/') ? 0 : 1;
				temp = (char *)malloc(end - start + 1 + extra);
				if (temp) {
					strncpy(temp, start, end - start);
					if (extra == 1) temp[end - start] = '/';
					temp[end - start + extra] = '\0';

					ExaminePath(temp);

					free(temp);
				}
			}
			start = end;
		}
	}

// Do we have any plugins now?
	if (m_Plugins.size() == 0) {
		cerr << "WARNING: No plugins found in given LADSPA_PATH" << endl;
	} else {
		cerr << m_Plugins.size() << " plugins found in " << m_Libraries.size() << " libraries" << endl;
	}

// No last loaded library or plugin
	m_LastLoadedLibraryIndex = m_Libraries.size();
	m_LastLoadedPluginIndex = m_Plugins.size();

// Sort list by name
	sort(m_OrderedPluginList.begin(), m_OrderedPluginList.end(), PluginEntrySortAsc());

// Deal with duplicates by numbering them
	for (vector<PluginEntry>::iterator i = m_OrderedPluginList.begin();
	     i != m_OrderedPluginList.end(); ) {
		string name = i->Name;

		i++;
		unsigned long n = 2;
		while ((i != m_OrderedPluginList.end()) && (i->Name == name)) {
			stringstream s;
			s << n;
			i->Name = name + " (" + s.str() + ")";
			n++;
			i++;
		}
	}
}

void
LADSPAInfo::UnloadAllLibraries(void)
{
	for (vector<LibraryInfo>::iterator i = m_Libraries.begin();
	     i != m_Libraries.end(); i++) {
		if (i->Handle) dlclose(i->Handle);
	}

// No last loaded library or plugin
	m_LastLoadedLibraryIndex = m_Libraries.size();
	m_LastLoadedPluginIndex = m_Plugins.size();
}

void
LADSPAInfo::UnloadLibraryByID(unsigned long unique_id)
{
	if (m_IDLookup.find(unique_id) == m_IDLookup.end()) {
		cerr << "LADSPA Plugin ID " << unique_id << " not found!" << endl;
	} else {

	// Get plugin index
		unsigned long plugin_index = m_IDLookup[unique_id];
		UnloadLibraryByPlugin(plugin_index);
	}
}

const LADSPA_Descriptor *
LADSPAInfo::GetDescriptorByID(unsigned long unique_id,
                              bool unload_previous_library)
{
	if (m_IDLookup.find(unique_id) == m_IDLookup.end()) {
		cerr << "LADSPA Plugin ID " << unique_id << " not found!" << endl;
		return NULL;
	}

// Got plugin index
	unsigned long plugin_index = m_IDLookup[unique_id];

	PluginInfo *pi = &(m_Plugins[plugin_index]);
	if (!(pi->Descriptor)) {
		LADSPA_Descriptor_Function desc_func = GetDescriptorFunctionForLibrary(pi->LibraryIndex);

		if (desc_func) {
			pi->Descriptor = desc_func(pi->Index);

		// Unload previously loaded library (if different)
			if ((m_LastLoadedLibraryIndex != m_Libraries.size()) &&
				(m_LastLoadedLibraryIndex != pi->LibraryIndex)) {
				UnloadLibraryByPlugin(plugin_index);
			}
		}
	}
	return pi->Descriptor;
}

unsigned long
LADSPAInfo::GetIDFromFilenameAndLabel(std::string filename,
                                      std::string label)
{
	bool library_loaded = false;

	if (m_FilenameLookup.find(filename) == m_FilenameLookup.end()) {
		cerr << "LADSPA Library " << filename << " not found!" << endl;
		return 0;
	}

	unsigned long library_index = m_FilenameLookup[filename];

	if (!(m_Libraries[library_index].Handle)) library_loaded = true;

	LADSPA_Descriptor_Function desc_func = GetDescriptorFunctionForLibrary(library_index);

	if (!desc_func) {
		return 0;
	}

// Search for label in library
	const LADSPA_Descriptor *desc;
	for (unsigned long i = 0; (desc = desc_func(i)) != NULL; i++) {
		string l = desc->Label;
		if (l == label) {
		// If we had to load the library, unload it
			unsigned long id = desc->UniqueID;
			if (library_loaded) {
				dlclose(m_Libraries[library_index].Handle);
				m_Libraries[library_index].Handle = NULL;
			}
			return id;
		}
	}

	cerr << "Plugin " << label << " not found in library " << filename << endl;
	return 0;
}

const vector<LADSPAInfo::PluginEntry>
LADSPAInfo::GetPluginList(void)
{
	return m_OrderedPluginList;
}

unsigned long
LADSPAInfo::GetPluginListEntryByID(unsigned long unique_id)
{
	unsigned long j = 0;
	for (vector<PluginEntry>::iterator i = m_OrderedPluginList.begin();
	     i != m_OrderedPluginList.end(); i++, j++) {
		if (i->UniqueID == unique_id) return j;
	}
	return m_OrderedPluginList.size();
}

// ****************************************************************************
// **                     Private Member Functions                           **
// ****************************************************************************

// Unload any loaded DLLs and clear vectors etc
void
LADSPAInfo::CleanUp(void)
{
	m_IDLookup.clear();
	m_FilenameLookup.clear();
	m_Plugins.clear();

// Unload loaded dlls
	for (vector<LibraryInfo>::iterator i = m_Libraries.begin();
	     i != m_Libraries.end(); i++) {
		if (i->Handle) dlclose(i->Handle);
	}

	m_Libraries.clear();
	m_Paths.clear();

	m_OrderedPluginList.clear();
	m_MaxInputPortCount = 0;
}

// Check given path:
//   1. Exists
//   2. Is a directory
//   3. Contains at least one LADSPA Plugin library
//
// If so, add the path to the Paths vector, and examine
// contents, adding all valid LADSPA plugin library info
// to the Libraries and Plugins vectors.
void
LADSPAInfo::ExaminePath(const char *path)
{
	DIR *dp;
	struct dirent *ep;
	void *handle;
	LADSPA_Descriptor_Function desc_func;
	const LADSPA_Descriptor *desc;
	bool path_added = false;
	bool library_added;
	string fullpath;
	vector<string> temp_names;

	dp = opendir(path);
	if (!dp) {
		cerr << "WARNING: Could not open path " << path << endl;
	} else {
		while ((ep = readdir(dp))) {

		// Ignore ./ and ../
			if (!((strlen(ep->d_name) == 2 && ep->d_name[0] == '.' && ep->d_name[1] == '.') ||
			      (strlen(ep->d_name) == 1 && ep->d_name[0] == '.'))) {

			// Need full path
				fullpath = path;
				fullpath.append(ep->d_name);

			// We're not fussed about resolving symbols yet, since we are just
			// checking libraries.
				handle = dlopen(fullpath.c_str(), RTLD_LAZY);

				if (!handle) {
					cerr << "WARNING: File " << path << ep->d_name
					     << " could not be examined" << endl;
					cerr << "dleror() output:" << endl;
					cerr << dlerror() << endl;
				} else {

				// It's a DLL, so now see if it's a LADSPA plugin library
					desc_func = (LADSPA_Descriptor_Function)dlsym(handle,
					                                              "ladspa_descriptor");
					if (!desc_func) {

					// Is DLL, but not a LADSPA one
						cerr << "WARNING: DLL " << path << ep->d_name
						     << " has no ladspa_descriptor function" << endl;
						cerr << "dleror() output:" << endl;
						cerr << dlerror() << endl;
					} else {

					// Got ladspa_descriptor, so we can now get plugin info
						library_added = false;
						unsigned long i = 0;
						desc = desc_func(i);
						while (desc) {

							// First, check that it's not a dupe
							if (m_IDLookup.find(desc->UniqueID) != m_IDLookup.end()) {
								unsigned long plugin_index;
								unsigned long library_index;
								unsigned long path_index;

								cerr << "WARNING: Duplicated Plugin ID ("
								     << desc->UniqueID << ") found:" << endl;

								plugin_index = m_IDLookup[desc->UniqueID];
								library_index = m_Plugins[plugin_index].LibraryIndex;
								path_index = m_Libraries[library_index].PathIndex;

								cerr << "  Plugin " << m_Plugins[plugin_index].Index
								     << " in library: " << m_Paths[path_index]
								     << m_Libraries[library_index].Basename
									 << " [First instance found]" << endl;
								cerr << "  Plugin " << i << " in library: " << path << ep->d_name
								     << " [Duplicate not added]" << endl;
							} else {
								if (CheckPlugin(desc_func)) {
									if (!path_added) {

									// Add path
										m_Paths.push_back(path);
										path_added = true;
									}
									if (!library_added) {

									// Add library info
										LibraryInfo li;
										li.PathIndex = m_Paths.size() - 1;
										li.Basename = ep->d_name;
										li.Handle = NULL;
										m_Libraries.push_back(li);

									// Add filename to lookup
										m_FilenameLookup[fullpath] = m_Libraries.size() - 1;

										library_added = true;
									}

									// Add plugin info
									PluginInfo pi;
									pi.LibraryIndex = m_Libraries.size() - 1;
									pi.Index = i;
									pi.Descriptor = NULL;
									m_Plugins.push_back(pi);

									// Add to index
									m_IDLookup[desc->UniqueID] = m_Plugins.size() - 1;

									// Add to ordered list
									PluginEntry pe;
									pe.UniqueID = desc->UniqueID;
									pe.Name = desc->Name;
									m_OrderedPluginList.push_back(pe);

									// Find number of input ports
									unsigned long in_port_count = 0;
									for (unsigned long p = 0; p < desc->PortCount; p++) {
										if (LADSPA_IS_PORT_INPUT(desc->PortDescriptors[p])) {
											in_port_count++;
										}
									}
									if (in_port_count > m_MaxInputPortCount) {
										m_MaxInputPortCount = in_port_count;
									}
								} else {
									cerr << "WARNING: Plugin " << desc->UniqueID << " not added" << endl;
								}
							}

							desc = desc_func(++i);
						}
					}
					dlclose(handle);
				}
			}
		}
		closedir(dp);
	}
}

bool
LADSPAInfo::CheckPlugin(const LADSPA_Descriptor_Function desc_func)
{
	const LADSPA_Descriptor *desc;

#define test(t, m) { \
	if (!(t)) { \
		cerr << m << endl; \
		return false; \
	} \
}
	for (unsigned long i = 0; (desc = desc_func(i)) != NULL; i++) {
		test(!LADSPA_IS_REALTIME(desc->Properties), "WARNING: Plugin must run real time");
		test(desc->instantiate, "WARNING: Plugin has no instatiate function");
		test(desc->connect_port, "WARNING: Warning: Plugin has no connect_port funciton");
		test(desc->run, "WARNING: Plugin has no run function");
		test(!(desc->run_adding != 0 && desc->set_run_adding_gain == 0),
			    "WARNING: Plugin has run_adding but no set_run_adding_gain");
		test(!(desc->run_adding == 0 && desc->set_run_adding_gain != 0),
			    "WARNING: Plugin has set_run_adding_gain but no run_adding");
		test(desc->cleanup, "WARNING: Plugin has no cleanup function");
		test(!LADSPA_IS_INPLACE_BROKEN(desc->Properties),
			    "WARNING: Plugin cannot use in place processing");
		test(desc->PortCount, "WARNING: Plugin has no ports");
	}
	return true;
}

LADSPA_Descriptor_Function
LADSPAInfo::GetDescriptorFunctionForLibrary(unsigned long library_index)
{
	LibraryInfo *li = &(m_Libraries[library_index]);
	if (!(li->Handle)) {

	// Need full path
		string fullpath = m_Paths[li->PathIndex];
		fullpath.append(li->Basename);

		li->Handle = dlopen(fullpath.c_str(), RTLD_NOW);
		if (!(li->Handle)) {

		// Plugin library changed since last path scan
			cerr << "WARNING: Plugin library " << fullpath << " cannot be loaded" << endl;
			cerr << "Rescan of plugins recommended" << endl;
			cerr << "dleror() output:" << endl;
			cerr << dlerror() << endl;
			return NULL;
		}
	}

// Got handle so now verify that it's a LADSPA plugin library
	const LADSPA_Descriptor_Function desc_func = (LADSPA_Descriptor_Function)dlsym(li->Handle,
	                                                                               "ladspa_descriptor");
	if (!desc_func) {

	// Is DLL, but not a LADSPA one (changed since last path scan?)
		cerr << "WARNING: DLL " << m_Paths[li->PathIndex] << li->Basename
			<< " has no ladspa_descriptor function" << endl;
		cerr << "Rescan of plugins recommended" << endl;
		cerr << "dleror() output:" << endl;
		cerr << dlerror() << endl;

	// Unload library
		dlclose(li->Handle);
		return NULL;
	}

	return desc_func;
}

void
LADSPAInfo::UnloadLibraryByPlugin(unsigned long plugin_index)
{
// Unload library corresponding to given plugin index
	PluginInfo *pi = &(m_Plugins[plugin_index]);
	LibraryInfo *li = &(m_Libraries[pi->LibraryIndex]);
	if (li->Handle) dlclose(li->Handle);

// Need to unset all plugin descriptors that may have been
// set from this library
// Plugins in library will be a contiguous block, so we
// just check each direction from given plugin
	unsigned long i = plugin_index - 1;
	while (m_Plugins[i].LibraryIndex == pi->LibraryIndex) {
		m_Plugins[i--].Descriptor = NULL;
	}
	i = plugin_index + 1;
	while (m_Plugins[i].LibraryIndex == pi->LibraryIndex) {
		m_Plugins[i++].Descriptor = NULL;
	}
}
