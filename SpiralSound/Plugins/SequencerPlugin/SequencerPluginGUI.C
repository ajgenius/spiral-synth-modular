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

#include <stdio.h>
#include "SequencerPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <FL/fl_file_chooser.h>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////

SequencerPluginGUI::PatternWin::PatternWin(int w,int h,const char* n) :
Fl_Double_Window(w,h,n)
{
	m_Scroll = new Fl_Scroll(0, 0, w, h, "");
	resizable(m_Scroll);
	add(m_Scroll);
	
	m_Melody = new Fl_EventMap(0, 0, 1000, 1000, "");
	m_Melody->SetType(Fl_EventMap::MELODY_MAP);
	m_Scroll->add(m_Melody);
	m_Melody->CreateWindow();
	m_Melody->SetUpdateLineClip(0, 18, w, h-38);
	m_Melody->show();
	
	end();
}

////////////////////////////////////////////

SequencerPluginGUI::SequencerPluginGUI(int w, int h,SequencerPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{		
	m_Scroll = new Fl_Scroll(50, 20, w-57, h-26, "");
	add(m_Scroll);
		
	m_ArrangementMap = new Fl_EventMap(0, 0, 1000, 1000, "");
	m_ArrangementMap->user_data((void*)this);
	m_ArrangementMap->SetType(Fl_EventMap::ARRANGE_MAP);
	m_Scroll->add(m_ArrangementMap);
	m_ArrangementMap->CreateWindow();
	m_ArrangementMap->SetUpdateLineClip(0, 18, w, h-38);
	m_ArrangementMap->show();
		
	m_Length = new Fl_Knob(5, 60, 40, 40, "Length");
        m_Length->color(GUI_COLOUR);
	m_Length->type(Fl_Knob::DOTLIN);
        m_Length->labelsize(10);
        m_Length->maximum(30);
        m_Length->step(0.01);
        m_Length->value(1.0);
	m_Length->callback((Fl_Callback*)cb_Length);
	add(m_Length);
	
	m_Speed = new Fl_Knob(5, 115, 40, 40, "Speed");
    m_Speed->color(GUI_COLOUR);
	m_Speed->type(Fl_Knob::DOTLIN);
    m_Speed->labelsize(10);
    m_Speed->maximum(4);
    m_Speed->step(0.01);
    m_Speed->value(2.0);   
	m_Speed->callback((Fl_Callback*)cb_Speed);
	add(m_Speed);
	
	m_Zoom = new Fl_Knob(5,170,40,40,"Zoom");
	m_Zoom->color(GUI_COLOUR);
	m_Zoom->type(Fl_Knob::DOTLIN);
    m_Zoom->labelsize(10);
    m_Zoom->maximum(2);
    m_Zoom->step(0.01);
    m_Zoom->value(1.0);   
	m_Zoom->callback((Fl_Callback*)cb_Zoom);
	add(m_Zoom);
	
	m_NewPattern = new Fl_Button(2,225,45,20,"New Pat");
	m_NewPattern->type(0);
	m_NewPattern->labelsize(10);
	m_NewPattern->callback((Fl_Callback*)cb_NewPattern,NULL);
	add(m_NewPattern);
	
	m_NoteCut = new Fl_Button(2,245,45,20,"NoteCut");
	m_NoteCut->type(1);
	m_NoteCut->labelsize(10);
	m_NoteCut->value(0);
	m_NoteCut->callback((Fl_Callback*)cb_NoteCut);
	add(m_NoteCut);
	
	m_Clear = new Fl_Button(2,265,45,20,"Clear");
	m_Clear->type(0);
	m_Clear->labelsize(10);
	m_Clear->value(0);
	m_Clear->callback((Fl_Callback*)cb_Clear);
	add(m_Clear);
	
	end();
}

void SequencerPluginGUI::UpdateValues(SpiralPlugin *o)
{
}
	
inline void SequencerPluginGUI::cb_NoteCut_i(Fl_Button* o, void* v)
{ 
	//m_Plugin->SetNoteCut(o->value());
}
void SequencerPluginGUI::cb_NoteCut(Fl_Button* o, void* v)
{ ((SequencerPluginGUI*)(o->parent()))->cb_NoteCut_i(o,v);}

inline void SequencerPluginGUI::cb_Zoom_i(Fl_Knob* o, void* v)
{
	//m_Plugin->SetZoom(o->value()); 
}
void SequencerPluginGUI::cb_Zoom(Fl_Knob* o, void* v)
{ ((SequencerPluginGUI*)(o->parent()))->cb_Zoom_i(o,v);}



inline void SequencerPluginGUI::cb_Pattern_i(Fl_Counter* o, void* v)
{ 	
	if (o->value()<0) o->value(0);
	if (o->value()>NUM_PATTERNS-1) o->value(NUM_PATTERNS-1);
	//m_Plugin->SetPattern((int)o->value());
	//UpdateValues();
}
void SequencerPluginGUI::cb_Pattern(Fl_Counter* o, void* v)
{ ((SequencerPluginGUI*)(o->parent()))->cb_Pattern_i(o,v);}

inline void SequencerPluginGUI::cb_Length_i(Fl_Knob* o, void* v)
{
	//m_Plugin->SetEndTime(o->value());
}
void SequencerPluginGUI::cb_Length(Fl_Knob* o, void* v)
{ ((SequencerPluginGUI*)(o->parent()))->cb_Length_i(o,v);}

inline void SequencerPluginGUI::cb_Speed_i(Fl_Knob* o, void* v)
{ 	
	//m_Plugin->SetSpeed(o->value()-2.0f);
}
void SequencerPluginGUI::cb_Speed(Fl_Knob* o, void* v)
{ ((SequencerPluginGUI*)(o->parent()))->cb_Speed_i(o,v);}

inline void SequencerPluginGUI::cb_Clear_i(Fl_Button* o, void* v)
{ 	
	//m_Plugin->ClearAll();
}
void SequencerPluginGUI::cb_Clear(Fl_Button* o, void* v)
{ ((SequencerPluginGUI*)(o->parent()))->cb_Clear_i(o,v);}

inline void SequencerPluginGUI::cb_NewPattern_i(Fl_Button* o, void* v)
{
	int eid = m_ArrangementMap->AddEvent(300,100,100,Fl_SEvent::NO_TYPE);
	m_ArrangementMap->GetEvent(eid)->SetName("My Pattern");
	const char *name = fl_input("Name the new pattern:", "My Pattern");
	if (name) m_ArrangementMap->GetEvent(eid)->SetName(name);
	m_PatternWinMap[eid] = new PatternWin(400,200,m_ArrangementMap->GetEvent(eid)->GetName().c_str());
	
	m_ArrangementMap->SetEventCallback((Fl_Callback*)cb_ArrangeRM);
	redraw();
}
void SequencerPluginGUI::cb_NewPattern(Fl_Button* o, void* v)
{ ((SequencerPluginGUI*)(o->parent()))->cb_NewPattern_i(o,v);}

inline void SequencerPluginGUI::cb_ArrangeRM_i(Fl_Button* o, void* v)
{ 	
	int ID=((Fl_SEvent*)o)->GetID();
		
	if (m_PatternWinMap[ID]->shown()) m_PatternWinMap[ID]->hide();
	else m_PatternWinMap[ID]->show();
}
void SequencerPluginGUI::cb_ArrangeRM(Fl_Button* o, void* v)
{ ((SequencerPluginGUI*)(o->parent()->user_data()))->cb_ArrangeRM_i(o,v);}

