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
#include <FL/Fl_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>

#include "EnvelopePlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef EnvelopeGUI
#define EnvelopeGUI


class EnvelopePluginGUI : public SpiralPluginGUI
{
public:
	EnvelopePluginGUI(int w, int h, EnvelopePlugin *o,ChannelHandler *ch,const HostInfo *Info);
	
	virtual void UpdateValues(SpiralPlugin *o);

protected:
    const string GetHelpText(const string &loc);	
	
private:
	
	Fl_Slider 		*Thresh;
	Fl_Slider 		*Attack;
	Fl_Slider 		*Decay;
	Fl_Slider 		*Sustain;
	Fl_Slider 		*Release;
	Fl_Slider 		*Volume;
	Fl_Button		*m_pop;
	Fl_Output 		*m_out_thresh;
	Fl_Output 		*m_out_attack;
	Fl_Output 		*m_out_decay;
	Fl_Output 		*m_out_sustain;
	Fl_Output 		*m_out_release;
	Fl_Output 		*m_out_volume;
	
	//// Callbacks ////
	inline void cb_Thresh_i(Fl_Slider* o, void* v);
    static void cb_Thresh(Fl_Slider*, void*);
	inline void cb_Attack_i(Fl_Slider* o, void* v);
    static void cb_Attack(Fl_Slider*, void*);
	inline void cb_Decay_i(Fl_Slider* o, void* v);
	static void cb_Decay(Fl_Slider* o, void* v); 
	inline void cb_Sustain_i(Fl_Slider* o, void* v);
	static void cb_Sustain(Fl_Slider* o, void* v); 
	inline void cb_Release_i(Fl_Slider* o, void* v);
	static void cb_Release(Fl_Slider* o, void* v); 
	inline void cb_Volume_i(Fl_Slider* o, void* v);
	static void cb_Volume(Fl_Slider* o, void* v); 
	inline void cb_pop_i(Fl_Button *o, void *v);
	static void cb_pop(Fl_Button *o, void *v);

};

#endif
