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
#include "ladspa.h"
#include <string>

#include "LADSPAPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef LADSPAGUI
#define LADSPAGUI

#include "ladspa.h"
#include <stdio.h>
#include <math.h>
#include <dlfcn.h>
#include <vector>
#include "utils.h"

class LPluginInfo {
public:
	string Filename;
	string Label;
	string Name;
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

class LADSPAPluginGUI : public SpiralPluginGUI
{
public:
	LADSPAPluginGUI(int w, int h, LADSPAPlugin *o,const HostInfo *Info);
	~LADSPAPluginGUI();

	virtual void UpdateValues();
	virtual SpiralPlugin* GetPlugin() { return m_Plugin; }

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
		
	LADSPAPlugin *m_Plugin;	
	
private:
	vector<LPluginInfo> PluginList;
	LPluginInfo CurrentPlugin;
	friend void describePluginLibrary(const char * pcFullFilename, void * pvPluginHandle, LADSPA_Descriptor_Function pfDescriptorFunction);
	void refreshPluginList(void);

	Fl_Scroll	      *m_InputScroll;
	Fl_Pack  		  *m_InputPack;
	Fl_Hold_Browser   *m_Browser;
	Fl_Knob	          *m_OutputGain;
	Fl_Box            *m_Name;
	Fl_Box            *m_Maker;
	Fl_Button		  *m_UpdateInputs;
	Fl_Button		  *m_UpdateMinMax;
	Fl_Button		  *m_PowerAmp;
	vector<Fl_Output*> m_PortOutput;
	vector<Fl_Input*>  m_PortMin;
	vector<Fl_Input*>  m_PortMax;	
	vector<Fl_Check_Button*> m_PortClamp;
	
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

};



#endif
