/*  LADSPAPlugin.h
 *  Copyleft (C) 2001 David Griffiths <dave@pawfal.org>
 *  LADSPA Plugin by Nicolas Noble <nicolas@nobis-crew.org>
 *  Modified by Mike Rawes <myk@waxfrenzy.org>
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

#include <config.h>

#include <FL/Fl_Pixmap.H>
#include <ladspa.h>

#include "../SpiralPlugin.h"
#include "LADSPAInfo.h"

#ifdef USE_POSIX_SHM
#include <sys/types.h> // For pid_t data member
#include <unistd.h>
#endif

struct PortSetting
{
	float Min;
	float Max;
	bool  Clamp;
	float LogBase;     // >1.0 -> Logarithmic, otherwise linear
	bool  Integer;
};
struct PortValue
{
	float Value;
	bool  Connected;
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

	unsigned long  GetUniqueID() { return m_UniqueID; }
	const char    *GetName() { return (const char *)m_Name; }
	const char    *GetMaker() { return (const char *)m_Maker; }
	int            GetTabIndex() { return m_TabIndex; }
	bool           GetUpdateInputs() { return m_UpdateInputs; }
	unsigned long  GetInputPortCount() { return m_InputPortCount; }
	const char    *GetInputPortName(unsigned long p)
	{
		return (const char *)(m_OutData.InputPortNames + p * 256);
	}
	PortSetting GetInputPortSetting(unsigned long p)
	{
		return m_OutData.InputPortSettings[p];
	}
	float          GetInputPortDefault(unsigned long p)
	{
		return m_OutData.InputPortDefaults[p];
	}

	enum GUICommands
	{
		NONE,
		SETTABINDEX,
		SELECTPLUGIN,
		CLEARPLUGIN,
		SETUPDATEINPUTS,
		SETDEFAULT,
		SETMIN,
		SETMAX,
		SETCLAMP
	};

private:
	bool UpdatePlugin(unsigned long UniqueID);
	bool SelectPlugin(unsigned long UniqueID);
	void ClearPlugin(void);
	void ResetPortSettings(void);
	void SetGUIExports(void);

	const LADSPA_Descriptor *m_PlugDesc;
	vector<LADSPA_Data*>     m_LADSPABufVec;
	LADSPA_Handle            m_PlugInstHandle;

	vector<int>     m_PortID;
	vector<float>   m_InputPortMin;
	vector<float>   m_InputPortMax;
	vector<bool>    m_InputPortClamp;
	vector<float>   m_InputPortDefault;

	int             m_Version;

	// our database of ladspa plugins
	LADSPAInfo     *m_LADSPAInfo;

	unsigned long   m_PluginIndex;
	unsigned long   m_UniqueID;
	int             m_TabIndex;
	bool            m_UpdateInputs;

	unsigned long   m_MaxInputPortCount;
	unsigned long   m_InputPortCount;
 char            m_Name[256];
	char            m_Maker[256];

	// Data sent to GUI
	struct OutputChannelData
	{
		char         *InputPortNames;
		PortSetting  *InputPortSettings;
		PortValue    *InputPortValues;
		float        *InputPortDefaults;
 };

	// Data received from GUI
	struct InputChannelData
	{
		unsigned long UniqueID;
		int           TabIndex;
		bool          UpdateInputs;
		unsigned long InputPortIndex;
		float         InputPortDefault;
		float         InputPortMin;
		float         InputPortMax;
		bool          InputPortClamp;
	};

	OutputChannelData     m_OutData;
	InputChannelData      m_InData;

#ifdef USE_POSIX_SHM
// SHM stuff - for sharing the LADSPA Plugin database
	static const char * const m_SHMRefCountPath = "/SpiralSynthModular-LADSPAPlugin-RefCount";
	static const char * const m_SHMLDBPath = "/SpiralSynthModular-LADSPAPlugin-Database";

	unsigned long *m_SHMRefCount;
	LADSPAInfo   **m_SHMLDB;
#endif
};

#endif // __ladspa_plugin_h__
