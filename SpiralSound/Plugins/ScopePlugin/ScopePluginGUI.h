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
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>

#include "../Widgets/Fl_Knob.H"
#include "../Widgets/Fl_DragBar.H"
#include "ScopePlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef SCOPEGUI
#define SCOPEGUI

class ScopeWidget : public Fl_Widget 
{
public:
    ScopeWidget(int x,int y,int w,int h,const char *l, int BUFSIZE);
	~ScopeWidget();
	
    void draw(); 
	void SetNumChannels(int s) {m_Channels=s;}
	
	const float *m_Data;
	int    m_Channels;
private:

	int m_GUIColour;
	int m_GUIBGColour;
	int m_Bufsize;
};


class ScopePluginGUI : public SpiralPluginGUI
{
public:
	ScopePluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch, const HostInfo *Info);
	
	virtual void UpdateValues(SpiralPlugin* o);
	virtual void Update();	
	virtual void draw();
	void Display(const float *data);
	
private:

	bool m_Bypass;

	Fl_Button       *Bypass;
	ScopeWidget		*m_Scope;	
  
	//// Callbacks ////
	
	inline void cb_Bypass_i(Fl_Button* o, void* v);
    static void cb_Bypass(Fl_Button* o, void* v);
};

#endif
