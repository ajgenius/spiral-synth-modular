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
#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Box.H>

#include "../Widgets/Fl_Knob.H"
#include "../Widgets/Fl_DragBar.H"
#include "../Widgets/Fl_LED_Button.H"
#include "JackPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef SCOPEGUI
#define SCOPEGUI

class JackPluginGUI : public SpiralPluginGUI
{
public:
	JackPluginGUI(int w, int h, JackPlugin *o,ChannelHandler *ch,const HostInfo *Info);
	
	virtual void UpdateValues(SpiralPlugin *o);
	virtual void Update();
	
protected:
    const string GetHelpText(const string &loc);	
		
private:

	Fl_LED_Button *m_Indicator;
	Fl_Button     *m_Attach;
	Fl_Button     *m_Detach;
	
	char           m_InputName[NUM_INPUTS][256];
	Fl_Box        *m_InputLabel[NUM_INPUTS];
	Fl_Button     *m_InputButton[NUM_INPUTS];
	
	char           m_OutputName[NUM_INPUTS][256];
	Fl_Box        *m_OutputLabel[NUM_OUTPUTS];
	Fl_Button     *m_OutputButton[NUM_OUTPUTS];
		
	//// Callbacks ////
	inline void cb_Attach_i(Fl_Button* o, void* v);
	static void cb_Attach(Fl_Button* o, void* v); 
	inline void cb_Detach_i(Fl_Button* o, void* v);
	static void cb_Detach(Fl_Button* o, void* v); 
	inline void cb_OutputConnect_i(Fl_Button* o, void* v);
	static void cb_OutputConnect(Fl_Button* o, void* v); 
	inline void cb_InputConnect_i(Fl_Button* o, void* v);
	static void cb_InputConnect(Fl_Button* o, void* v); 
};

#endif
