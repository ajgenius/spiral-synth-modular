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

#include "OscillatorPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

static unsigned char *image_Square[] = {
(unsigned char*)"20 20 2 1",
(unsigned char*)" \tc None",
(unsigned char*)".\tc #000000",
(unsigned char*)"                    ",
(unsigned char*)"                    ",
(unsigned char*)"     ......         ",
(unsigned char*)"     .    .         ",
(unsigned char*)"     .    .         ",
(unsigned char*)"     .    .         ",
(unsigned char*)"     .    .         ",
(unsigned char*)"     .    .         ",
(unsigned char*)"     .    .         ",
(unsigned char*)"     .    .         ",
(unsigned char*)"   ...    .    ...  ",
(unsigned char*)"          .    .    ",
(unsigned char*)"          .    .    ",
(unsigned char*)"          .    .    ",
(unsigned char*)"          .    .    ",
(unsigned char*)"          .    .    ",
(unsigned char*)"          .    .    ",
(unsigned char*)"          ......    ",
(unsigned char*)"                    ",
(unsigned char*)"                    "};

static unsigned char *image_Noise[] = {
(unsigned char*)"20 20 2 1",
(unsigned char*)" \tc None",
(unsigned char*)".\tc #000000",
(unsigned char*)"                    ",
(unsigned char*)"                    ",
(unsigned char*)"                    ",
(unsigned char*)"                    ",
(unsigned char*)"            .       ",
(unsigned char*)"        .   .       ",
(unsigned char*)"        .  . .      ",
(unsigned char*)"        .  . .      ",
(unsigned char*)"       . . .  .     ",
(unsigned char*)"       . . .  .     ",
(unsigned char*)"   ... . . .  . ..  ",
(unsigned char*)"     . . . .  ..    ",
(unsigned char*)"     . . . .  .     ",
(unsigned char*)"     . .  .         ",
(unsigned char*)"      .   .         ",
(unsigned char*)"      .   .         ",
(unsigned char*)"      .   .         ",
(unsigned char*)"          .         ",
(unsigned char*)"                    ",
(unsigned char*)"                    "};

static unsigned char *image_Saw[] = {
(unsigned char*)"20 20 2 1",
(unsigned char*)" \tc None",
(unsigned char*)".\tc #000000",
(unsigned char*)"                    ",
(unsigned char*)"                    ",
(unsigned char*)"         ..         ",
(unsigned char*)"         ..         ",
(unsigned char*)"        . .         ",
(unsigned char*)"        . .         ",
(unsigned char*)"       .  .         ",
(unsigned char*)"       .  .         ",
(unsigned char*)"      .   .         ",
(unsigned char*)"      .   .         ",
(unsigned char*)"   ...    .   ....  ",
(unsigned char*)"          .   .     ",
(unsigned char*)"          .  .      ",
(unsigned char*)"          .  .      ",
(unsigned char*)"          . .       ",
(unsigned char*)"          . .       ",
(unsigned char*)"          ..        ",
(unsigned char*)"          ..        ",
(unsigned char*)"                    ",
(unsigned char*)"                    "};


