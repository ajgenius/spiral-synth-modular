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

#include "MousePluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 154;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

ScratchWidget::ScratchWidget (int x, int y, int w, int h, const char *l, int BUFSIZE) :
Fl_Widget (x, y, w, h, l),
m_Data (0.0),
m_Channels (1),
m_Bufsize (BUFSIZE)
{
}

void ScratchWidget::draw() {
     fl_color (GUIBG_COLOUR);
     fl_rectf (x(), y(), w(), h());
     fl_push_clip (x(), y(), w(), h());
     int val = (int)(((m_Data * -1) + 1) * (h() / 2.0));
     fl_color (FL_WHITE);
     fl_rectf (x(), y() + val, w(), h());
     fl_pop_clip();
}

////////////////////////////////////////////

MousePluginGUI::MousePluginGUI (int w, int h, MousePlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch)
{
    m_Scope = new ScratchWidget (5, 20, 60, 100, "Scratch", Info->BUFSIZE);
    end();
}

void MousePluginGUI::Update () {
     m_Scope->m_Data = m_GUICH->GetFloat ("Data");
     m_Scope->redraw();
}

// Update GUI on Load
void MousePluginGUI::UpdateValues (SpiralPlugin* o) {
}

const string MousePluginGUI::GetHelpText (const string &loc) {
      return string("")
      + "\n"
      + "\n";
}
