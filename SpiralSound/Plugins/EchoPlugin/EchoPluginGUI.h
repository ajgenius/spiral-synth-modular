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
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>

#include "EchoPlugin.h"
#include "../SpiralPluginGUI.h"
#include "../Widgets/Fl_Knob.H"

#ifndef FilterGUI
#define FilterGUI


class EchoPluginGUI : public SpiralPluginGUI
{
public:
	EchoPluginGUI(int w, int h, EchoPlugin *o,const HostInfo *Info);
	
	virtual void UpdateValues();
	virtual SpiralPlugin* GetPlugin() { return m_Plugin; }
	
	EchoPlugin *m_Plugin;	
	
private:
	
	Fl_Slider 		*m_Delay;
	Fl_Slider 		*m_Feedback;
	Fl_Button 		*m_pop;	
	Fl_Output 		*m_out_delay;
	Fl_Output 		*m_out_feedback;

	//// Callbacks ////
	
	inline void cb_Delay_i(Fl_Slider* o, void* v);
    static void cb_Delay(Fl_Slider*, void*);
	inline void cb_Feedback_i(Fl_Slider* o, void* v);
	static void cb_Feedback(Fl_Slider* o, void* v); 
  	inline void cb_pop_i(Fl_Button* o, void*);
  	static void cb_pop(Fl_Button* o, void*);
};

#endif
