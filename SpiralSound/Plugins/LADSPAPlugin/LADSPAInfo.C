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
#include <sys/stat.h>
#include <dirent.h>
#include <dlfcn.h>

#include <ladspa.h>
#include <lrdf.h>
#include "LADSPAInfo.h"

using namespace std;

LADSPAInfo::LADSPAInfo(bool override,
                       const char *path_list)
{
    if (strlen(path_list) > 0) {
        m_ExtraPaths = strdup(path_list);
    } else {
        m_ExtraPaths = NULL;
    }
    m_LADSPAPathOverride = override;

    lrdf_init();
    
    RescanPlugins();
}

LADSPAInfo::~LADSPAInfo()
{
    CleanUp();
}

void
LADSPAInfo::PrintInfo(void)
{
    cout << "LADSPA Plugin Info" << endl;
    cout << endl;
    cout << "    Paths:" << endl;
    for (vector<string>::iterator p = m_Paths.begin(); p != m_Paths.end(); p++) {
        cout << "        " << *p << endl;
    }
    cout << endl;
    cout << "    Libraries:" << endl;
    for (vector<LibraryInfo>::iterator l = m_Libraries.begin(); l != m_Libraries.end(); l++) {
        cout << "        " << m_Paths[l->PathIndex] << l->Basename << ": " << "Refs: " << l->RefCount << endl;
    }
    cout << endl;
    cout << "    Plugins:" << endl;
    for (vector<PluginInfo>::iterator p = m_Plugins.begin(); p != m_Plugins.end(); p++) {
        cout << "        " << m_Paths[m_Libraries[p->LibraryIndex].PathIndex]
             << m_Libraries[p->LibraryIndex].Basename
             << ": " << p->Index << endl;
    }
}

void
LADSPAInfo::RescanPlugins(void)
{
// Clear out what we've got
    CleanUp();

    if (!m_LADSPAPathOverride) {
    // Get $LADPSA_PATH, if available
        char *ladspa_path = getenv("LADSPA_PATH");
        if (!ladspa_path) {

        // Oops
            cerr << "WARNING: LADSPA_PATH environment variable not set" << endl;
        }

    // Extract path elements and add path
        if (ladspa_path) {
            ScanPathList(ladspa_path, &LADSPAInfo::ExaminePluginLibrary);
        }
    }

// Check any supplied extra paths
    if (m_ExtraPaths) {
        ScanPathList(m_ExtraPaths, &LADSPAInfo::ExaminePluginLibrary);
    }

// Do we have any plugins now?
    if (m_Plugins.size() == 0) {
        cerr << "WARNING: No plugins found" << endl;
    } else {
        cerr << m_Plugins.size() << " plugins found in " << m_Libraries.size() << " libraries" << endl;

    // Got some plugins. Now search for RDF data
        char *rdf_path = getenv("LADSPA_RDF_PATH");

        if (!rdf_path) {
            cerr << "WARNING: LADSPA_RDF_PATH environment variable set" << endl;
        } else {

        // Examine rdf info
            ScanPathList(rdf_path, &LADSPAInfo::ExamineRDFFile);
        }
    }

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
        i->RefCount = 0;
    }
}

const LADSPA_Descriptor *
LADSPAInfo::GetDescriptorByID(unsigned long unique_id)
{
    if (m_IDLookup.find(unique_id) == m_IDLookup.end()) {
        cerr << "LADSPA Plugin ID " << unique_id << " not found!" << endl;
        return NULL;
    }

// Got plugin index
    unsigned long plugin_index = m_IDLookup[unique_id];

    PluginInfo *pi = &(m_Plugins[plugin_index]);
    LibraryInfo *li = &(m_Libraries[pi->LibraryIndex]);

    if (!(pi->Descriptor)) {
        LADSPA_Descriptor_Function desc_func = GetDescriptorFunctionForLibrary(pi->LibraryIndex);

        if (desc_func) pi->Descriptor = desc_func(pi->Index);
    }

    if (pi->Descriptor) {

    // Success, so increment ref counter for library
        li->RefCount++;
    }

    return pi->Descriptor;
}

