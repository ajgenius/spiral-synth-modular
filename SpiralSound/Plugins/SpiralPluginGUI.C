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

#include "SpiralPluginGUI.h"
#include "SpiralPlugin.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 154;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

SpiralPluginGUI::SpiralPluginGUI(int w, int h, SpiralPlugin* o, ChannelHandler *ch) :
SpiralGUIType(0,0,w,h,"")
{	
	Fl::visible_focus(false);
	
	m_GUICH = ch;
	
	box(FL_THIN_UP_BOX);
	m_DragBar = new Fl_DragBar(0,0,w,20,o->GetName().c_str());
	m_DragBar->type(Fl_DragBar::FLDRAG);
	add(m_DragBar);
  	
  	m_Hide = new Fl_Button(2,2,18,16,"X");
	m_Hide->labeltype(FL_ENGRAVED_LABEL);
	m_Hide->box(FL_NO_BOX);
	m_Hide->callback((Fl_Callback*)cb_Hide);
	add(m_Hide);
}

SpiralPluginGUI::~SpiralPluginGUI()
{
	// Needed to properly remove the window.
	Fl::check();
}

void SpiralPluginGUI::Update()
{
}

//// Callbacks ////

inline void SpiralPluginGUI::cb_Hide_i(Fl_Button* o, void* v) 
{ hide(); }
void SpiralPluginGUI::cb_Hide(Fl_Button* o, void* v) 
{ ((SpiralPluginGUI*)(o->parent()))->cb_Hide_i(o,v); }


