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

#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Box.H>

#include "../Widgets/Fl_Knob.H"
#include "../SpiralPluginGUI.h"
#include "WaveTablePlugin.h"

#ifndef SCOPEGUI
#define SCOPEGUI

class WaveTablePluginGUI : public SpiralPluginGUI
{
public:
	WaveTablePluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch, const HostInfo *Info);
	
	virtual void UpdateValues(SpiralPlugin* o);
			
private:

	Fl_Check_Button *ShapeSquare;
	Fl_Pixmap 		 pixmap_Square;
	Fl_Check_Button *ShapeSaw;
	Fl_Pixmap 		 pixmap_Saw;
	Fl_Check_Button *ShapeRevSaw;
	Fl_Pixmap 		 pixmap_RevSaw;
	Fl_Check_Button *ShapeTri;
	Fl_Pixmap 		 pixmap_Tri;
	Fl_Check_Button *ShapeSine;
	Fl_Pixmap 		 pixmap_Sine;
	Fl_Check_Button *Pulse1;
	Fl_Pixmap 		 pixmap_Pulse1;
	Fl_Check_Button *Pulse2;
	Fl_Pixmap 		 pixmap_Pulse2;
	Fl_Check_Button *ShapeInvSine;
	Fl_Pixmap 		 pixmap_InvSine;
	
	Fl_Knob 		*Freq;
	Fl_Knob 		*ModAmount;
	Fl_Knob 		*FineTune;
	Fl_Button		*m_pop;
	Fl_Output		*m_out_freq;
	Fl_Output		*m_out_mod;
	
	//// Callbacks ////
	inline void cb_Freq_i(Fl_Knob* o, void* v);
    static void cb_Freq(Fl_Knob*, void*);
	inline void cb_FineTune_i(Fl_Knob* o, void* v);
	static void cb_FineTune(Fl_Knob* o, void* v);
	inline void cb_ModAmount_i(Fl_Knob* o, void* v);
	static void cb_ModAmount(Fl_Knob* o, void* v);
	
	inline void cb_Square_i(Fl_Check_Button* o, void* v);
    static void cb_Square(Fl_Check_Button*, void*);
	inline void cb_Saw_i(Fl_Check_Button* o, void* v);
    static void cb_Saw(Fl_Check_Button*, void*);
	inline void cb_Sine_i(Fl_Check_Button* o, void* v);
	static void cb_Sine(Fl_Check_Button* o, void* v); 
	inline void cb_RevSaw_i(Fl_Check_Button* o, void* v);
	static void cb_RevSaw(Fl_Check_Button* o, void* v); 
	inline void cb_Tri_i(Fl_Check_Button* o, void* v);
	static void cb_Tri(Fl_Check_Button* o, void* v); 
	inline void cb_Pulse1_i(Fl_Check_Button* o, void* v);
	static void cb_Pulse1(Fl_Check_Button* o, void* v); 
	inline void cb_Pulse2_i(Fl_Check_Button* o, void* v);
	static void cb_Pulse2(Fl_Check_Button* o, void* v); 
	inline void cb_InvSine_i(Fl_Check_Button* o, void* v);
	static void cb_InvSine(Fl_Check_Button* o, void* v); 
	inline void cb_pop_i(Fl_Button* o, void*);
  	static void cb_pop(Fl_Button* o, void*);
};

#endif