void
LADSPAInfo::DiscardDescriptorByID(unsigned long unique_id)
{
    if (m_IDLookup.find(unique_id) == m_IDLookup.end()) {
        cerr << "LADSPA Plugin ID " << unique_id << " not found!" << endl;
    } else {

    // Get plugin index
        unsigned long plugin_index = m_IDLookup[unique_id];

        PluginInfo *pi = &(m_Plugins[plugin_index]);
        LibraryInfo *li = &(m_Libraries[pi->LibraryIndex]);

    // Decrement reference counter for library, and unload if last
        if (li->RefCount > 0) {
            li->RefCount--;
            if (li->RefCount == 0) {
                dlclose(li->Handle);

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
        }
    }
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

    lrdf_cleanup();

    m_OrderedPluginList.clear();
    m_MaxInputPortCount = 0;

    if (m_ExtraPaths) {
        free(m_ExtraPaths);
        m_ExtraPaths = NULL;
    }
}

// Given a colon-separated list of paths, examine the contents of each
// path, examining any regular files using the given member function,
// which currently can be:
//
//   ExaminePluginLibrary - add plugin library info from plugins
//   ExamineRDFFile       - add plugin information from .rdf/.rdfs files
void
LADSPAInfo::ScanPathList(const char *path_list,
                         void (LADSPAInfo::*ExamineFunc)(const string,
                                                         const string))
{
    const char *start;
    const char *end;
    int extra;
    char *path;
    string basename;
    DIR *dp;
    struct dirent *ep;
    struct stat sb;

// This does the same kind of thing as strtok, but strtok won't
// like the const
    start = path_list;
    while (*start != '\0') {
        while (*start == ':') start++;
        end = start;
        while (*end != ':' && *end != '\0') end++;

        if (end - start > 0) {
            extra = (*(end - 1) == '/') ? 0 : 1;
            path = (char *)malloc(end - start + 1 + extra);
            if (path) {
                strncpy(path, start, end - start);
                if (extra == 1) path[end - start] = '/';
                path[end - start + extra] = '\0';

                dp = opendir(path);
                if (!dp) {
                    cerr << "WARNING: Could not open path " << path << endl;
                } else {
                    while ((ep = readdir(dp))) {

                    // Stat file to get type
                        basename = ep->d_name;
                        if (!stat((path + basename).c_str(), &sb)) {

                        // We only want regular files
                            if (S_ISREG(sb.st_mode)) (*this.*ExamineFunc)(path, basename);
                        }
                    }
                    closedir(dp);
                }
                free(path);
            }
        }
        start = end;
    }
}

// Check given file is a valid LADSPA Plugin library
//
// If so, add path, library and plugin info
// to the m_Paths, m_Libraries and m_Plugins vectors.
void
LADSPAInfo::ExaminePluginLibrary(const string path,
                                 const string basename)
{
    void *handle;
    LADSPA_Descriptor_Function desc_func;
    const LADSPA_Descriptor *desc;
    string fullpath = path + basename;

// We're not fussed about resolving symbols yet, since we are just
// checking libraries.
    handle = dlopen(fullpath.c_str(), RTLD_LAZY);

    if (!handle) {
        cerr << "WARNING: File " << fullpath
            << " could not be examined" << endl;
        cerr << "dlerror() output:" << endl;
        cerr << dlerror() << endl;
    } else {

    // It's a DLL, so now see if it's a LADSPA plugin library
        desc_func = (LADSPA_Descriptor_Function)dlsym(handle,
                                                      "ladspa_descriptor");
        if (!desc_func) {

        // Is DLL, but not a LADSPA one
            cerr << "WARNING: DLL " << fullpath
                << " has no ladspa_descriptor function" << endl;
            cerr << "dlerror() output:" << endl;
            cerr << dlerror() << endl;
        } else {

        // Got ladspa_descriptor, so we can now get plugin info
            bool library_added = false;
            unsigned long i = 0;
            desc = desc_func(i);
            while (desc) {

            // First, check that it's not a dupe
                if (m_IDLookup.find(desc->UniqueID) != m_IDLookup.end()) {
                    unsigned long plugin_index = m_IDLookup[desc->UniqueID];
                    unsigned long library_index = m_Plugins[plugin_index].LibraryIndex;
                    unsigned long path_index = m_Libraries[library_index].PathIndex;

                    cerr << "WARNING: Duplicated Plugin ID ("
                        << desc->UniqueID << ") found:" << endl;

                    cerr << "  Plugin " << m_Plugins[plugin_index].Index
                         << " in library: " << m_Paths[path_index]
                         << m_Libraries[library_index].Basename
                         << " [First instance found]" << endl;
                    cerr << "  Plugin " << i << " in library: " << fullpath
                         << " [Duplicate not added]" << endl;
                } else {
                    if (CheckPlugin(desc)) {

                    // Add path if not already added
                        unsigned long path_index;
                        vector<string>::iterator p = find(m_Paths.begin(), m_Paths.end(), path);
                        if (p == m_Paths.end()) {
                            path_index = m_Paths.size();
                            m_Paths.push_back(path);
                        } else {
                            path_index = p - m_Paths.begin();
                        }

                    // Add library info if not already added
                        if (!library_added) {
                            LibraryInfo li;
                            li.PathIndex = path_index;
                            li.Basename = basename;
                            li.RefCount = 0;
                            li.Handle = NULL;
                            m_Libraries.push_back(li);

                            library_added = true;
                        }

                    // Add filename to lookup
                        m_FilenameLookup[fullpath] = m_Libraries.size() - 1;

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

// Examine given RDF plugin meta-data file
void
LADSPAInfo::ExamineRDFFile(const std::string path,
                           const std::string basename)
{
    string fileuri = "file://" + path + basename;

    if (lrdf_read_file(fileuri.c_str())) {
        cerr << "WARNING: File " << path + basename << " could not be parsed [Ignored]" << endl;
    }
}

bool
LADSPAInfo::CheckPlugin(const LADSPA_Descriptor *desc)
{
#define test(t, m) { \
    if (!(t)) { \
        cerr << m << endl; \
        return false; \
    } \
}
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
            cerr << "dlerror() output:" << endl;
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
        cerr << "dlerror() output:" << endl;
        cerr << dlerror() << endl;

    // Unload library
        dlclose(li->Handle);
        return NULL;
    }

    return desc_func;
}
