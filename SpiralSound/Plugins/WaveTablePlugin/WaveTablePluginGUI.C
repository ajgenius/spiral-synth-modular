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

#include "WaveTablePluginGUI.h"
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

static unsigned char *image_RevSaw[] = {
(unsigned char*)"20 20 2 1",
(unsigned char*)" \tc None",
(unsigned char*)".\tc #000000",
(unsigned char*)"                    ",
(unsigned char*)"                    ",
(unsigned char*)"          ..        ",
(unsigned char*)"          ..        ",
(unsigned char*)"          . .       ",
(unsigned char*)"          . .       ",
(unsigned char*)"          .  .      ",
(unsigned char*)"          .  .      ",
(unsigned char*)"          .   .     ",
(unsigned char*)"          .   .     ",
(unsigned char*)"   ...    .   ....  ",
(unsigned char*)"      .   .         ",
(unsigned char*)"      .   .         ",
(unsigned char*)"       .  .         ",
(unsigned char*)"       .  .         ",
(unsigned char*)"        . .         ",
(unsigned char*)"        . .         ",
(unsigned char*)"         ..         ",
(unsigned char*)"                    ",
(unsigned char*)"                    "};

static unsigned char *image_Tri[] = {
(unsigned char*)"20 20 2 1",
(unsigned char*)" \tc None",
(unsigned char*)".\tc #000000",
(unsigned char*)"                    ",
(unsigned char*)"                    ",
(unsigned char*)"       .            ",
(unsigned char*)"       .            ",
(unsigned char*)"      . .           ",
(unsigned char*)"      . .           ",
(unsigned char*)"     .   .          ",
(unsigned char*)"     .   .          ",
(unsigned char*)"    .     .         ",
(unsigned char*)"    .     .         ",
(unsigned char*)"   .       .      . ",
(unsigned char*)"           .     .  ",
(unsigned char*)"            .   .   ",
(unsigned char*)"            .   .   ",
(unsigned char*)"             . .    ",
(unsigned char*)"             . .    ",
(unsigned char*)"              .     ",
(unsigned char*)"              .     ",
(unsigned char*)"                    ",
(unsigned char*)"                    "};

static unsigned char *image_Sine[] = {
(unsigned char*)"20 20 2 1",
(unsigned char*)" \tc None",
(unsigned char*)".\tc #000000",
(unsigned char*)"                    ",
(unsigned char*)"                    ",
(unsigned char*)"      ..            ",
(unsigned char*)"     .  .           ",
(unsigned char*)"    .    .          ",
(unsigned char*)"    .    .          ",
(unsigned char*)"    .    .          ",
(unsigned char*)"    .     .         ",
(unsigned char*)"   .      .         ",
(unsigned char*)"   .      .       . ",
(unsigned char*)"  .       .      .  ",
(unsigned char*)"          .      .  ",
(unsigned char*)"          .     .   ",
(unsigned char*)"          .     .   ",
(unsigned char*)"           .    .   ",
(unsigned char*)"           .    .   ",
(unsigned char*)"            .  .    ",
(unsigned char*)"             ..     ",
(unsigned char*)"                    ",
(unsigned char*)"                    "};

static unsigned char *image_Pulse1[] = {
(unsigned char*)"20 20 2 1",
(unsigned char*)" \tc None",
(unsigned char*)".\tc #000000",
(unsigned char*)"                    ",
(unsigned char*)"                    ",
(unsigned char*)"     ....           ",
(unsigned char*)"     .  .           ",
(unsigned char*)"     .  .           ",
(unsigned char*)"     .  .           ",
(unsigned char*)"     .  .           ",
(unsigned char*)"     .  .           ",
(unsigned char*)"     .  .           ",
(unsigned char*)"     .  .           ",
(unsigned char*)"   ...  .      ...  ",
(unsigned char*)"        .      .    ",
(unsigned char*)"        .      .    ",
(unsigned char*)"        .      .    ",
(unsigned char*)"        .      .    ",
(unsigned char*)"        .      .    ",
(unsigned char*)"        .      .    ",
(unsigned char*)"        ........    ",
(unsigned char*)"                    ",
(unsigned char*)"                    "};

