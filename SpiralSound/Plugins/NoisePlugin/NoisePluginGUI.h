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
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

#include "../SpiralPluginGUI.h"
#include "NoisePlugin.h"

#ifndef PLUGINGUI
#define PLUGINGUI

class NoisePluginGUI : public SpiralPluginGUI
{
public:
	NoisePluginGUI(int w, int h, NoisePlugin *o,ChannelHandler *ch,const HostInfo *Info);
	
	virtual void UpdateValues(SpiralPlugin *o);
	
protected:
    const string GetHelpText(const string &loc);	
	
private:

	Fl_Check_Button *White;
	Fl_Check_Button *Pink;
		
	//// Callbacks ////
	inline void cb_White_i(Fl_Check_Button* o, void* v);
    static void cb_White(Fl_Check_Button*, void*);
	inline void cb_Pink_i(Fl_Check_Button* o, void* v);
    static void cb_Pink(Fl_Check_Button*, void*);
};

#endif
