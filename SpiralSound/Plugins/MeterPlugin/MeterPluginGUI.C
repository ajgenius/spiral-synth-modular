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

MeterPluginGUI::MeterPluginGUI (int w, int h, MeterPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch),
m_Bypass (false)
{
  // If I'm only going to use the first value from this, is it worth doing all this
  m_BufSize = Info->BUFSIZE;
  m_Data = new float[m_BufSize];
  // Create the widgets and stuff!
  Bypass = new Fl_Button (2, 18, 54, 20, "Bypass");
  Bypass -> labelsize (10);
  Bypass -> type (1);
  Bypass -> callback ((Fl_Callback*)cb_Bypass);
  add (Bypass);
  VUMode = new Fl_Check_Button (86, 18, 54, 20, "VU");
  VUMode->type (FL_RADIO_BUTTON);
  VUMode->down_box (FL_DIAMOND_DOWN_BOX);
  VUMode->selection_color (GUI_COLOUR);
  VUMode->callback ((Fl_Callback*)cb_Mode);
  VUMode->set();
  add (VUMode);
  MMMode = new Fl_Check_Button (142, 18, 54, 20, "Min/Max");
  MMMode->type (FL_RADIO_BUTTON);
  MMMode->down_box (FL_DIAMOND_DOWN_BOX);
  MMMode->selection_color (GUI_COLOUR);
  MMMode->callback ((Fl_Callback*)cb_Mode);
  add (MMMode);
  for (int display=0; display<8; display++) {
    Digits[display] = new Fl_SevenSeg ((display*28)+2, 40, 28, 40);
    Digits[display] -> bar_width (4);
    Digits[display] -> color (FL_WHITE);
    Digits[display] -> color2 (GUI_COLOUR);
    add (Digits[display]);
  }
  MinBox = new Fl_Output (2, 104, 84, 20);
  MinBox -> box (FL_ENGRAVED_BOX);
  MinBox -> color (16);
  MinBox -> set_output();
  add (MinBox);
  Reset = new Fl_Button (88, 104, 54, 20, "Reset");
  Reset -> labelsize (10);
  Reset -> type (0);
  Reset -> callback ((Fl_Callback*)cb_Reset);
  add (Reset);
  MaxBox = new Fl_Output (144, 104, 84, 20);
  MaxBox->set_output();
  MaxBox->box (FL_ENGRAVED_BOX);
  MaxBox->color (16);
  add (MaxBox);
  Meter = new Fl_VU_Meter (2, 82, 226, 20);
  Meter->color (FL_BLACK);
  cb_Reset_i (Reset, NULL);
  add (Reset);
  end ();
  DoReset ();
}

void MeterPluginGUI::draw() {
  SpiralGUIType::draw ();
  if (! m_Bypass) {
    m_GUICH->GetData ("AudioData", m_Data);
    // The min and max values are based on the whole buffer
    for (int c=0; c<m_BufSize; c++) {
      if (VUMode->value ()) m_Data[c] = fabs (m_Data[c]);
      if (m_Data[c] < m_Min) m_Min=m_Data[c];
      if (m_Data[c] > m_Max) m_Max=m_Data[c];
    }
    SetMinMax (m_Min, m_Max);
    // The meter displays the first datum in the buffer (it's a quick average)
    Meter->value (*m_Data);
    Meter->redraw();
    snprintf (label_buf, 64, "%1.5f", *m_Data);
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

void MeterPluginGUI::Update () {
  redraw();
}

void MeterPluginGUI::UpdateValues (SpiralPlugin* o) {
  MeterPlugin* Plugin = (MeterPlugin*)o;
  VUMode->value (Plugin->GetVUMode ());
  MMMode->value (! Plugin->GetVUMode ());
}

void MeterPluginGUI::SetMinMax (float NewMin, float NewMax) {
  m_Min = NewMin;
  m_Max = NewMax;
  snprintf (label_buf, 64, "%1.5f", m_Min);
  MinBox->value (label_buf);
  snprintf (label_buf, 64, "%1.5f", m_Max);
  MaxBox->value (label_buf);
  if (MMMode->value ()) {
    Meter->minimum (m_Min);
    Meter->maximum (m_Max);
  }
  else {
    Meter->minimum (0.0);
    Meter->maximum (1.0);
    if (m_Max > 1.0) {
      MaxBox->color (FL_RED);
    }
  }
}

void MeterPluginGUI::cb_Bypass_i (Fl_Button* o, void* v) {
  m_Bypass = o->value();
}

void MeterPluginGUI::cb_Bypass (Fl_Button* o, void* v) {
  ((MeterPluginGUI*)(o->parent()))->cb_Bypass_i (o, v);
}

void MeterPluginGUI::DoReset (void) {
  MaxBox->color (16);
  SetMinMax (10, -10); // Yes, I know that LOOKS the wrong way round, but it isn't!
}

void MeterPluginGUI::cb_Reset_i (Fl_Button* o, void* v) {
  DoReset ();
}

void MeterPluginGUI::cb_Reset (Fl_Button* o, void* v) {
  ((MeterPluginGUI*)(o->parent()))->cb_Reset_i (o, v);
}

void MeterPluginGUI::cb_Mode_i (Fl_Check_Button* o, void* v) {
  DoReset ();
  if (o==VUMode) m_GUICH->SetCommand (MeterPlugin::SETVU);
  else m_GUICH->SetCommand (MeterPlugin::SETMM);
}

void MeterPluginGUI::cb_Mode (Fl_Check_Button* o, void* v) {
  ((MeterPluginGUI*)(o->parent()))->cb_Mode_i (o, v);
}

const string MeterPluginGUI::GetHelpText (const string &loc) {
    return string ("")
    + "The Meter lets you see a numeric representation of the\n"
    + "data flowing through it. It does nothing to the signal,\n"
    + "but its very useful for checking the layouts, looking at\n"
    + "CV value etc.\n";
}