OscillatorPluginGUI::OscillatorPluginGUI(int w, int h,OscillatorPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch),
pixmap_Square(image_Square),
pixmap_Noise(image_Noise),
pixmap_Saw(image_Saw),
m_FineFreq(0),
m_Octave(0)
{	
    ShapeSquare = new Fl_Check_Button(5, 15, 55, 30);
    ShapeSquare->type(102);
    ShapeSquare->down_box(FL_DIAMOND_DOWN_BOX);
    ShapeSquare->selection_color(GUI_COLOUR);
    ShapeSquare->set();
    pixmap_Square.label(ShapeSquare);
	ShapeSquare->callback((Fl_Callback*)cb_Square);
	              
	ShapeNoise = new Fl_Check_Button(5, 65, 55, 30);
    ShapeNoise->type(102);
    ShapeNoise->down_box(FL_DIAMOND_DOWN_BOX);
    ShapeNoise->selection_color(GUI_COLOUR);
    pixmap_Noise.label(ShapeNoise);      
    ShapeNoise->callback((Fl_Callback*)cb_Noise);
	  
	ShapeSaw = new Fl_Check_Button(5, 40, 55, 30);
    ShapeSaw->type(102);
    ShapeSaw->down_box(FL_DIAMOND_DOWN_BOX);
    ShapeSaw->selection_color(GUI_COLOUR);
    pixmap_Saw.label(ShapeSaw);      
    ShapeSaw->callback((Fl_Callback*)cb_Saw);
	  	   
	Freq = new Fl_Knob(188, 2, 40, 40, "Octave");
    Freq->color(GUI_COLOUR);
	Freq->type(Fl_Knob::LINELIN);
    Freq->labelsize(10);
    Freq->maximum(6);
    Freq->step(1);
    Freq->value(3);   
	Freq->scaleticks(6);
	Freq->cursor(50);
	Freq->callback((Fl_Callback*)cb_Freq);
	  
    FineTune = new Fl_Knob(112, 22, 65, 65, "Fine Tune");
    FineTune->color(GUI_COLOUR);
	FineTune->type(Fl_Knob::LINELIN);
    FineTune->labelsize(10);
    FineTune->scaleticks(20);
    FineTune->maximum(1.414);
    FineTune->step(0.000001);
    FineTune->value(1);  
	FineTune->callback((Fl_Callback*)cb_FineTune);
      
	ModAmount = new Fl_Knob(188, 54, 40, 40, "Mod Depth");
    ModAmount->color(GUI_COLOUR);
	ModAmount->type(Fl_Knob::LINELIN);
	ModAmount->labelsize(10);
    ModAmount->maximum(2.0f);
    ModAmount->step(0.001);
    ModAmount->value(1.0);
    ModAmount->callback((Fl_Callback*)cb_ModAmount);
             
	PulseWidth = new Fl_Slider(60, 20, 20, 70, "PW");
	PulseWidth->type(4);
	PulseWidth->selection_color(GUI_COLOUR);
    PulseWidth->labelsize(10);
	PulseWidth->maximum(1);
    PulseWidth->step(0.01);
    PulseWidth->value(0.5);
    PulseWidth->callback((Fl_Callback*)cb_PulseWidth);
	  
	SHLen = new Fl_Slider(80, 20, 20, 70, "SH");
	SHLen->type(4);
	SHLen->selection_color(GUI_COLOUR);
    SHLen->labelsize(10);
	SHLen->maximum(0.2);
    SHLen->step(0.001);
    SHLen->value(0.1);
    SHLen->callback((Fl_Callback*)cb_SHLen);
	
	m_pop = new Fl_Button(1,h-14, 13, 13, "@>");
    m_pop->type(1);
    m_pop->box(FL_FLAT_BOX);
    m_pop->down_box(FL_FLAT_BOX);
    m_pop->labeltype(FL_SYMBOL_LABEL);
    m_pop->labelsize(10);
    m_pop->labelcolor(25);
    m_pop->callback((Fl_Callback*)cb_pop);
	
	m_out_freq = new Fl_Output(65,h+5, 58, 15, "Frequency");
    m_out_freq->box(FL_ENGRAVED_BOX);
    m_out_freq->color(16);
    m_out_freq->labelsize(10);
    m_out_freq->textsize(10);
	m_out_freq->hide();
	m_out_freq->set_output();
	
	m_out_mod = new Fl_Output(65,h+22, 48, 15, "Modulation");
    m_out_mod->box(FL_ENGRAVED_BOX);
    m_out_mod->color(16);
    m_out_mod->labelsize(10);
    m_out_mod->textsize(10);	
	m_out_mod->hide();
	m_out_mod->set_output();
	
	m_out_pulseW = new Fl_Output(190,h+5, 48, 15, "Pulse width");
    m_out_pulseW->box(FL_ENGRAVED_BOX);
    m_out_pulseW->color(16);
    m_out_pulseW->labelsize(10);
    m_out_pulseW->textsize(10);
	m_out_pulseW->hide();
	m_out_pulseW->set_output();
	
	m_out_SHlen = new Fl_Output(190,h+22, 48, 15, "Sampling");
    m_out_SHlen->box(FL_ENGRAVED_BOX);
    m_out_SHlen->color(16);
    m_out_SHlen->labelsize(10);
    m_out_SHlen->textsize(10);	  	   
	m_out_SHlen->hide();
	m_out_SHlen->set_output();
	
	end();
}

extern "C" int sprintf(char *,const char *,...);	

void OscillatorPluginGUI::UpdateValues(SpiralPlugin *o)
{
	OscillatorPlugin *Plugin = (OscillatorPlugin*)o;

	ShapeSquare->value(0);
	ShapeNoise->value(0);
	ShapeSaw->value(0);

	switch (Plugin->GetType())
	{
		case OscillatorPlugin::SQUARE : ShapeSquare->value(1); break;
		case OscillatorPlugin::NOISE  : ShapeNoise->value(1); break;
		case OscillatorPlugin::SAW    : ShapeSaw->value(1); break;
		case OscillatorPlugin::NONE   : break;
	}

	Freq->value(Plugin->GetOctave()+3);
	ModAmount->value(Plugin->GetModAmount());
	FineTune->value(sqrt(Plugin->GetFineFreq()));
	PulseWidth->value(Plugin->GetPulseWidth());
	
	SHLen->value(0.2f-Plugin->GetSHLen());
	
	char str[10];
	float fr = 110.0f * Plugin->GetFineFreq();
	m_FineFreq=Plugin->GetFineFreq();
	int oc = Plugin->GetOctave();
	m_Octave = oc;
	if (oc > 0) fr *= 1 << oc;
	if (oc < 0) fr /= 1 << (-oc);
  	sprintf(str,"%4.1f Hz", fr);
  	m_out_freq->value(str); 
	sprintf(str,"%4.0f %%", 100*Plugin->GetPulseWidth());
	m_out_pulseW->value(str); 
	sprintf(str,"%4.0f %%", 100*Plugin->GetModAmount());
	m_out_mod->value(str);
	sprintf(str,"%4.3f s", Plugin->GetSHLen());
	m_out_SHlen->value(str);
}
	

