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

#include "SpiralLoopPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <FL/fl_file_chooser.h>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////

SpiralLoopPluginGUI::SpiralLoopPluginGUI(Loop *pLoop, int w, int h,SpiralLoopPlugin *o,const HostInfo *Info) :
SpiralPluginGUI(w,h,o)
{	
	m_Plugin=o;
		
	int Width=20;
	int Height=100;

	m_LoopGUI = new LoopGUI(pLoop);
	pLoop->SetGUI(m_LoopGUI);
	m_LoopGUI->CreateGUI("");
	add(m_LoopGUI->GetGUI());
	
	end();
}

void SpiralLoopPluginGUI::UpdateValues()
{
}
