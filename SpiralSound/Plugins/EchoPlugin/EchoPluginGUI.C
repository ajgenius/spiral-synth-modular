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

#include "EchoPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////

EchoPluginGUI::EchoPluginGUI(int w, int h,EchoPlugin *o,const HostInfo *Info) :
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
	
	m_Feedback = new Fl_Slider(75, 20, 20, 70, "Feedback");
	m_Feedback->type(4);
	m_Feedback->selection_color(GUI_COLOUR);
    m_Feedback->labelsize(10);
	m_Feedback->maximum(1.1);
    m_Feedback->step(0.01);
    m_Feedback->value(0.4);
    m_Feedback->callback((Fl_Callback*)cb_Feedback);
		
	m_pop = new Fl_Button(1,h-14, 13, 13, "@>");
    m_pop->type(1);
    m_pop->box(FL_FLAT_BOX);
    m_pop->down_box(FL_FLAT_BOX);
    m_pop->labeltype(FL_SYMBOL_LABEL);
    m_pop->labelsize(10);
    m_pop->labelcolor(25);
    m_pop->callback((Fl_Callback*)cb_pop);
	
	m_out_delay = new Fl_Output(50, h+5, 55, 15, "Delay");
	m_out_delay->box(FL_ENGRAVED_BOX);
    m_out_delay->color(16);
    m_out_delay->labelsize(10);
    m_out_delay->textsize(10);
	m_out_delay->hide();
	m_out_delay->set_output();
	
	m_out_feedback = new Fl_Output(50, h+22, 55, 15, "Feedback");
	m_out_feedback->box(FL_ENGRAVED_BOX);
    m_out_feedback->color(16);
    m_out_feedback->labelsize(10);
    m_out_feedback->textsize(10);
	m_out_feedback->hide();
	m_out_feedback->set_output();
	
	end();
}

extern "C" int sprintf(char *,const char *,...);

void EchoPluginGUI::UpdateValues()
{
	m_Delay->value(1.0f-m_Plugin->GetDelay());
	m_Feedback->value(1.1f-m_Plugin->GetFeedback());
	char str[10];
	sprintf(str,"%5.3f s", m_Plugin->GetDelay());
	m_out_delay->value(str);
	sprintf(str,"%5.1f %%", 100*m_Plugin->GetFeedback());
	m_out_feedback->value(str);
}

inline void EchoPluginGUI::cb_Delay_i(Fl_Slider* o, void* v) 
{
char str[10]; 
float value=1.0f-o->value();
	m_Plugin->SetDelay(value);
	sprintf(str,"%5.3f s", value);
	m_out_delay->value(str);
}

void EchoPluginGUI::cb_Delay(Fl_Slider* o, void* v) 
{ ((EchoPluginGUI*)(o->parent()))->cb_Delay_i(o,v); }

inline void EchoPluginGUI::cb_Feedback_i(Fl_Slider* o, void* v) 
{
char str[10];  
	m_Plugin->SetFeedback(1.1f-o->value()); 
	sprintf(str,"%5.1f %%", 100*(1.1f-o->value()));
	m_out_feedback->value(str);
}
		
void EchoPluginGUI::cb_Feedback(Fl_Slider* o, void* v) 
{ ((EchoPluginGUI*)(o->parent()))->cb_Feedback_i(o,v); }

inline void EchoPluginGUI::cb_pop_i(Fl_Button *o, void*) {
  if (o->value())
  	{
  		o->label("@2>");
		m_out_delay->show();
		m_out_feedback->show();
		redraw();
	}
	else 
	{
		o->label("@>");
		m_out_delay->hide();
		m_out_feedback->hide();
		redraw();
		parent()->redraw();
	}
}
void EchoPluginGUI::cb_pop(Fl_Button* o, void* v) {
  ((EchoPluginGUI*)(o->parent()))->cb_pop_i(o,v);
}
