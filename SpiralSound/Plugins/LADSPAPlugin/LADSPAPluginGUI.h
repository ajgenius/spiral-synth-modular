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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Scroll.H>
#include "../Widgets/Fl_Knob.H"

#include <vector>
#include <string>

#include "LADSPAPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef LADSPAGUI
#define LADSPAGUI

#include <cstdio>
#include <cmath>
#include <dlfcn.h>
#include <vector>

class LADSPAPluginGUI : public SpiralPluginGUI
{
public:
	LADSPAPluginGUI(int w, int h, LADSPAPlugin *o, ChannelHandler *ch, const HostInfo *Info, const vector<LPluginInfo> &PVec);
	~LADSPAPluginGUI();

	virtual void UpdateValues(SpiralPlugin *o);

	void SetName(const char *s);
	void SetMaker(const char *s);

	void ClearPortInfo();
	void AddPortInfo(const char *Info);
	void UpdatePortDisplay(int n, float v);
	void SetMinMax(int n, float min, float max, bool clamp);
	void GetMinMax(int n, float &min, float &max, bool &clamp);

	string GetFilename() { return m_Filename; }
	string GetLabel() { return m_Label; }
	void SetFilename(string s) { m_Filename=s; }
	void SetLabel(string s) { m_Label=s; }

private:
	LPluginInfo m_CurrentPlugin;

	Fl_Scroll		*m_InputScroll;
	Fl_Pack  		*m_InputPack;
	Fl_Hold_Browser		*m_Browser;
	Fl_Knob			*m_OutputGain;
	Fl_Box			*m_Name;
	Fl_Box			*m_Maker;
	Fl_Button		*m_UpdateInputs;
	Fl_Button		*m_UpdateMinMax;
	Fl_Button		*m_PowerAmp;
	vector<Fl_Output*>	 m_PortOutput;
	vector<Fl_Input*>	 m_PortMin;
	vector<Fl_Input*>	 m_PortMax;
	vector<Fl_Check_Button*> m_PortClamp;

	vector<LPluginInfo> PluginList;

	// this is needed as fltk seems to crash if you delete
	// the pack, is won't delete the children properly???
	vector<Fl_Group*>   m_PackVec;

	int inited;

	string m_Filename;
	string m_Label;

	inline void cb_Select_i(Fl_Hold_Browser* o);
	static void cb_Select(Fl_Hold_Browser* o);
	inline void cb_Gain_i(Fl_Knob* o, void* v);
	static void cb_Gain(Fl_Knob* o, void* v);
	inline void cb_MinMax_i(Fl_Button* o, void* v);
	static void cb_MinMax(Fl_Button* o, void* v);
	inline void cb_PowerAmp_i(Fl_Button* o, void* v);
	static void cb_PowerAmp(Fl_Button* o, void* v);

	// This lot is for copying plugin info from the audio thread
	// to the GUI thread, via ChannelHandler

	char         *m_NameString;		// Plugin Name
	char         *m_MakerString;		// Plugin Maker;
	unsigned long m_InputPortCountMax;	// Maximum number of input ports
						// Corresponds to input port count of one
						// (or more) plugins found
	unsigned long m_InputPortCount;		// Number of input ports in current plugin
	float        *m_InputPortMin;		// Input port range minima
	float        *m_InputPortMax;		// Input port range maxima
	bool         *m_InputPortClamp;		// Input port clamp state
	char         *m_InputPortNames;		// Input port names
};



#endif
