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



#include "SplitSwitchPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <stdio.h>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////

SplitSwitchPluginGUI::SplitSwitchPluginGUI (int w, int h, SplitSwitchPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch)
{
  m_Chans = new Fl_Counter (5, 20, 40, 20, "Channels");
  m_Chans->labelsize (10);
  m_Chans->type (FL_SIMPLE_COUNTER);
  m_Chans->step (1);
  m_Chans->value (2);
  m_Chans->callback ((Fl_Callback*) cb_Chans);
  add (m_Chans);

  m_Switch = new Fl_Counter (49, 20, 40, 20, "Select");
  m_Switch->labelsize (10);
  m_Switch->type (FL_SIMPLE_COUNTER);
  m_Switch->step (1);
  m_Switch->value (1);
  m_Switch->callback ((Fl_Callback*) cb_Switch);
  add (m_Switch);
}

inline void SplitSwitchPluginGUI::cb_Chans_i (Fl_Counter* o, void* v) {
  if (o->value() < 2) o->value(2);
  else {
    m_GUICH->Set ("Chans", int (o->value ()));
    m_GUICH->SetCommand (SplitSwitchPlugin::SETCHANS);
  }
}

void SplitSwitchPluginGUI::cb_Chans (Fl_Counter* o, void* v) {
  ((SplitSwitchPluginGUI*) (o->parent ())) -> cb_Chans_i (o, v);
}


inline void SplitSwitchPluginGUI::cb_Switch_i (Fl_Counter* o, void* v) {
  if (o->value()<1 || o->value() > m_Chans->value ())
    o->value (1);
  m_GUICH->Set ("Switch", int (o->value ()));
}

void SplitSwitchPluginGUI::cb_Switch (Fl_Counter* o, void* v) {
  ((SplitSwitchPluginGUI*) (o->parent ())) -> cb_Switch_i (o, v);
}

void SplitSwitchPluginGUI::Update () {
  m_Switch->value (m_GUICH->GetInt ("Echo"));
}

void SplitSwitchPluginGUI::UpdateValues (SpiralPlugin *o) {
  SplitSwitchPlugin* Plugin = (SplitSwitchPlugin*)o;
  m_Chans->value (Plugin->GetChans ());
  m_Switch->value (Plugin->GetSwitch ());
}

const string SplitSwitchPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "It's a simple 1-input n-output rotary switch.\n\n"
    + "The 'Channels' parameter controls the number of outputs.\n\n"
    + "The 'Select' parameter selects which output echos the input.\n\n"
    + "The 'CV' input also selects which output echos the input.\n\n"
    + "The 'Clock' input is a trigger to select the next output chanel.\n\n"
    + "The 'CV' output enables you to gang several switches together, by\n"
    + "connecting it to the other switches' 'CV' input.\n";
}
