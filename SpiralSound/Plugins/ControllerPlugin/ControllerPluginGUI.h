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
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Slider.H>

#include "ControllerPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef MixerGUI
#define MixerGUI

static int Numbers[MAX_CHANNELS];

class ControllerPluginGUI : public SpiralPluginGUI
{
public:
	ControllerPluginGUI(int w, int h, ControllerPlugin *o,const HostInfo *Info);
	
	virtual void UpdateValues();
	virtual SpiralPlugin* GetPlugin() { return m_Plugin; }
	
	ControllerPlugin *m_Plugin;	
	
	void StreamIn(istream &s);
	void StreamOut(ostream &s);
	
private:

	void Clear();
	
	class CVGUI
	{
	public:
		CVGUI::CVGUI(int n, ControllerPluginGUI *p);
	
		Fl_Group     *m_SliderGroup;
		Fl_Input     *m_Title;
		Fl_Int_Input *m_Min;
		Fl_Int_Input *m_Max;
		Fl_Slider    *m_Chan;
	};
	
	friend class CVGUI;
	
	Fl_Pack *m_MainPack;
	Fl_Button *m_Add;
	Fl_Button *m_Delete;
	
	vector<CVGUI*> m_GuiVec;
	void AddCV();
	void DeleteCV();
	
	int m_CVCount;
	
	//// Callbacks ////
	inline void cb_Chan_i(Fl_Slider* o, void* v);
	static void cb_Chan(Fl_Slider* o, void* v); 
	inline void cb_Add_i(Fl_Button* o, void* v); 
	static void cb_Add(Fl_Button* o, void* v);
	inline void cb_Delete_i(Fl_Button* o, void* v); 
	static void cb_Delete(Fl_Button* o, void* v); 

	friend istream &operator>>(istream &s, ControllerPluginGUI &o);
};

istream &operator>>(istream &s, ControllerPluginGUI &o);

#endif
