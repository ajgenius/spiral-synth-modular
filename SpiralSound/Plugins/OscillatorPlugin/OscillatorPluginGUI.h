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
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>

#include "../Widgets/Fl_Knob.H"
#include "../SpiralPluginGUI.h"
#include "OscillatorPlugin.h"

#ifndef SCOPEGUI
#define SCOPEGUI

class OscillatorPluginGUI : public SpiralPluginGUI
{
public:
	OscillatorPluginGUI(int w, int h, OscillatorPlugin *o,ChannelHandler *ch,const HostInfo *Info);
	
	virtual void UpdateValues(SpiralPlugin *o);
	
protected:
    const string GetHelpText(const string &loc);	
	
private:

	Fl_Check_Button *ShapeSquare;
	Fl_Pixmap 		 pixmap_Square;
	Fl_Check_Button *ShapeNoise;
	Fl_Pixmap 		 pixmap_Noise;
	Fl_Check_Button *ShapeSaw;
	Fl_Pixmap 		 pixmap_Saw;
	Fl_Knob 		*Freq;
	Fl_Knob 		*ModAmount;
	Fl_Knob 		*FineTune;
	Fl_Slider 		*PulseWidth;
	Fl_Slider 		*SHLen;
	Fl_Button		*m_pop;
	Fl_Output		*m_out_freq;
	Fl_Output		*m_out_mod;
	Fl_Output		*m_out_pulseW;
	Fl_Output		*m_out_SHlen;
	
	float m_FineFreq;
	int m_Octave;
		
	//// Callbacks ////
	inline void cb_Freq_i(Fl_Knob* o, void* v);
    static void cb_Freq(Fl_Knob*, void*);
	inline void cb_FineTune_i(Fl_Knob* o, void* v);
	static void cb_FineTune(Fl_Knob* o, void* v);
	inline void cb_ModAmount_i(Fl_Knob* o, void* v);
	static void cb_ModAmount(Fl_Knob* o, void* v);
	inline void cb_PulseWidth_i(Fl_Slider* o, void* v);
	static void cb_PulseWidth(Fl_Slider* o, void* v); 
	inline void cb_SHLen_i(Fl_Slider* o, void* v);
	static void cb_SHLen(Fl_Slider* o, void* v); 
	inline void cb_Square_i(Fl_Check_Button* o, void* v);
    static void cb_Square(Fl_Check_Button*, void*);
	inline void cb_Saw_i(Fl_Check_Button* o, void* v);
    static void cb_Saw(Fl_Check_Button*, void*);
	inline void cb_Noise_i(Fl_Check_Button* o, void* v);
	static void cb_Noise(Fl_Check_Button* o, void* v); 
	inline void cb_pop_i(Fl_Button* o, void*);
  	static void cb_pop(Fl_Button* o, void*);
};

#endif
