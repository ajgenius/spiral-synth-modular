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
	m_Data = new float[BUFSIZE];
}

ScopeWidget::~ScopeWidget()
{
	delete[] m_Data;
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
	for(int n=0; n<m_Bufsize-1 && n<w(); n++)
	{
		Value = NextValue;
		
		NextValue = m_Data[n]*ho;
			
		fl_line((int)(x()+n-2), (int)(y()+ho-Value),
				(int)(x()+n-1), (int)(y()+ho-NextValue));
	}	
	
	fl_pop_clip();
}

////////////////////////////////////////////

ScopePluginGUI::ScopePluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch),
m_Bypass(false) 
{	
    m_Scope = new ScopeWidget(5, 20, 210, 85, "Scope", Info->BUFSIZE);
 	/*Bypass = new Fl_Button(175, 107, 40, 16, "Bypass");
  	Bypass->labelsize(10);
    Bypass->type(1);
  	Bypass->callback((Fl_Callback*)cb_Bypass);*/
	end();
}

void ScopePluginGUI::Display(const float *data)
{
	//m_Scope->m_Data=data;
	if (!m_Bypass) m_Scope->redraw();
}

void ScopePluginGUI::Update()
{
	redraw();
}

void ScopePluginGUI::draw()
{
	SpiralGUIType::draw();
	const float *data;
	//cerr<<"getting and drawing..."<<endl;
	m_GUICH->GetData("AudioData",(void*)m_Scope->m_Data);
	Display(data);
}

void ScopePluginGUI::UpdateValues(SpiralPlugin* o)
{
}
	
void ScopePluginGUI::cb_Bypass_i(Fl_Button* o, void* v)
{m_Bypass=o->value();}
void ScopePluginGUI::cb_Bypass(Fl_Button* o, void* v)
{((ScopePluginGUI*)(o->parent()))->cb_Bypass_i(o,v);}

const string ScopePluginGUI::GetHelpText(const string &loc){
    return string("")
    + "The Scope lets you see a visual representation of the\n"
    + "data flowing through it. It does nothing to the signal,\n"
    + "but its very useful for checking the layouts, looking at\n"
    + "CV value etc.\n";
}
