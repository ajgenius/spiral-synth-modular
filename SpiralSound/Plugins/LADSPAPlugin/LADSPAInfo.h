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
// If override is false, examine $LADSPA_PATH
// Also examine supplied path list
// For all paths, add basic plugin information for later lookup,
// instantiation and so on.
    LADSPAInfo(bool override, const char *path_list);

// Unload all loaded plugins and clean up
    ~LADSPAInfo();

// ************************************************************************
// Loading/Unloading plugin libraries
//
// At first, no library dlls are loaded.
//
// Each library has an associated reference count, which is initially 0.
// As descriptors are requested, using GetDescriptorByID, this count
// is incremented. The library dll is loaded on the first request.
// At descriptors are discarded, the count is decremented, and when this
// reaches 0, the library is unloaded.

// Rescan all paths in $LADSPA_PATH, as per constructor.
// This will also unload all libraries, and make any descriptors that
// have not been discarded with DiscardDescriptorByID invalid.
    void                            RescanPlugins(void);

// Unload all dlopened libraries. This will make any descriptors that
// have not been discarded with DiscardDescriptorByID invalid.
    void                            UnloadAllLibraries(void);

// Get descriptor of plugin with given ID. This increments the descriptor
// count for the corresponding library.
    const LADSPA_Descriptor        *GetDescriptorByID(unsigned long unique_id);

// Notify that a descriptor corresponding to the given ID has been
// discarded. This decrements the descriptor count for the corresponding
// library.
    void                            DiscardDescriptorByID(unsigned long unique_id);

// Get unique ID of plugin identified by given library filename and label.
    unsigned long                   GetIDFromFilenameAndLabel(std::string filename,
                                                              std::string label);

    struct PluginEntry
    {
        unsigned long               UniqueID;
        std::string                 Name;
    };

// Get a list of plugins ordered by name (duplicate names are
// appended with a (number)
    const std::vector<PluginEntry>  GetPluginList(void);

// Get the index in the above list for given Unique ID
// If not found, this returns the size of the above list
    unsigned long                   GetPluginListEntryByID(unsigned long unique_id);

// Get the number of input ports for the plugin with the most
// input ports
    unsigned long                   GetMaxInputPortCount(void) { return m_MaxInputPortCount; }

    void                            PrintInfo(void);

private:
    void                            CleanUp(void);
    void                            ScanPathList(const char *path_list,
                                                 void (LADSPAInfo::*ExamineFunc)(const std::string,
                                                                                 const std::string));
    void                            ExaminePluginLibrary(const std::string path,
                                                         const std::string basename);
#ifdef HAVE_LIBLRDF
    void                            ExamineRDFFile(const std::string path,
                                                   const std::string basename);
#endif

    bool                            CheckPlugin(const LADSPA_Descriptor *desc);
    LADSPA_Descriptor_Function      GetDescriptorFunctionForLibrary(unsigned long library_index);

    struct LibraryInfo
    {
        unsigned long           PathIndex;  // Index of path in m_Paths
        std::string             Basename;   // Filename
        unsigned long           RefCount;   // Count of descriptors requested from library
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

    bool                            m_LADSPAPathOverride;
    char                           *m_ExtraPaths;

    std::vector<std::string>        m_Paths;
    std::vector<LibraryInfo>        m_Libraries;
    std::vector<PluginInfo>         m_Plugins;

    IDMap                           m_IDLookup;
    StringMap                       m_FilenameLookup;

    std::vector<PluginEntry>        m_OrderedPluginList;

    unsigned long                   m_MaxInputPortCount;
};

#endif // __ladspa_info_h__
