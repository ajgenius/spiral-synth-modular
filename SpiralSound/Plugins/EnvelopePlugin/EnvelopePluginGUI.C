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

#include "EnvelopePluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;
static const float TIMED_SLIDER_MAX = 3.0f;

////////////////////////////////////////////

EnvelopePluginGUI::EnvelopePluginGUI(int w, int h,EnvelopePlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{		
	Thresh = new Fl_Slider(10, 20, 20, 70, "T");
	Thresh->type(4);
	Thresh->selection_color(GUI_COLOUR);
    Thresh->labelsize(10);
	Thresh->maximum(1.0);
    Thresh->step(0.01);
    Thresh->value(0.98f);
    Thresh->callback((Fl_Callback*)cb_Thresh);
	add(Thresh);
	
	Attack = new Fl_Slider(30, 20, 20, 70, "A");
	Attack->type(4);
	Attack->selection_color(GUI_COLOUR);
    Attack->labelsize(10);
	Attack->maximum(TIMED_SLIDER_MAX);
    Attack->step(0.01);
    Attack->value(1.0f);
    Attack->callback((Fl_Callback*)cb_Attack);
	add(Attack);
	
	Decay = new Fl_Slider(50, 20, 20, 70, "D");
	Decay->type(4);
	Decay->selection_color(GUI_COLOUR);
    Decay->labelsize(10);
	Decay->maximum(TIMED_SLIDER_MAX);
    Decay->step(0.01);
    Decay->value(0.1);
    Decay->callback((Fl_Callback*)cb_Decay);
	add(Decay);
	
	Sustain = new Fl_Slider(70, 20, 20, 70, "S");
	Sustain->type(4);	  
	Sustain->selection_color(GUI_COLOUR);
    Sustain->labelsize(10);
	Sustain->maximum(1);
    Sustain->step(0.01);
    Sustain->value(0.5);
    Sustain->callback((Fl_Callback*)cb_Sustain);
	add(Sustain);
		 
	Release = new Fl_Slider(90, 20, 20, 70, "R");
	Release->type(4);
	Release->selection_color(GUI_COLOUR);
    Release->labelsize(10);
	Release->maximum(TIMED_SLIDER_MAX);
    Release->step(0.01);
    Release->value(0.5);
    Release->callback((Fl_Callback*)cb_Release);	  
	add(Release);
	      
	Volume = new Fl_Slider(110, 20, 20, 70, "V");
	Volume->type(4);
	Volume->selection_color(GUI_COLOUR);
    Volume->labelsize(10);
	Volume->maximum(1);
    Volume->step(0.01);
    Volume->value(0.5f);
    Volume->callback((Fl_Callback*)cb_Volume);
	add(Volume);
		
	m_pop = new Fl_Button(1,h-14, 13, 13, "@>");
    m_pop->type(1);
    m_pop->box(FL_FLAT_BOX);
    m_pop->down_box(FL_FLAT_BOX);
    m_pop->labeltype(FL_SYMBOL_LABEL);
    m_pop->labelsize(10);
    m_pop->labelcolor(25);
    m_pop->callback((Fl_Callback*)cb_pop);
	
	m_out_thresh = new Fl_Output(15,h+5, 48, 15, "T");
    m_out_thresh->box(FL_ENGRAVED_BOX);
    m_out_thresh->color(16);
    m_out_thresh->labelsize(10);
    m_out_thresh->textsize(10);
	m_out_thresh->hide();
	m_out_thresh->set_output();
	
	m_out_attack = new Fl_Output(15,h+22, 48, 15, "A");
    m_out_attack->box(FL_ENGRAVED_BOX);
    m_out_attack->color(16);
    m_out_attack->labelsize(10);
    m_out_attack->textsize(10);
	m_out_attack->hide();
	m_out_attack->set_output();
	
	m_out_decay = new Fl_Output(15,h+39, 48, 15, "D");
    m_out_decay->box(FL_ENGRAVED_BOX);
    m_out_decay->color(16);
    m_out_decay->labelsize(10);
    m_out_decay->textsize(10);	
	m_out_decay->hide();
	m_out_decay->set_output();
	
	m_out_sustain = new Fl_Output(87,h+5, 48, 15, "S");
    m_out_sustain->box(FL_ENGRAVED_BOX);
    m_out_sustain->color(16);
    m_out_sustain->labelsize(10);
    m_out_sustain->textsize(10);
	m_out_sustain->hide();
	m_out_sustain->set_output();
	
	m_out_release = new Fl_Output(87,h+22, 48, 15, "R");
    m_out_release->box(FL_ENGRAVED_BOX);
    m_out_release->color(16);
    m_out_release->labelsize(10);
    m_out_release->textsize(10);
	m_out_release->hide();
	m_out_release->set_output();
	
	m_out_volume = new Fl_Output(87,h+39, 48, 15, "V");
    m_out_volume->box(FL_ENGRAVED_BOX);
    m_out_volume->color(16);
    m_out_volume->labelsize(10);
    m_out_volume->textsize(10);
	m_out_volume->hide();
	m_out_volume->set_output();
	
	end();
}

extern "C" int sprintf(char *,const char *,...);

void EnvelopePluginGUI::UpdateValues(SpiralPlugin *o)
{
	EnvelopePlugin *Plugin = (EnvelopePlugin*)o;

	Thresh->value(1.0f-sqrt(Plugin->GetAttack()));
	Attack->value(TIMED_SLIDER_MAX-sqrt(Plugin->GetAttack()));
	Decay->value(TIMED_SLIDER_MAX-sqrt(Plugin->GetDecay()));
	Sustain->value(1.0f-Plugin->GetSustain());
	Release->value(TIMED_SLIDER_MAX-sqrt(Plugin->GetRelease()));
	Volume->value(1.0f-Plugin->GetVolume());
	char str[10];
	sprintf(str,"%4.0f %%", 100*Plugin->GetTrigThresh());
	m_out_thresh->value(str);
	sprintf(str,"%5.3f s", Plugin->GetAttack());
	m_out_attack->value(str); 
	sprintf(str,"%5.3f s", Plugin->GetDecay());
	m_out_decay->value(str); 
	sprintf(str,"%4.0f %%", 100*Plugin->GetSustain());
	m_out_sustain->value(str); 
	sprintf(str,"%5.3f s", Plugin->GetRelease());
	m_out_release->value(str); 
	sprintf(str,"%4.0f %%", 100*Plugin->GetVolume());
	m_out_volume->value(str); 
}


inline void EnvelopePluginGUI::cb_Thresh_i(Fl_Slider* o, void* v) 
{ 
char str[10];
	m_GUICH->Set("Trig",1.0f-o->value()); 
	sprintf(str,"%4.0f %%", 100*(1.0f-o->value()));
	m_out_thresh->value(str); 
}

void EnvelopePluginGUI::cb_Thresh(Fl_Slider* o, void* v) 
{ ((EnvelopePluginGUI*)(o->parent()))->cb_Thresh_i(o,v); }
	
inline void EnvelopePluginGUI::cb_Attack_i(Fl_Slider* o, void* v) 
{ 
char str[10];
	float value=TIMED_SLIDER_MAX-o->value();
	m_GUICH->Set("Attack",value*value); 
	sprintf(str,"%5.3f s", value*value);
	m_out_attack->value(str);
}
void EnvelopePluginGUI::cb_Attack(Fl_Slider* o, void* v) 
{ ((EnvelopePluginGUI*)(o->parent()))->cb_Attack_i(o,v); }

inline void EnvelopePluginGUI::cb_Decay_i(Fl_Slider* o, void* v) 
{ 
char str[10];
	float value=TIMED_SLIDER_MAX-o->value();
	m_GUICH->Set("Decay",value*value);  
	sprintf(str,"%5.3f s", value*value);
	m_out_decay->value(str);
}
void EnvelopePluginGUI::cb_Decay(Fl_Slider* o, void* v) 
{ ((EnvelopePluginGUI*)(o->parent()))->cb_Decay_i(o,v); }

inline void EnvelopePluginGUI::cb_Sustain_i(Fl_Slider* o, void* v) 
{
char str[10];
	m_GUICH->Set("Sustain",1.0f-o->value());
	sprintf(str,"%4.0f %%", 100*(1.0f-o->value()));
	m_out_sustain->value(str); 
}
		
void EnvelopePluginGUI::cb_Sustain(Fl_Slider* o, void* v) 
{ ((EnvelopePluginGUI*)(o->parent()))->cb_Sustain_i(o,v); }

inline void EnvelopePluginGUI::cb_Release_i(Fl_Slider* o, void* v) 
{ 
char str[10];
	float value=TIMED_SLIDER_MAX-o->value();
	m_GUICH->Set("Release",value*value); 
	sprintf(str,"%5.3f s", value*value);
		m_out_release->value(str);
}
void EnvelopePluginGUI::cb_Release(Fl_Slider* o, void* v) 
{ ((EnvelopePluginGUI*)(o->parent()))->cb_Release_i(o,v); }

inline void EnvelopePluginGUI::cb_Volume_i(Fl_Slider* o, void* v) 
{
char str[10];

	m_GUICH->Set("Volume",1.0f-o->value());
	sprintf(str,"%4.0f %%", 100*(1.0f-o->value()));
	m_out_volume->value(str); 
}

void EnvelopePluginGUI::cb_Volume(Fl_Slider* o, void* v) 
{ ((EnvelopePluginGUI*)(o->parent()))->cb_Volume_i(o,v); }


inline void EnvelopePluginGUI::cb_pop_i(Fl_Button *o, void*) {
  if (o->value())
  	{
  		o->label("@2>");
		m_out_thresh->show();
		m_out_attack->show();
		m_out_decay->show();
		m_out_sustain->show();
		m_out_release->show();
		redraw();
	}
	else 
	{
		o->label("@>");
		m_out_thresh->hide();
		m_out_attack->hide();
		m_out_decay->hide();
		m_out_sustain->hide();
		m_out_release->hide();
		redraw();
		parent()->redraw();
	}
}
void EnvelopePluginGUI::cb_pop(Fl_Button* o, void* v) {
  ((EnvelopePluginGUI*)(o->parent()))->cb_pop_i(o,v);
}

