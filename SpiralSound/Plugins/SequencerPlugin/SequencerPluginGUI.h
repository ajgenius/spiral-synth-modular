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
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Output.H>

#include "SequencerPlugin.h"
#include "../SpiralPluginGUI.h"
#include "../Widgets/Fl_Knob.H"
#include "../Widgets/Fl_EventMap.h"

#ifndef MixerGUI
#define MixerGUI


class SequencerPluginGUI : public SpiralPluginGUI
{
public:
	SequencerPluginGUI(int w, int h, SequencerPlugin *o, ChannelHandler *ch,const HostInfo *Info);
	
	virtual void UpdateValues(SpiralPlugin *o);

private:
	
	Fl_Scroll*  m_Scroll;
	Fl_Button*  m_NewPattern;
	Fl_Button*  m_NoteCut;
	Fl_Knob*    m_Zoom;
	Fl_Knob*    m_Length;
	Fl_Knob*    m_Speed;
	Fl_Button*  m_Clear;
	
	class PatternWin : public Fl_Double_Window
	{
	public:
		PatternWin(int w,int h,const char* n);
		
	private:
		Fl_Scroll*  m_Scroll;
		Fl_EventMap* m_Melody;
	};
	
	map<int,PatternWin*> m_PatternWinMap;
	Fl_EventMap* m_ArrangementMap;
	
	//// Callbacks ////
	inline void cb_Zoom_i(Fl_Knob* o, void* v);
	static void cb_Zoom(Fl_Knob* o, void* v); 
	inline void cb_NoteCut_i(Fl_Button* o, void* v);
	static void cb_NoteCut(Fl_Button* o, void* v);
	inline void cb_Pattern_i(Fl_Counter* o, void* v);
	static void cb_Pattern(Fl_Counter* o, void* v); 
	inline void cb_Length_i(Fl_Knob* o, void* v);
	static void cb_Length(Fl_Knob* o, void* v); 
	inline void cb_Speed_i(Fl_Knob* o, void* v);
	static void cb_Speed(Fl_Knob* o, void* v); 
	inline void cb_Clear_i(Fl_Button* o, void* v);
	static void cb_Clear(Fl_Button* o, void* v); 
	inline void cb_NewPattern_i(Fl_Button* o, void* v);
	static void cb_NewPattern(Fl_Button* o, void* v); 
	
	inline void cb_ArrangeRM_i(Fl_Button* o, void* v);
	static void cb_ArrangeRM(Fl_Button* o, void* v); 
};

#endif
