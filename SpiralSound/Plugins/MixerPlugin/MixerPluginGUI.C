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

#include "MixerPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////

MixerPluginGUI::MixerPluginGUI(int w, int h,MixerPlugin *o,const HostInfo *Info) :
SpiralPluginGUI(w,h,o)
{	
	m_Plugin=o;
		
	int Width=20;
	int Height=100;
	
	for (int n=0; n<NUM_CHANNELS; n++)
	{
		Numbers[n]=n;
		
		m_Chan[n] = new Fl_Slider(10+(2+Width)*n, 22, Width, Height, "");
		m_Chan[n]->type(4);
		m_Chan[n]->selection_color(GUI_COLOUR);
    	m_Chan[n]->labelsize(10);
		m_Chan[n]->maximum(2);
    	m_Chan[n]->step(0.01);
    	m_Chan[n]->value(1.0);
    	m_Chan[n]->callback((Fl_Callback*)cb_Chan,(void*)&Numbers[n]);
		add(m_Chan[n]);		
	}
	
	end();
}

void MixerPluginGUI::UpdateValues()
{
	for (int n=0; n<NUM_CHANNELS; n++)
	{
		m_Chan[n]->value(2.0f-m_Plugin->GetChannel(n));
	}
}
	
inline void MixerPluginGUI::cb_Chan_i(Fl_Slider* o, void* v) 
{ m_Plugin->SetChannel(*(int*)(v),2.0f-o->value()); }
void MixerPluginGUI::cb_Chan(Fl_Slider* o, void* v) 
{ ((MixerPluginGUI*)(o->parent()))->cb_Chan_i(o,v);}
