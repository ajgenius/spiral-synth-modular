/*  SpiralPlugin
 *  Copyleft (C) 2002 Andy Preston <andy@clubunix.co.uk>
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

#include "LFOPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

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

LFOPluginGUI::LFOPluginGUI(int w, int h,LFOPlugin *o,const HostInfo *Info) :
SpiralPluginGUI(w,h,o),
pixmap_Sine(image_Sine),
pixmap_Tri(image_Tri),
pixmap_Square(image_Square),
pixmap_Saw(image_Saw) {

        m_Plugin=o;

        ShapeSine = new Fl_Check_Button (5, 15, 55, 30);
        ShapeSine->type (FL_RADIO_BUTTON);
        ShapeSine->down_box (FL_DIAMOND_DOWN_BOX);
        ShapeSine->selection_color (GUI_COLOUR);
        pixmap_Sine.label (ShapeSine);
	ShapeSine->set();
        ShapeSine->callback ((Fl_Callback*)cb_Sine);

	ShapeTri = new Fl_Check_Button (5, 46, 55, 30);
        ShapeTri->type (FL_RADIO_BUTTON);
        ShapeTri->down_box (FL_DIAMOND_DOWN_BOX);
        ShapeTri->selection_color (GUI_COLOUR);
        pixmap_Tri.label (ShapeTri);
        ShapeTri->callback ((Fl_Callback*)cb_Tri);

        ShapeSquare = new Fl_Check_Button (5, 77, 55, 30);
        ShapeSquare->type (FL_RADIO_BUTTON);
        ShapeSquare->down_box (FL_DIAMOND_DOWN_BOX);
        ShapeSquare->selection_color (GUI_COLOUR);
        pixmap_Square.label (ShapeSquare);
	ShapeSquare->callback ((Fl_Callback*)cb_Square);

        ShapeSaw = new Fl_Check_Button (5, 108, 55, 30);
        ShapeSaw->type (FL_RADIO_BUTTON);
        ShapeSaw->down_box (FL_DIAMOND_DOWN_BOX);
        ShapeSaw->selection_color (GUI_COLOUR);
        pixmap_Saw.label (ShapeSaw);
        ShapeSaw->callback ((Fl_Callback*)cb_Saw);

        Perd = new Fl_Knob (75, 15, 60, 60, "");
        Perd->color (GUI_COLOUR);
	Perd->type (Fl_Knob::LINELIN);
        Perd->labelsize (10);
        Perd->scaleticks (20);
        Perd->maximum (120);
        Perd->minimum (0.1);
        Perd->step (0.001);
        Perd->value (10);
	Perd->callback ((Fl_Callback*)cb_Perd);

        NumPerd = new Fl_Counter (50, 80, 110, 20, "Period (Secs)");
        NumPerd->textsize (10);
        NumPerd->labelsize (10);
        NumPerd->value (Perd->value());
        NumPerd->step (Perd->step());
        NumPerd->lstep (0.1);
        NumPerd->maximum (Perd->maximum());
        NumPerd->minimum (Perd->minimum());
        NumPerd->callback ((Fl_Callback*)cb_NumPerd);

        NumFreq = new Fl_Counter (50, 115, 110, 20, "Freq (Hz)");
        NumFreq->textsize (10);
        NumFreq->labelsize (10);
        NumFreq->value (1 / Perd->value());
        NumFreq->step (0.001);
        NumFreq->lstep (0.1);
        NumFreq->maximum (1 / Perd->minimum());
        NumFreq->minimum (1 / Perd->maximum());
        NumFreq->callback ((Fl_Callback*)cb_NumFreq);

        end();
}

void LFOPluginGUI::UpdateValues() {
     ShapeSine->value (0);
     ShapeTri->value (0);
     ShapeSquare->value (0);
     ShapeSaw->value (0);
     switch (m_Plugin->GetType()) {
            case LFOPlugin::SINE : ShapeSine->value (1); break;
            case LFOPlugin::TRIANGLE : ShapeTri->value (1); break;
            case LFOPlugin::SQUARE : ShapeSquare->value (1); break;
            case LFOPlugin::SAW : ShapeSaw->value (1); break;
     }
     float x = m_Plugin->GetFreq();
     NumFreq->value (x);
     x = 1 / x;
     Perd->value (x);
     NumPerd->value (x);
}

// Callbacks for knobs and counters

inline void LFOPluginGUI::cb_Perd_i (Fl_Knob* o, void* v) {
       m_Plugin->SetFreq (1 / o->value());
       UpdateValues();
}
void LFOPluginGUI::cb_Perd (Fl_Knob* o, void* v) {
     ((LFOPluginGUI*)(o->parent()))->cb_Perd_i (o, v);
}


inline void LFOPluginGUI::cb_NumPerd_i (Fl_Knob* o, void* v) {
       m_Plugin->SetFreq (1 / o->value());
       UpdateValues();
}
void LFOPluginGUI::cb_NumPerd (Fl_Knob* o, void* v) {
     ((LFOPluginGUI*)(o->parent()))->cb_NumPerd_i (o, v);
}


inline void LFOPluginGUI::cb_NumFreq_i (Fl_Knob* o, void* v) {
       m_Plugin->SetFreq (o->value());
       UpdateValues();
}

void LFOPluginGUI::cb_NumFreq (Fl_Knob* o, void* v) {
     ((LFOPluginGUI*)(o->parent()))->cb_NumFreq_i (o, v);
}

// Callbacks for waveform buttons

inline void LFOPluginGUI::cb_Sine_i (Fl_Check_Button* o, void* v) {
       m_Plugin->SetType (LFOPlugin::SINE);
}
void LFOPluginGUI::cb_Sine(Fl_Check_Button* o, void* v) {
     ((LFOPluginGUI*)(o->parent()))->cb_Sine_i (o, v);
}


inline void LFOPluginGUI::cb_Tri_i (Fl_Check_Button* o, void* v) {
       m_Plugin->SetType (LFOPlugin::TRIANGLE);
}
void LFOPluginGUI::cb_Tri (Fl_Check_Button* o, void* v) {
     ((LFOPluginGUI*)(o->parent()))->cb_Tri_i (o, v);
}


inline void LFOPluginGUI::cb_Square_i (Fl_Check_Button* o, void* v) {
       m_Plugin->SetType (LFOPlugin::SQUARE);
}
void LFOPluginGUI::cb_Square (Fl_Check_Button* o, void* v) {
     ((LFOPluginGUI*)(o->parent()))->cb_Square_i (o, v);
}


inline void LFOPluginGUI::cb_Saw_i (Fl_Check_Button* o, void* v) {
       m_Plugin->SetType (LFOPlugin::SAW);
}
void LFOPluginGUI::cb_Saw (Fl_Check_Button* o, void* v) {
     ((LFOPluginGUI*)(o->parent()))->cb_Saw_i (o, v);
}