//// Callbacks ////

inline void OscillatorPluginGUI::cb_Freq_i(Fl_Knob* o, void* v) 
{
char str[10]; 
 	m_GUICH->Set("Octave",(int)o->value()-3);
	m_Octave = (int)o->value()-3;
	float fr = 110.0f * m_FineFreq;
	int oc = m_Octave;
	if (oc > 0) fr *= 1 << oc;
	if (oc < 0) fr /= 1 << (-oc);
  	sprintf(str,"%4.1f Hz", fr);
    m_out_freq->value(str); 
}
  
void OscillatorPluginGUI::cb_Freq(Fl_Knob* o, void* v) 
{ ((OscillatorPluginGUI*)(o->parent()))->cb_Freq_i(o,v); }

inline void OscillatorPluginGUI::cb_FineTune_i(Fl_Knob* o, void* v) 
{
	char str[10]; 
 	m_GUICH->Set("FineFreq",(float)(o->value()*o->value()));
	m_FineFreq=(float)(o->value()*o->value());
	float fr = 110.0f * m_FineFreq;
	int oc = m_Octave;
	if (oc > 0) fr *= 1 << oc;
	if (oc < 0) fr /= 1 << (-oc);
  	sprintf(str,"%4.1f Hz", fr);
	m_out_freq->value(str);
}
  
void OscillatorPluginGUI::cb_FineTune(Fl_Knob* o, void* v) 
{ ((OscillatorPluginGUI*)(o->parent()))->cb_FineTune_i(o,v); }

inline void OscillatorPluginGUI::cb_PulseWidth_i(Fl_Slider* o, void* v) 
{
	char str[10]; 
	m_GUICH->Set("PulseWidth",o->value());
	sprintf(str,"%4.0f %%", 100*o->value());
	m_out_pulseW->value(str); 
}

void OscillatorPluginGUI::cb_PulseWidth(Fl_Slider* o, void* v) 
{ ((OscillatorPluginGUI*)(o->parent()))->cb_PulseWidth_i(o,v);}

inline void OscillatorPluginGUI::cb_Square_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)OscillatorPlugin::SQUARE); }
void OscillatorPluginGUI::cb_Square(Fl_Check_Button* o, void* v)
{ ((OscillatorPluginGUI*)(o->parent()))->cb_Square_i(o,v); }

inline void OscillatorPluginGUI::cb_Saw_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)OscillatorPlugin::SAW); }
void OscillatorPluginGUI::cb_Saw(Fl_Check_Button* o, void* v)
{ ((OscillatorPluginGUI*)(o->parent()))->cb_Saw_i(o,v); }

inline void OscillatorPluginGUI::cb_Noise_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)OscillatorPlugin::NOISE); }
void OscillatorPluginGUI::cb_Noise(Fl_Check_Button* o, void* v)
{ ((OscillatorPluginGUI*)(o->parent()))->cb_Noise_i(o,v); }

inline void OscillatorPluginGUI::cb_SHLen_i(Fl_Slider* o, void* v)
{
	char str[10]; 
	m_GUICH->Set("SHLen",0.2f-o->value()); 
	sprintf(str,"%4.3f s", 0.2f-o->value());
	m_out_SHlen->value(str);
}

void OscillatorPluginGUI::cb_SHLen(Fl_Slider* o, void* v)
{ ((OscillatorPluginGUI*)(o->parent()))->cb_SHLen_i(o,v); }

inline void OscillatorPluginGUI::cb_ModAmount_i(Fl_Knob* o, void* v)
{
	char str[10];
	m_GUICH->Set("ModAmount",o->value());
	sprintf(str,"%4.0f %%", 100*o->value());
	m_out_mod->value(str); 
}

void OscillatorPluginGUI::cb_ModAmount(Fl_Knob* o, void* v)
{ ((OscillatorPluginGUI*)(o->parent()))->cb_ModAmount_i(o,v); }

inline void OscillatorPluginGUI::cb_pop_i(Fl_Button *o, void*) {
  if (o->value())
  	{
  		o->label("@2>");
		m_out_freq->show();
		m_out_mod->show();
		m_out_SHlen->show();
		m_out_pulseW->show();
		redraw();
	}
	else 
	{
		o->label("@>");
		m_out_freq->hide();
		m_out_mod->hide();
		m_out_SHlen->hide();
		m_out_pulseW->hide();
		redraw();
		parent()->redraw();
	}
}
void OscillatorPluginGUI::cb_pop(Fl_Button* o, void* v) {
  ((OscillatorPluginGUI*)(o->parent()))->cb_pop_i(o,v);
}

