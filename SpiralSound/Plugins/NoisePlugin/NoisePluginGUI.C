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

#include "NoisePluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

NoisePluginGUI::NoisePluginGUI(int w, int h,NoisePlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
    White = new Fl_Check_Button(15, 15, 55, 30, "White");
    White->type(102);
	White->labelsize(10);
    White->down_box(FL_DIAMOND_DOWN_BOX);
    White->selection_color(GUI_COLOUR);
    White->set();
	White->callback((Fl_Callback*)cb_White);
	              
	Pink = new Fl_Check_Button(15, 35, 55, 30, "Pink");
    Pink->type(102);
 	Pink->labelsize(10);
    Pink->down_box(FL_DIAMOND_DOWN_BOX);
    Pink->selection_color(GUI_COLOUR);
    Pink->callback((Fl_Callback*)cb_Pink);
	end();
}

void NoisePluginGUI::UpdateValues(SpiralPlugin *o)
{
	NoisePlugin *Plugin = (NoisePlugin*)o;

	switch (Plugin->GetType())
	{
		case NoisePlugin::WHITE : White->value(1); break;
		case NoisePlugin::PINK  : Pink->value(1); break;
	}
}
	
//// Callbacks ////
inline void NoisePluginGUI::cb_White_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)NoisePlugin::WHITE); }
void NoisePluginGUI::cb_White(Fl_Check_Button* o, void* v)
{ ((NoisePluginGUI*)(o->parent()))->cb_White_i(o,v); }

inline void NoisePluginGUI::cb_Pink_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)NoisePlugin::PINK); }
void NoisePluginGUI::cb_Pink(Fl_Check_Button* o, void* v)
{ ((NoisePluginGUI*)(o->parent()))->cb_Pink_i(o,v); }


const string NoisePluginGUI::GetHelpText(const string &loc){
    return string("") 
	+ "Makes noise, white noise is just raw randomness, pink noise\n"
	+ "is white noise filtered to contain equal amounts of each\n"
	+ "frequency, it should sound more natural and be more useful\n"
	+ "for synthesis uses";
}
