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
#include "ControllerPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////

ControllerPluginGUI::CVGUI::CVGUI(int n, ControllerPluginGUI *p)
{
	m_SliderGroup = new Fl_Group(0,0,60,153,"");
	m_SliderGroup->box(FL_UP_BOX);
	m_SliderGroup->user_data((void *)p);
		
	m_Title = new Fl_Input(5,2,50,15,"");
	m_Title->value("Name");
	m_Title->textsize(10);
	m_SliderGroup->add(m_Title);
		
	m_Max = new Fl_Int_Input(5,18,50,15,"");
	char t[64];
	sprintf(t,"%d",1);
	m_Max->value(t);
	m_Max->textsize(10);
	m_SliderGroup->add(m_Max);
		
	m_Chan = new Fl_Slider(20, 34, 20, 100, "");
	m_Chan->type(4);
	m_Chan->selection_color(GUI_COLOUR);
	m_Chan->maximum(1);
   	m_Chan->step(0.01);
   	m_Chan->value(0.5);
		
   	m_Chan->callback((Fl_Callback*)ControllerPluginGUI::cb_Chan,
		(void*)&Numbers[n]);
	m_SliderGroup->add(m_Chan);
	
	m_Min = new Fl_Int_Input(5,136,50,15,"");
	char t2[64];
	sprintf(t2,"%d",-1);
	m_Min->value(t2);
	m_Min->textsize(10);
	m_SliderGroup->add(m_Min);
}

////////////////////////////////////////////

ControllerPluginGUI::ControllerPluginGUI(int w, int h,ControllerPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch),
m_CVCount(0)
{	
	for (int n=0; n<MAX_CHANNELS; n++)
	{
		Numbers[n]=n;
	}
	
	m_MainPack = new Fl_Pack(0,20,w,h-44);
	m_MainPack->type(FL_HORIZONTAL);
	
	// start with four...
	AddCV();
	AddCV();
	AddCV();
	AddCV();
	
	m_Delete = new Fl_Button(2,h-22,20,20,"-");
	m_Delete->user_data(this);
	m_Delete->callback((Fl_Callback*)cb_Delete);
	add(m_Delete);
	m_Add = new Fl_Button(24,h-22,20,20,"+");
	m_Add->user_data(this);
	m_Add->callback((Fl_Callback*)cb_Add);
	add(m_Add);	
}

void ControllerPluginGUI::AddCV()
{
	CVGUI *NewCV = new CVGUI(m_CVCount,this);
	m_GuiVec.push_back(NewCV);
	m_MainPack->add(NewCV->m_SliderGroup);
	m_CVCount++;
}

void ControllerPluginGUI::DeleteCV()
{
	vector<CVGUI*>::iterator i=m_GuiVec.end();
	i--;
	m_MainPack->remove((*i)->m_SliderGroup);
	delete *i;
	m_GuiVec.erase(i);
	m_CVCount--;
}

void ControllerPluginGUI::Clear()
{
	for (vector<ControllerPluginGUI::CVGUI*>::iterator i=m_GuiVec.begin();
		 i!=m_GuiVec.end(); i++)
	{
		m_MainPack->remove((*i)->m_SliderGroup);
		delete *i;
	}
	m_GuiVec.clear();
	m_CVCount=0;
}

void ControllerPluginGUI::UpdateValues(SpiralPlugin *o)
{	
	ControllerPlugin *Plugin = (ControllerPlugin *)o;

	int c;
	string Title,Min,Max;
	char temp[64];
	
	Clear();
	
	c=Plugin->GetNum();
	for (int n=0; n<c; n++)
	{
		AddCV();		
		m_GuiVec[n]->m_Title->value(Plugin->GetName(n).c_str());
		sprintf(temp,"%f",Plugin->GetMin(n));
		m_GuiVec[n]->m_Min->value(temp);
		sprintf(temp,"%f",Plugin->GetMax(n));
		m_GuiVec[n]->m_Max->value(temp);
		m_GuiVec[n]->m_Chan->value(Plugin->GetVal(n));	
	}
	
	resize(x(),y(),c*60,h());
}
	
