/*  SpiralPlugin
 *  Copyleft (C) 2002 Andy Preston <andy@clublinux.co.uk>
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

#include "MeterPluginGUI.h"
#include <stdio.h>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

char label_buf[10];

MeterPluginGUI::MeterPluginGUI(int w, int h, MeterPlugin *o, const HostInfo *Info) :
SpiralPluginGUI (w, h, o),
m_Bypass(0) {
   m_Plugin=o;
   Bypass = new Fl_Button (175, 2, 40, 16, "Bypass");
   Bypass -> labelsize (10);
   Bypass -> type (1);
   Bypass -> callback ((Fl_Callback*)cb_Bypass);
   add (Bypass);
   for (int display=0; display<8; display++) {
       Digits[display] = new Fl_SevenSeg ((display*28)+2, 20, 28, 40);
       Digits[display] -> bar_width (4);
       Digits[display] -> color (FL_WHITE);
       Digits[display] -> color2 (GUI_COLOUR);
       add (Digits[display]);
   }
   MinBox = new Fl_Output (2, 84, 84, 20);
   MinBox -> box (FL_ENGRAVED_BOX);
   MinBox -> color (16);
   MinBox -> set_output();
   add (MinBox);
   Reset = new Fl_Button (88, 84, 54, 20, "Reset");
   Reset -> labelsize (10);
   Reset -> type (0);
   Reset -> callback ((Fl_Callback*)cb_Reset);
   add (Reset);
   MaxBox = new Fl_Output (144, 84, 84, 20);
   MaxBox->set_output();
   MaxBox->box (FL_ENGRAVED_BOX);
   MaxBox->color (16);
   add (MaxBox);
   Meter = new Fl_VU_Meter (2, 62, 226, 20);
   Meter->color (FL_BLACK);
   cb_Reset_i (Reset, NULL);
   add (Reset);
   end();
}

void MeterPluginGUI::Display (const float *data) {
     if (! m_Bypass) {
        snprintf (label_buf, 64, "%1.5f", *data);
        if (Meter->minimum() > *data) SetMin (*data);
        if (Meter->maximum() < *data) SetMax (*data);
        Meter->value (*data);
        Meter->redraw();
        char* c = label_buf;
        for (int display=0; display<8; display++) {
            Digits[display] -> dp (off);
            if (*c == 0) Digits[display] -> value (0);
            else {
               if (*c == '.') {
                  Digits[display] -> dp (point);
                  c++;
               }
               int val;
               if (*c == '-') val = -1; else val = (int)*c - (int)'0';
               Digits[display] -> value (val);
               c++;
            }
        }
     }
}

void MeterPluginGUI::draw() {
     SpiralGUIType::draw();
     if (m_Plugin->GetInput(0) != NULL) {
	Display (m_Plugin->GetInput(0)->GetBuffer());
     }
}

void MeterPluginGUI::UpdateValues() {
}

void MeterPluginGUI::SetMax (float NewValue) {
     Meter->maximum (NewValue);
     MaxBox->value (label_buf);
}

void MeterPluginGUI::SetMin (float NewValue) {
     Meter->minimum (NewValue);
     MinBox->value (label_buf);
}

void MeterPluginGUI::cb_Bypass_i (Fl_Button* o, void* v) {
     m_Bypass = o->value();
}

void MeterPluginGUI::cb_Bypass (Fl_Button* o, void* v) {
     ((MeterPluginGUI*)(o->parent()))->cb_Bypass_i (o,v);
}

void MeterPluginGUI::cb_Reset_i (Fl_Button* o, void* v) {
     SetMin (10); // Yes, I know that LOOKS the wrong way round, but it isn't!
     SetMax (-10);
}

void MeterPluginGUI::cb_Reset (Fl_Button* o, void* v) {
     ((MeterPluginGUI*)(o->parent()))->cb_Reset_i (o,v);
}