static unsigned char *image_Pulse2[] = {
(unsigned char*)"20 20 2 1",
(unsigned char*)" \tc None",
(unsigned char*)".\tc #000000",
(unsigned char*)"                    ",
(unsigned char*)"                    ",
(unsigned char*)"     ........       ",
(unsigned char*)"     .      .       ",
(unsigned char*)"     .      .       ",
(unsigned char*)"     .      .       ",
(unsigned char*)"     .      .       ",
(unsigned char*)"     .      .       ",
(unsigned char*)"     .      .       ",
(unsigned char*)"     .      .       ",
(unsigned char*)"   ...      .  ...  ",
(unsigned char*)"            .  .    ",
(unsigned char*)"            .  .    ",
(unsigned char*)"            .  .    ",
(unsigned char*)"            .  .    ",
(unsigned char*)"            .  .    ",
(unsigned char*)"            .  .    ",
(unsigned char*)"            ....    ",
(unsigned char*)"                    ",
(unsigned char*)"                    "};

static unsigned char *image_InvSine[] = {
(unsigned char*)"20 20 2 1",
(unsigned char*)" \tc None",
(unsigned char*)".\tc #000000",
(unsigned char*)"                    ",
(unsigned char*)"                    ",
(unsigned char*)"   .      .         ",
(unsigned char*)"   .      .         ",
(unsigned char*)"   .      .         ",
(unsigned char*)"   ..    ..         ",
(unsigned char*)"   ..    ..         ",
(unsigned char*)"   . .  . .         ",
(unsigned char*)"   .  ..  .         ",
(unsigned char*)"   .      .         ",
(unsigned char*)"   .      .      .  ",
(unsigned char*)"          .      .  ",
(unsigned char*)"          .  ..  .  ",
(unsigned char*)"          . .  . .  ",
(unsigned char*)"          ..    ..  ",
(unsigned char*)"          ..    ..  ",
(unsigned char*)"          .      .  ",
(unsigned char*)"          .      .  ",
(unsigned char*)"                    ",
(unsigned char*)"                    "};

WaveTablePluginGUI::WaveTablePluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch),
pixmap_Square(image_Square),
pixmap_Saw(image_Saw),
pixmap_RevSaw(image_RevSaw),
pixmap_Tri(image_Tri),
pixmap_Sine(image_Sine),
pixmap_Pulse1(image_Pulse1),
pixmap_Pulse2(image_Pulse2),
pixmap_InvSine(image_InvSine),
m_FineFreq(0),
m_Octave(0)
{	
	ShapeSine = new Fl_Check_Button(5, 15, 55, 30);
    ShapeSine->type(102);
    ShapeSine->down_box(FL_DIAMOND_DOWN_BOX);
    ShapeSine->selection_color(GUI_COLOUR);
    pixmap_Sine.label(ShapeSine); 
	ShapeSine->set();     
    ShapeSine->callback((Fl_Callback*)cb_Sine);

    ShapeSquare = new Fl_Check_Button(5, 35, 55, 30);
    ShapeSquare->type(102);
    ShapeSquare->down_box(FL_DIAMOND_DOWN_BOX);
    ShapeSquare->selection_color(GUI_COLOUR);
    pixmap_Square.label(ShapeSquare);
	ShapeSquare->callback((Fl_Callback*)cb_Square);
	  
	ShapeSaw = new Fl_Check_Button(5, 55, 55, 30);
    ShapeSaw->type(102);
    ShapeSaw->down_box(FL_DIAMOND_DOWN_BOX);
    ShapeSaw->selection_color(GUI_COLOUR);
    pixmap_Saw.label(ShapeSaw);      
    ShapeSaw->callback((Fl_Callback*)cb_Saw);	  	  		  
		   
	ShapeRevSaw = new Fl_Check_Button(5, 75, 55, 30);
    ShapeRevSaw->type(102);
    ShapeRevSaw->down_box(FL_DIAMOND_DOWN_BOX);
    ShapeRevSaw->selection_color(GUI_COLOUR);
    pixmap_RevSaw.label(ShapeRevSaw);      
    ShapeRevSaw->callback((Fl_Callback*)cb_RevSaw);
		   
	ShapeTri = new Fl_Check_Button(55, 15, 55, 30);
    ShapeTri->type(102);
    ShapeTri->down_box(FL_DIAMOND_DOWN_BOX);
    ShapeTri->selection_color(GUI_COLOUR);
    pixmap_Tri.label(ShapeTri);      
    ShapeTri->callback((Fl_Callback*)cb_Tri);

    Pulse1 = new Fl_Check_Button(55, 35, 55, 30);
    Pulse1->type(102);
    Pulse1->down_box(FL_DIAMOND_DOWN_BOX);
    Pulse1->selection_color(GUI_COLOUR);
    pixmap_Pulse1.label(Pulse1);  
	Pulse1->callback((Fl_Callback*)cb_Pulse1);
	  
	Pulse2 = new Fl_Check_Button(55, 55, 55, 30);
    Pulse2->type(102);
    Pulse2->down_box(FL_DIAMOND_DOWN_BOX);
    Pulse2->selection_color(GUI_COLOUR);
    pixmap_Pulse2.label(Pulse2);
	Pulse2->callback((Fl_Callback*)cb_Pulse2);
	  	   
	ShapeInvSine = new Fl_Check_Button(55, 75, 55, 30);
    ShapeInvSine->type(102);
    ShapeInvSine->down_box(FL_DIAMOND_DOWN_BOX);
    ShapeInvSine->selection_color(GUI_COLOUR);
    pixmap_InvSine.label(ShapeInvSine);
	ShapeInvSine->callback((Fl_Callback*)cb_InvSine);
		   
		   
		   
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
	
	m_pop = new Fl_Button(1,h-14, 13, 13, "@>");
    m_pop->type(1);
    m_pop->box(FL_FLAT_BOX);
    m_pop->down_box(FL_FLAT_BOX);
    //m_pop->labeltype(FL_ICON_LABEL);
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
	
	m_out_mod = new Fl_Output(188,h+5, 48, 15, "Modulation");
    m_out_mod->box(FL_ENGRAVED_BOX);
    m_out_mod->color(16);
    m_out_mod->labelsize(10);
    m_out_mod->textsize(10);
	m_out_mod->hide();
	m_out_mod->set_output();
	
	end();
}

