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

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <iostream>
#include <math.h>

#include "Widgets/Fl_DragBar.H"
#include "SpiralPlugin.h" // for the channel handler

#ifndef SPIRALPLUGINGUI
#define SPIRALPLUGINGUI

#define SpiralGUIType Fl_Group
 
class SpiralPluginGUI : public SpiralGUIType
{
public:
	SpiralPluginGUI(int w, int h, SpiralPlugin* o, ChannelHandler *ch);
	~SpiralPluginGUI();
	
	virtual void Update();
	
	// called while audio thread is suspended, so direct access to the
	// spiralplugin is acceptable
	virtual void UpdateValues(SpiralPlugin *o)=0;

protected:

	ChannelHandler *m_GUICH;
		
private:
	Fl_DragBar*      m_DragBar;
	Fl_Button*		 m_Hide;
	
	//// Callbacks ////
	inline void cb_Hide_i(Fl_Button* o, void* v);
    static void cb_Hide(Fl_Button*, void*);
};

#endif
