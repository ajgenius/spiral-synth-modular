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

#include "ScopePluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

ScopeWidget::ScopeWidget(int x,int y,int w,int h,const char *l, int BUFSIZE) :
Fl_Widget(x,y,w,h,l),
m_Data(NULL),
m_Channels(1),
m_Bufsize(BUFSIZE)
{
}


void ScopeWidget::draw()    	
{	
	int ho=h()/2;
	fl_color(GUIBG_COLOUR);
	fl_rectf(x(), y(), w(), h());
	if (!m_Data) return;
	fl_push_clip(x(), y(), w(), h());

	float Value=0,NextValue=0;

	fl_color(FL_WHITE);
	for(int n=0; n<m_Bufsize-1 && n<w(); n+=m_Channels)
	{
		Value = NextValue;
		
		NextValue = m_Data[n]*ho;
			
		fl_line(x()+n-2, y()+ho-(int)Value,
				x()+n-1, y()+ho-(int)NextValue);
	}	
	
	fl_pop_clip();
}

////////////////////////////////////////////

ScopePluginGUI::ScopePluginGUI(int w, int h,ScopePlugin *o,const HostInfo *Info) :
SpiralPluginGUI(w,h,o),
m_Bypass(false) 
{	
	m_Plugin=o;

    m_Scope = new ScopeWidget(5, 20, 210, 85, "Scope", Info->BUFSIZE);
 	Bypass = new Fl_Button(175, 107, 40, 16, "Bypass");
  	Bypass->labelsize(10);
    Bypass->type(1);
  	Bypass->callback((Fl_Callback*)cb_Bypass);
	end();
}

void ScopePluginGUI::Display(const float *data)
{
	m_Scope->m_Data=data;
	if (!m_Bypass) m_Scope->redraw();
}

void ScopePluginGUI::draw()
{
	SpiralGUIType::draw();
	
	if (m_Plugin->GetInput(0)!=NULL)
	{
		Display(m_Plugin->GetInput(0)->GetBuffer());
	}
	else
	{
		Display(NULL);
	}
}

void ScopePluginGUI::UpdateValues()
{
}
	
void ScopePluginGUI::cb_Bypass_i(Fl_Button* o, void* v)
{m_Bypass=o->value();}
void ScopePluginGUI::cb_Bypass(Fl_Button* o, void* v)
{((ScopePluginGUI*)(o->parent()))->cb_Bypass_i(o,v);}
