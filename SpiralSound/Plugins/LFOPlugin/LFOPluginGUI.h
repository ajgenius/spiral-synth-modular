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

#ifndef LFOGUI
#define LFOGUI

#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Counter.H>
#include "../Widgets/Fl_Knob.H"
#include "../SpiralPluginGUI.h"
#include "LFOPlugin.h"

class LFOPluginGUI : public SpiralPluginGUI {
   public:
      LFOPluginGUI(int w, int h, LFOPlugin *o,const HostInfo *Info);
      virtual void UpdateValues();
      virtual SpiralPlugin* GetPlugin() { return m_Plugin; }
      LFOPlugin *m_Plugin;
   private:
      Fl_Check_Button* ShapeSine;
      Fl_Pixmap pixmap_Sine;
      Fl_Check_Button* ShapeTri;
      Fl_Pixmap pixmap_Tri;
      Fl_Check_Button* ShapeSquare;
      Fl_Pixmap pixmap_Square;
      Fl_Check_Button* ShapeSaw;
      Fl_Pixmap pixmap_Saw;
      Fl_Knob* Perd;
      Fl_Counter* NumFreq;
      Fl_Counter* NumPerd;
      inline void cb_Perd_i (Fl_Knob* o, void* v);
      static void cb_Perd (Fl_Knob* o, void* v);
      inline void cb_NumFreq_i (Fl_Knob* o, void* v);
      static void cb_NumFreq (Fl_Knob* o, void* v);
      inline void cb_NumPerd_i (Fl_Knob* o, void* v);
      static void cb_NumPerd (Fl_Knob* o, void* v);
      inline void cb_Sine_i (Fl_Check_Button* o, void* v);
      static void cb_Sine (Fl_Check_Button* o, void* v);
      inline void cb_Tri_i (Fl_Check_Button* o, void* v);
      static void cb_Tri (Fl_Check_Button* o, void* v);
      inline void cb_Square_i (Fl_Check_Button* o, void* v);
      static void cb_Square (Fl_Check_Button*, void*);
      inline void cb_Saw_i (Fl_Check_Button* o, void* v);
      static void cb_Saw (Fl_Check_Button*, void*);
};

#endif
