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

#ifndef __ladspa_plugin_h__
#define __ladspa_plugin_h__

#include <FL/Fl_Pixmap.H>
#include <ladspa.h>

#include "../SpiralPlugin.h"
#include "LADSPAInfo.h"

static const unsigned int NUM_PORTS = 8;

struct PortSettings
{
	float   Min;
	float   Max;
	bool    Clamp;
	float   Default;
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

	float          GetGain() { return m_Gain; }
	bool           GetAmped() { return m_Amped; }
	const char    *GetName() { return (const char *)m_Name; }
	const char    *GetMaker() { return (const char *)m_Maker; }
	unsigned long  GetInputPortCount() { return m_InputPortCount; }
	const char    *GetPortName(unsigned long p)
	{
		return (const char *)(m_OutData.InputPortNames + p * 256); 
	}
	PortSettings GetPortSettings(unsigned long p)
	{
		PortSettings settings;
		settings.Min = m_PortMin[p];
		settings.Max = m_PortMax[p];
		settings.Clamp = m_PortClamp[p];
		settings.Default = m_PortDefault[p];
		return settings;
	}

	enum GUICommands{NONE,SETPORTSETTINGS,SELECTPLUGIN};

private:

	void UpdatePortRange(void);
	bool UpdatePlugin(unsigned long UniqueID, bool PortClampReset=true);
	void SetPortSettings(void);

	void LoadPluginList(void);

	void * PlugHandle;
	const LADSPA_Descriptor * PlugDesc;

	vector<LADSPA_Data*> m_LADSPABufVec;
	LADSPA_Handle PlugInstHandle;

	vector<int>   m_PortID;
	vector<float> m_PortMin;
	vector<float> m_PortMax;
	vector<bool>  m_PortClamp;
	vector<float> m_PortDefault;

	int           m_Version;

	// our database of ladspa plugins
	LADSPAInfo    m_LADSPAInfo;

	unsigned long m_PluginIndex;
	unsigned long m_UniqueID;

	float         m_Gain;
	bool          m_Amped;
	unsigned long m_MaxInputPortCount;
	unsigned long m_InputPortCount;
	char          m_Name[256];
	char          m_Maker[256];

	// Data sent to GUI
	struct OutputChannelData
	{
		char         *InputPortNames;
		PortSettings *InputPortSettings;
		float        *InputPortValues;
	};

	// Data received from GUI
	struct InputChannelData
	{
		unsigned long PluginIndex;
		float         Gain;
		bool          Amped;
		PortSettings *InputPortSettings;
	};

	OutputChannelData     m_OutData;
	InputChannelData      m_InData;
};

#endif // __ladspa_plugin_h__
