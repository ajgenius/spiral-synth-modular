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

#include "list"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pack.H>
#include "../Widgets/Fl_LED_Button.H"

#include "SeqSelectorPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef MixerGUI
#define MixerGUI

static const int NUM_VALUES = 8;

class CountLine : public Fl_Group
{
public:
	CountLine(int n);
	~CountLine();
	
	void  SetVal(int n, float val);
	float GetVal(int n);
	void  SetLED(bool s) { m_Flasher->value(s); }
	
private:
	Fl_LED_Button *m_Flasher;
	Fl_Counter    *m_Counter[NUM_VALUES];
	
	char m_Count[32];
};

class SeqSelectorPluginGUI : public SpiralPluginGUI
{
public:
	SeqSelectorPluginGUI(int w, int h, SeqSelectorPlugin *o,const HostInfo *Info);
	
	virtual void UpdateValues();
	virtual SpiralPlugin* GetPlugin() { return m_Plugin; }
	
	SeqSelectorPlugin *m_Plugin;
		
	void  AddLine(int* Val=NULL);
	void  RemoveLine();
	float GetVal(int l, int v);
	int   GetNumLines() { return m_LineVec.size(); }
	void  SetLED(int n);
	
	void StreamOut(ostream &s);
	void StreamIn(istream &s);
	
private:
	
	Fl_Pack    *m_Main;
	Fl_Scroll  *m_Scroll;
	Fl_Button  *m_New;
	Fl_Button  *m_Delete;
	Fl_Counter *m_Begin;
	Fl_Counter *m_End;
	Fl_Button  *m_UseRange;
	
	list<CountLine*> m_LineVec;
	
	//// Callbacks ////
	inline void cb_New_i(Fl_Button* o, void* v);
	static void cb_New(Fl_Button* o, void* v); 
	inline void cb_Delete_i(Fl_Button* o, void* v);
	static void cb_Delete(Fl_Button* o, void* v); 
	inline void cb_Begin_i(Fl_Counter* o, void* v);
	static void cb_Begin(Fl_Counter* o, void* v); 
	inline void cb_End_i(Fl_Counter* o, void* v);
	static void cb_End(Fl_Counter* o, void* v); 
	inline void cb_UseRange_i(Fl_Button* o, void* v);
	static void cb_UseRange(Fl_Button* o, void* v); 
};

#endif