extern "C" int sprintf(char *,const char *,...);	

void WaveTablePluginGUI::UpdateValues(SpiralPlugin *o)
{
	WaveTablePlugin *Plugin = (WaveTablePlugin*)o;
	
	ShapeSquare->value(0);
	ShapeRevSaw->value(0);
	ShapeSaw->value(0);
	ShapeTri->value(0);
	ShapeSine->value(0);
	Pulse1->value(0);
	Pulse2->value(0);
	ShapeInvSine->value(0);

	switch (Plugin->GetType())
	{
		case WaveTablePlugin::SQUARE   : ShapeSquare->value(1); break;
		case WaveTablePlugin::SINE     : ShapeSine->value(1); break;
		case WaveTablePlugin::SAW      : ShapeSaw->value(1); break;
		case WaveTablePlugin::REVSAW   : ShapeRevSaw->value(1); break;
		case WaveTablePlugin::TRIANGLE : ShapeTri->value(1); break;
		case WaveTablePlugin::PULSE1   : Pulse1->value(1); break;
		case WaveTablePlugin::PULSE2   : Pulse2->value(1); break;
		case WaveTablePlugin::INVSINE  : ShapeInvSine->value(1); break;
	}

	Freq->value(Plugin->GetOctave());
	ModAmount->value(Plugin->GetModAmount());
	FineTune->value(sqrt(Plugin->GetFinefreq()));
	
	char str[10];
	float fr = 110.0f * Plugin->GetFinefreq();
	m_FineFreq=Plugin->GetFinefreq();
	int oc = Plugin->GetOctave();
	m_Octave = oc;
	if (oc > 0) fr *= 1 << oc;
	if (oc < 0) fr /= 1 << (-oc);
  	sprintf(str,"%4.1f Hz", fr);
  	m_out_freq->value(str);
	sprintf(str,"%4.0f %%", 100*Plugin->GetModAmount());
	m_out_mod->value(str);
}
	
	
//// Callbacks ////

inline void WaveTablePluginGUI::cb_Freq_i(Fl_Knob* o, void* v) 
{
	char str[10]; 
 	m_GUICH->Set("Octave",(int)o->value()-3);
	m_Octave=(int)o->value()-3;
	float fr = 110.0f * m_FineFreq;
	int oc = m_Octave;
	if (oc > 0) fr *= 1 << oc;
	if (oc < 0) fr /= 1 << (-oc);
  	sprintf(str,"%4.1f Hz", fr);
    m_out_freq->value(str);
}
void WaveTablePluginGUI::cb_Freq(Fl_Knob* o, void* v) 
{ ((WaveTablePluginGUI*)(o->parent()))->cb_Freq_i(o,v); }

inline void WaveTablePluginGUI::cb_FineTune_i(Fl_Knob* o, void* v) 
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

void WaveTablePluginGUI::cb_FineTune(Fl_Knob* o, void* v) 
{ ((WaveTablePluginGUI*)(o->parent()))->cb_FineTune_i(o,v); }

