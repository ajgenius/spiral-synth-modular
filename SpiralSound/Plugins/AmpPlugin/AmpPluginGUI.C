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

#include "AmpPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////

AmpPluginGUI::AmpPluginGUI(int w, int h,AmpPlugin *o,const HostInfo *Info) :
SpiralPluginGUI(w,h,o)
{	
	m_Plugin=o;
	
	m_Amp = new Fl_Slider(15, 20, 20, 70, "Amp");
	m_Amp->type(4);
	m_Amp->selection_color(GUI_COLOUR);
    m_Amp->labelsize(10);
	m_Amp->maximum(4);
    m_Amp->step(0.0001);
    m_Amp->value(2);
    m_Amp->callback((Fl_Callback*)cb_Amp);
	
	m_DC = new Fl_Slider(70, 20, 20, 70, "DC Offset");
	m_DC->type(4);
	m_DC->selection_color(GUI_COLOUR);
    m_DC->labelsize(10);
	m_DC->maximum(4);
    m_DC->step(0.0001);
    m_DC->value(2);
    m_DC->callback((Fl_Callback*)cb_DC);	
				
	m_pop = new Fl_Button(1,h-14, 13, 13, "@>");
    m_pop->type(1);
    m_pop->box(FL_FLAT_BOX);
    m_pop->down_box(FL_FLAT_BOX);
    m_pop->labeltype(FL_SYMBOL_LABEL);
    m_pop->labelsize(10);
    m_pop->labelcolor(25);
    m_pop->callback((Fl_Callback*)cb_pop);
	
	m_out_gain = new Fl_Output(25,h+5,36, 15, "Gain");
    m_out_gain->box(FL_ENGRAVED_BOX);
    m_out_gain->color(16);
    m_out_gain->labelsize(10);
    m_out_gain->textsize(10);
	m_out_gain->hide();
	m_out_gain->set_output();
	
	m_out_DC = new Fl_Output(82,h+5,36, 15, "DC");
    m_out_DC->box(FL_ENGRAVED_BOX);
    m_out_DC->color(16);
    m_out_DC->labelsize(10);
    m_out_DC->textsize(10);
	m_out_DC->hide();
	m_out_DC->set_output();
	
	end();
}

extern "C" int sprintf(char *,const char *,...);	

void AmpPluginGUI::UpdateValues()
{
	m_Amp->value(2.0f-m_Plugin->GetAmp());
	m_DC->value(2.0f-m_Plugin->GetDC());
	char str[10];
  	sprintf(str,"%4.2f",m_Plugin->GetAmp());
  	m_out_gain->value(str);
	sprintf(str,"%4.2f",m_Plugin->GetDC());
	m_out_DC->value(str);
}

inline void AmpPluginGUI::cb_Amp_i(Fl_Slider* o, void* v) 
{ 

char str[10];
	float value=2.0f-o->value();
	m_Plugin->SetAmp(value); 
	sprintf(str,"%4.2f",value);
	m_out_gain->value(str); 
}
void AmpPluginGUI::cb_Amp(Fl_Slider* o, void* v) 
{ ((AmpPluginGUI*)(o->parent()))->cb_Amp_i(o,v); }

inline void AmpPluginGUI::cb_DC_i(Fl_Slider* o, void* v) 
{ 
char str[10];
	float value=2.0f-o->value();
	m_Plugin->SetDC(value);
	sprintf(str,"%4.2f",value);
	m_out_DC->value(str); 
}
void AmpPluginGUI::cb_DC(Fl_Slider* o, void* v) 
{ ((AmpPluginGUI*)(o->parent()))->cb_DC_i(o,v); }

inline void AmpPluginGUI::cb_pop_i(Fl_Button *o, void*) {
  if (o->value())
  	{
  		o->label("@2>");
		m_out_gain->show();
		m_out_DC->show();
		redraw();
	}
	else 
	{
		o->label("@>");
		m_out_gain->hide();
		m_out_DC->hide();
		redraw();
		parent()->redraw();
	}
}
void AmpPluginGUI::cb_pop(Fl_Button* o, void* v) {
  ((AmpPluginGUI*)(o->parent()))->cb_pop_i(o,v);
}

