/*  MousePlugin
 *  Copyleft (C) 2002 Dan Bethell <dan@pawfal.org>
 *                    Dave Griffiths <dave@pawfal.org>
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

#ifndef MouseGUI_H
#define MouseGUI_H

#include <FL/Fl.H>
#include "MousePlugin.h"
#include "../SpiralPluginGUI.h"

class ScratchWidget : public Fl_Widget {
   public:
      ScratchWidget (int x, int y, int w, int h, const char *l, int BUFSIZE);
      void draw();
      void SetNumChannels (int s) { m_Channels = s; }
      float m_Data;
      int m_Channels;
   private:
      int m_GUIColour;
      int m_GUIBGColour;
      int m_Bufsize;
};

class MousePluginGUI : public SpiralPluginGUI {
   public:
      MousePluginGUI (int w, int h, MousePlugin *o, ChannelHandler *ch, const HostInfo *Info);
      virtual void UpdateValues (SpiralPlugin *o);
      virtual void Update ();
   protected:
      const string GetHelpText (const string &loc);
   private:
//      virtual void draw();
//      void Display(float data);
//      virtual SpiralPlugin* GetPlugin() { return m_Plugin; }
//      MousePlugin *m_Plugin;
      ScratchWidget *m_Scope;
};

#endif