inline void WaveTablePluginGUI::cb_Square_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)WaveTablePlugin::SQUARE); }
void WaveTablePluginGUI::cb_Square(Fl_Check_Button* o, void* v)
{ ((WaveTablePluginGUI*)(o->parent()))->cb_Square_i(o,v); }

inline void WaveTablePluginGUI::cb_Saw_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)WaveTablePlugin::SAW); }
void WaveTablePluginGUI::cb_Saw(Fl_Check_Button* o, void* v)
{ ((WaveTablePluginGUI*)(o->parent()))->cb_Saw_i(o,v); }

inline void WaveTablePluginGUI::cb_Sine_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)WaveTablePlugin::SINE); }
void WaveTablePluginGUI::cb_Sine(Fl_Check_Button* o, void* v)
{ ((WaveTablePluginGUI*)(o->parent()))->cb_Sine_i(o,v); }

inline void WaveTablePluginGUI::cb_RevSaw_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)WaveTablePlugin::REVSAW); }
void WaveTablePluginGUI::cb_RevSaw(Fl_Check_Button* o, void* v)
{ ((WaveTablePluginGUI*)(o->parent()))->cb_RevSaw_i(o,v); }

inline void WaveTablePluginGUI::cb_Tri_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)WaveTablePlugin::TRIANGLE); }
void WaveTablePluginGUI::cb_Tri(Fl_Check_Button* o, void* v)
{ ((WaveTablePluginGUI*)(o->parent()))->cb_Tri_i(o,v); }

inline void WaveTablePluginGUI::cb_Pulse1_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)WaveTablePlugin::PULSE1); }
void WaveTablePluginGUI::cb_Pulse1(Fl_Check_Button* o, void* v)
{ ((WaveTablePluginGUI*)(o->parent()))->cb_Pulse1_i(o,v); }

inline void WaveTablePluginGUI::cb_Pulse2_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)WaveTablePlugin::PULSE2); }
void WaveTablePluginGUI::cb_Pulse2(Fl_Check_Button* o, void* v)
{ ((WaveTablePluginGUI*)(o->parent()))->cb_Pulse2_i(o,v); }

inline void WaveTablePluginGUI::cb_InvSine_i(Fl_Check_Button* o, void* v)
{ m_GUICH->Set("Type",(char)WaveTablePlugin::INVSINE); }
void WaveTablePluginGUI::cb_InvSine(Fl_Check_Button* o, void* v)
{ ((WaveTablePluginGUI*)(o->parent()))->cb_InvSine_i(o,v); }

inline void WaveTablePluginGUI::cb_ModAmount_i(Fl_Knob* o, void* v)
{
char str[10];
	m_GUICH->Set("ModAmount",(float)o->value());
	sprintf(str,"%4.0f %%", 100*o->value());
	m_out_mod->value(str); 
}

void WaveTablePluginGUI::cb_ModAmount(Fl_Knob* o, void* v)
{ ((WaveTablePluginGUI*)(o->parent()))->cb_ModAmount_i(o,v); }

inline void WaveTablePluginGUI::cb_pop_i(Fl_Button *o, void*) {
  if (o->value())
  	{
  		o->label("@2>");
		m_out_freq->show();
 		m_out_mod->show();
		redraw();
	}
	else 
	{
		o->label("@>");
		m_out_freq->hide();
 		m_out_mod->hide();
		redraw();
		parent()->redraw();
	}
}
void WaveTablePluginGUI::cb_pop(Fl_Button* o, void* v) {
  ((WaveTablePluginGUI*)o->parent())->cb_pop_i(o,v);
}

const string WaveTablePluginGUI::GetHelpText(const string &loc){
    return string("") 
	+ "The WaveTable plugin is a fast multifunction oscillator with a variety \n"
	+ "of wave shapes:\n"
	+ "Sine, Square, Saw, Reverse Saw, Triangle, Two pulse shapes and an inverse\n" 
	+ "sinewave.\n\n"
	+ "These wave shapes are internally represented as samples, rather than\n" 
	+ "being continually calculated like the conventional oscillator. This \n"
	+ "makes the plugin fast, but restricts the modulations you can do on the\n" 
	+ "wave forms (no pulsewidth).\n\n"
	+ "The oscillator can be pitched very low for use as a LFO CV generator,\n" 
	+ "using any of the supported wave shapes. User wave shapes are planned,\n" 
	+ "so you will be able to load your own samples in.";
}
