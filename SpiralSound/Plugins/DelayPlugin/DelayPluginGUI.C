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

#include "DelayPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////

DelayPluginGUI::DelayPluginGUI(int w, int h,DelayPlugin *o,const HostInfo *Info) :
SpiralPluginGUI(w,h,o)
{	
	m_Plugin=o;
	
	m_Delay = new Fl_Slider(15, 20, 20, 70, "Delay");
	m_Delay->type(4);
	m_Delay->selection_color(GUI_COLOUR);
    m_Delay->labelsize(10);
	m_Delay->maximum(1);
    m_Delay->step(0.001);
    m_Delay->value(0.5);
    m_Delay->callback((Fl_Callback*)cb_Delay);	
	
	m_Mix = new Fl_Knob(58, 28, 45, 45, "Mix");
    m_Mix->color(GUI_COLOUR);
	m_Mix->type(Fl_Knob::DOTLIN);
    m_Mix->labelsize(10);
    m_Mix->maximum(1);
    m_Mix->step(0.01);
    m_Mix->value(0);   
	m_Mix->callback((Fl_Callback*)cb_Mix);
	
	end();
}

void DelayPluginGUI::UpdateValues()
{
	m_Delay->value(m_Plugin->GetDelay()-5.0f);
	m_Mix->value(m_Plugin->GetMix());
}

inline void DelayPluginGUI::cb_Delay_i(Fl_Slider* o, void* v) 
{ 
	float value=1.0f-o->value();
	m_Plugin->SetDelay(value); 
}
void DelayPluginGUI::cb_Delay(Fl_Slider* o, void* v) 
{ ((DelayPluginGUI*)(o->parent()))->cb_Delay_i(o,v); }

inline void DelayPluginGUI::cb_Mix_i(Fl_Knob* o, void* v) 
{ m_Plugin->SetMix(o->value()); }
void DelayPluginGUI::cb_Mix(Fl_Knob* o, void* v) 
{ ((DelayPluginGUI*)(o->parent()))->cb_Mix_i(o,v); }
