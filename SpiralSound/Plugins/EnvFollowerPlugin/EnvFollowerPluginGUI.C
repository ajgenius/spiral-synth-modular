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

#include "EnvFollowerPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////

EnvFollowerPluginGUI::EnvFollowerPluginGUI(int w, int h,EnvFollowerPlugin *o,const HostInfo *Info) :
SpiralPluginGUI(w,h,o)
{	
	m_Plugin=o;
	
	m_Attack = new Fl_Knob(20, 25, 40, 40, "Attack");	
	m_Attack->color(GUI_COLOUR);
	m_Attack->type(Fl_Knob::DOTLIN);
   	m_Attack->labelsize(10);
   	m_Attack->maximum(1);
   	m_Attack->step(0.001);
   	m_Attack->value(0.5);   
	m_Attack->callback((Fl_Callback*)cb_Attack);
	add(m_Attack);
	
	m_Decay = new Fl_Knob(75, 25, 40, 40, "Decay");	
	m_Decay->color(GUI_COLOUR);
	m_Decay->type(Fl_Knob::DOTLIN);
   	m_Decay->labelsize(10);
   	m_Decay->maximum(1);
   	m_Decay->step(0.001);
   	m_Decay->value(0.5);   
	m_Decay->callback((Fl_Callback*)cb_Decay);
	add(m_Decay);
	
	end();
}



void EnvFollowerPluginGUI::UpdateValues()
{
	m_Attack->value(m_Plugin->GetAttack());
	m_Decay->value(m_Plugin->GetDecay());
}
	
inline void EnvFollowerPluginGUI::cb_Decay_i(Fl_Knob* o, void* v)
{ m_Plugin->SetDecay(o->value()); }
void EnvFollowerPluginGUI::cb_Decay(Fl_Knob* o, void* v)
{ ((EnvFollowerPluginGUI*)(o->parent()))->cb_Decay_i(o,v);}

inline void EnvFollowerPluginGUI::cb_Attack_i(Fl_Knob* o, void* v)
{ m_Plugin->SetAttack(o->value()); }
void EnvFollowerPluginGUI::cb_Attack(Fl_Knob* o, void* v)
{ ((EnvFollowerPluginGUI*)(o->parent()))->cb_Attack_i(o,v);}