inline void ControllerPluginGUI::cb_Chan_i(Fl_Slider* o, void* v) 
{ 
	int num=*(int*)(v);
	// swap em over, cos it's the easiqest way to reverse
	// the fltk slider, which is upside down imho	
	long max=strtol(m_GuiVec[num]->m_Min->value(),NULL,10);
	long min=strtol(m_GuiVec[num]->m_Max->value(),NULL,10);
	float val=o->value()*(max-min)+min;				
	m_GUICH->Set("Number",num);
	m_GUICH->Set("Value",val);
	m_GUICH->Set("Min",(int)min);
	m_GUICH->Set("Max",(int)max);
	char temp[256];
	sprintf(temp,"%s",m_GuiVec[num]->m_Title->value());
	m_GUICH->Set("Name",temp);	
	m_GUICH->SetCommand(ControllerPlugin::SETCHANNEL);
}
void ControllerPluginGUI::cb_Chan(Fl_Slider* o, void* v) 
{ ((ControllerPluginGUI*)(o->parent()->user_data()))->cb_Chan_i(o,v);}

inline void ControllerPluginGUI::cb_Add_i(Fl_Button* o, void* v) 
{ 
	if (m_CVCount<MAX_CHANNELS)
	{
		AddCV();
		resize(x(),y(),w()+60,h());
		redraw();
		m_GUICH->Set("Number",(int)m_GuiVec.size());
		m_GUICH->SetCommand(ControllerPlugin::SETNUM);
	}
}
void ControllerPluginGUI::cb_Add(Fl_Button* o, void* v) 
{ ((ControllerPluginGUI*)(o->parent()))->cb_Add_i(o,v);}

inline void ControllerPluginGUI::cb_Delete_i(Fl_Button* o, void* v) 
{ 
	if (m_GuiVec.size()>1)
	{
		DeleteCV();
		resize(x(),y(),w()-60,h());
		redraw(); 
		
		m_GUICH->Set("Number",(int)m_GuiVec.size());
		m_GUICH->SetCommand(ControllerPlugin::SETNUM);
	}
}
void ControllerPluginGUI::cb_Delete(Fl_Button* o, void* v) 
{ ((ControllerPluginGUI*)(o->parent()))->cb_Delete_i(o,v);}

// call for version <3
istream &operator>>(istream &s, ControllerPluginGUI &o)
{
	int c;
	string Title,Min,Max;
	float Val;
	
	o.Clear();
	
	s>>c;
	for (int n=0; n<c; n++)
	{
		s>>Title>>Min>>Max>>Val;		
		o.AddCV();		
		o.m_GuiVec[n]->m_Title->value(Title.c_str());
		o.m_GuiVec[n]->m_Min->value(Min.c_str());
		o.m_GuiVec[n]->m_Max->value(Max.c_str());
		o.m_GuiVec[n]->m_Chan->value(Val);	
	}
	
	o.resize(o.x(),o.y(),c*60,o.h());
	
	return s;
}

// call for version >=3
// another version of the stream to fix the old style string streaming problem
void ControllerPluginGUI::StreamIn(istream &s)
{
	int c,version,size;
	string Title,Min,Max;
	float Val;
	char Buf[4096];
	
	Clear();
	s>>version;
	s>>c;
	for (int n=0; n<c; n++)
	{		
		AddCV();		
		
		s>>size;
		s.ignore(1);
		s.get(Buf,size+1);
		m_GuiVec[n]->m_Title->value(Buf);
		
		s>>Min>>Max>>Val;	
		
		m_GuiVec[n]->m_Min->value(Min.c_str());
		m_GuiVec[n]->m_Max->value(Max.c_str());
		m_GuiVec[n]->m_Chan->value(Val);	
	}
	
	resize(x(),y(),c*60,h());
}

void ControllerPluginGUI::StreamOut(ostream &s)
{
	// version
	s<<1<<endl;
	s<<m_GuiVec.size()<<endl;
	for (vector<ControllerPluginGUI::CVGUI*>::iterator i=m_GuiVec.begin();
		 i!=m_GuiVec.end(); i++)
	{
		s<<strlen((*i)->m_Title->value())<<endl;
		s<<(*i)->m_Title->value()<<" ";
		s<<(*i)->m_Min->value()<<" ";
		s<<(*i)->m_Max->value()<<" ";
		s<<(*i)->m_Chan->value()<<endl;		
	}
}
