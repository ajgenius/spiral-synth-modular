/*  SpiralPlugin
 *  Copyleft (C) 2000 David Griffiths <dave@pawfal.org>
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

#ifndef __ladspa_plugin_gui_h__
#define __ladspa_plugin_gui_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>
#include "../Widgets/Fl_Knob.H"

#include <vector>
#include <string>
#include <cstdio>
#include <cmath>
#include <dlfcn.h>

#include "LADSPAPlugin.h"
#include "LADSPAInfo.h"
#include "../SpiralPluginGUI.h"

class LADSPAPluginGUI : public SpiralPluginGUI
{
public:
	LADSPAPluginGUI(int w, int h,
	                LADSPAPlugin *o,
	                ChannelHandler *ch,
	                const HostInfo *Info,
	                const std::vector<LADSPAInfo::PluginEntry> &PVec);
	~LADSPAPluginGUI();

	virtual void UpdateValues(SpiralPlugin *o);
	virtual void Update(void);

	void SetName(const char *s);
	void SetMaker(const char *s);

	void ClearPortInfo();
	void AddPortInfo(const char *Info);
	void UpdatePortDisplay(int n, PortValues pv);
	void SetPortSettings(unsigned long n, float min, float max, bool clamp, float defolt);

private:
	Fl_Tabs                       *m_Tab;
	Fl_Group                      *m_ControlGroup;
	Fl_Group                      *m_SetupGroup;
	Fl_Scroll                     *m_InputScroll;
	Fl_Pack                       *m_InputPack;
	Fl_Choice                     *m_Browser;
	Fl_Box                        *m_Name;
	Fl_Box                        *m_Maker;
	Fl_Button                     *m_UpdateInputs;
	Fl_Button                     *m_UpdatePortSettings;
	std::vector<Fl_Output*>        m_PortOutput;
	std::vector<Fl_Input*>         m_PortMin;
	std::vector<Fl_Input*>         m_PortMax;
	std::vector<Fl_Check_Button*>  m_PortClamp;
	std::vector<Fl_Input*>         m_PortDefault;
	std::vector<Fl_Knob*>          m_PortDefaultKnob;

	std::vector<LADSPAInfo::PluginEntry> PluginList;

	// this is needed as fltk seems to crash if you delete
	// the pack, is won't delete the children properly???
	std::vector<Fl_Group*>   m_PackVec;

	int inited;

	inline void cb_Select_i(Fl_Choice* o);
	static void cb_Select(Fl_Choice* o);
	inline void cb_PortSettings_i(Fl_Button* o, void* v);
	static void cb_PortSettings(Fl_Button* o, void* v);

	struct InChannelData
	{
		unsigned long   PluginIndex;
		char            Name[256];
		char            Maker[256];
		unsigned long   MaxInputPortCount;
		unsigned long   InputPortCount;
		char           *InputPortNames;
		PortSettings   *InputPortSettings;
		PortValues     *InputPortValues;
	};

	InChannelData  m_InData;
};



#endif // __ladspa_plugin_gui_h__
