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
	~LADSPAPluginGUI(void);

	virtual void UpdateValues(SpiralPlugin *o);
	virtual void Update(void);

private:
	void AddPortInfo(const char *Info);
	void SetTabIndex(int index);
	void SetUpdateInputs(bool state);
	void SetPluginIndex(unsigned long n);
	void SetName(const char *s);
	void SetMaker(const char *s);
	void SetPortSettings(unsigned long n, float min, float max, bool clamp, float defolt);
	void SetDefaultAdjust(unsigned long n);
	void UpdateDefaultAdjustControls(void);

	Fl_Box                        *m_NameLabel;
	Fl_Box                        *m_MakerLabel;
	Fl_Tabs                       *m_Tab;
	Fl_Group                      *m_ControlGroup;
	Fl_Scroll                     *m_ControlScroll;
	Fl_Pack                       *m_ControlPack;
	Fl_Group                      *m_SetupGroup;
	Fl_Choice                     *m_Browser;
	Fl_Scroll                     *m_InputScroll;
	Fl_Pack                       *m_InputPack;
	Fl_Check_Button               *m_UpdateInputs;
	std::vector<Fl_Output*>        m_PortValue;
	std::vector<Fl_Input*>         m_PortMin;
	std::vector<Fl_Input*>         m_PortMax;
	std::vector<Fl_Check_Button*>  m_PortClamp;
	std::vector<Fl_Input*>         m_PortDefault;
	std::vector<Fl_Knob*>          m_PortDefaultAdjust;

	std::vector<LADSPAInfo::PluginEntry> m_PluginList;

	unsigned long                  m_PortIndex;
	float                          m_Default;
	float                          m_Min;
	float                          m_Max;
	bool                           m_Clamp;

	unsigned long                  m_PluginIndex;
	int                            m_TabIndex;
	bool                           m_UpdateInputState;
	char                           m_Name[256];
	char                           m_Maker[256];
	unsigned long                  m_MaxInputPortCount;
	unsigned long                  m_InputPortCount;
	char                          *m_InputPortNames;
	PortSettings                  *m_InputPortSettings;
	PortValues                    *m_InputPortValues;
	float                         *m_InputPortDefaults;

	inline void cb_TabChange_i(Fl_Tabs *o);
	static void cb_TabChange(Fl_Tabs *o);
	inline void cb_Select_i(Fl_Choice* o);
	static void cb_Select(Fl_Choice* o);
	inline void cb_UpdateInputs_i(Fl_Check_Button* o);
	static void cb_UpdateInputs(Fl_Check_Button* o);
	inline void cb_Default_i(Fl_Input* o);
	static void cb_Default(Fl_Input* o);
	inline void cb_Min_i(Fl_Input* o);
	static void cb_Min(Fl_Input* o);
	inline void cb_Max_i(Fl_Input* o);
	static void cb_Max(Fl_Input* o);
	inline void cb_Clamp_i(Fl_Check_Button* o);
	static void cb_Clamp(Fl_Check_Button* o);
	inline void cb_DefaultAdjust_i(Fl_Knob *o);
	static void cb_DefaultAdjust(Fl_Knob *o);
};



#endif // __ladspa_plugin_gui_h__
