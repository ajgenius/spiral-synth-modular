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

#include "SmoothPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////

SmoothPluginGUI::SmoothPluginGUI(int w, int h,SmoothPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
	m_Up = new Fl_Knob(20, 25, 40, 40, "Up");	
	m_Up->color(GUI_COLOUR);
	m_Up->type(Fl_Knob::DOTLIN);
   	m_Up->labelsize(10);
   	m_Up->maximum(1);
   	m_Up->step(0.001);
   	m_Up->value(0.5);   
	m_Up->callback((Fl_Callback*)cb_Up);
	add(m_Up);
	
	m_Down = new Fl_Knob(75, 25, 40, 40, "Down");	
	m_Down->color(GUI_COLOUR);
	m_Down->type(Fl_Knob::DOTLIN);
   	m_Down->labelsize(10);
   	m_Down->maximum(1);
   	m_Down->step(0.001);
   	m_Down->value(0.5);   
	m_Down->callback((Fl_Callback*)cb_Down);
	add(m_Down);
	
	end();
}



void SmoothPluginGUI::UpdateValues(SpiralPlugin *o)
{
	SmoothPlugin *Plugin = (SmoothPlugin*)o;
	
	m_Up->value(Plugin->GetUp());
	m_Down->value(Plugin->GetDown());
}
	
inline void SmoothPluginGUI::cb_Down_i(Fl_Knob* o, void* v)
{ m_GUICH->Set("Down",(float)o->value()); }

void SmoothPluginGUI::cb_Down(Fl_Knob* o, void* v)
{ ((SmoothPluginGUI*)(o->parent()))->cb_Down_i(o,v);}

inline void SmoothPluginGUI::cb_Up_i(Fl_Knob* o, void* v)
{ m_GUICH->Set("Up",(float)o->value()); }

void SmoothPluginGUI::cb_Up(Fl_Knob* o, void* v)
{ ((SmoothPluginGUI*)(o->parent()))->cb_Up_i(o,v);}
