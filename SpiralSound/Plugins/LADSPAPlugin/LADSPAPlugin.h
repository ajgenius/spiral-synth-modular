/*  SpiralSound
 *  Copyleft (C) 2001 David Griffiths <dave@pawfal.org>
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

#include "../SpiralPlugin.h"
#include <FL/Fl_Pixmap.H>
#include "ladspa.h"
#include "utils.h"

#ifndef LADSPAPLUGIN
#define LADSPAPLUGIN

static const unsigned int NUM_PORTS = 8;

class LPluginInfo {
public:
	string Filename;
	string Label;
	string Name;
	string Maker;
	unsigned long InputPorts;

	struct LPortDetails
	{
		string Name;
		float Min,Max;
		bool Clamped;
	};

	vector<LPortDetails> Ports;

	bool operator<(const LPluginInfo & li) { return (Name < li.Name); }
	bool operator==(const LPluginInfo& li) { return (Name == li.Name); }
};

// For sorting vectors of LPluginInfo's
struct LPluginInfoSortAsc
{
	bool operator()(const LPluginInfo & begin, const LPluginInfo & end)
	{
		return begin.Name < end.Name;
	}
};

struct PortRange
{
	float   Min;
	float   Max;
	bool    Clamp;
};

class LADSPAPlugin : public SpiralPlugin
{
public:
 	LADSPAPlugin();
	virtual ~LADSPAPlugin();

	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void ExecuteCommands();
	virtual void StreamOut(ostream &s);
	virtual void StreamIn(istream &s);

	float GetGain() { return m_Gain; }
	bool GetAmped() { return m_Amped; }

	enum GUICommands{NONE,UPDATERANGES,UPDATEPLUGIN};

private:

	void UpdatePortRange(void);
	bool UpdatePlugin(int n);
	bool UpdatePlugin(const char * filename, const char * label, bool PortClampReset=true);
	void UpdatePortRanges(void);

	friend void describePluginLibrary(const char * pcFullFilename, void * pvPluginHandle, LADSPA_Descriptor_Function pfDescriptorFunction);
	void LoadPluginList(void);

	void * PlugHandle;
	const LADSPA_Descriptor * PlugDesc;

	vector<LADSPA_Data*> m_LADSPABufVec;
	LADSPA_Handle PlugInstHandle;

	vector<int>   m_PortID;
	vector<float> m_PortMin;
	vector<float> m_PortMax;
	vector<bool>  m_PortClamp;

	// our database of ladspa plugins
	vector<LPluginInfo> m_LADSPAList;
	LPluginInfo m_CurrentPlugin;

	float         m_Gain;
	bool          m_Amped;

	struct ChannelData
	{
		unsigned long PluginIndex;
		char          Name[256];
		char          Maker[256];
		unsigned long MaxInputPorts;
		unsigned long InputPorts;
		char         *InputPortNames;
		PortRange    *SetInputPortRanges;
		PortRange    *GetInputPortRanges;
	};

	ChannelData     m_ChannelData;
};

#endif
