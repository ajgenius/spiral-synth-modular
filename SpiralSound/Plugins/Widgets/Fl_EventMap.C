/*  EventMap Widget
 *  Copyleft (C) 2001 David Griffiths <dave@pawfal.org>
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

#include "Fl_EventMap.h"
#include <FL/fl_draw.h>
#include <FL/Fl_Window.h>
#include <FL/x.H>
#include <stdio.h>

static const int AREA_WIDTH = 1000;
static const int AREA_HEIGHT = 1000;
static const int BUTTON_COLOUR = 10;

static const int EVENT_COLOUR = 0;

static const int BG_COLOUR = 139;
static const int LINE_COLOUR = 140;

////////////////////////////////////////////////////////////////////

Fl_EventMap::Fl_EventMap(int x, int y, int w, int h, const char* label) :
Fl_Group(x,y,w,h,label),
m_SeqPointer(NULL),
m_Type(ARRANGE_MAP),
m_Update(true),
m_Zoom(1.0f),
m_GridSizeX(0),
m_GridSizeY(10),
m_PixelsPerSec(100),
m_NextID(0),
m_StartLoop(1.0f),
m_EndLoop(2.0f),
m_Pos(0),
m_LastPos(0),
m_DrawBeats(false),
m_BarLength(100),
m_BeatsBar(8),
cb_NewEvent(NULL),
cb_RightMouse(NULL),
cb_CopyEvent(NULL),
m_FirstUpdate(true)
{
	// white/black keys for the melody
	m_KeyColMap[0] = FL_YELLOW;
	m_KeyColMap[1] = FL_BLUE;
	m_KeyColMap[2] = FL_YELLOW;
	m_KeyColMap[3] = FL_BLUE;
	m_KeyColMap[4] = FL_YELLOW;
	m_KeyColMap[5] = FL_BLUE;
	m_KeyColMap[6] = FL_YELLOW;
	m_KeyColMap[7] = FL_YELLOW;
	m_KeyColMap[8] = FL_BLUE;
	m_KeyColMap[9] = FL_YELLOW;
	m_KeyColMap[10] = FL_BLUE;
	m_KeyColMap[11] = FL_YELLOW;
}



void Fl_EventMap::CreateWindow()
{
	int yoff=0;
	
	//m_StartTri = new Fl_TriEvent(90, 5, 20, 10, "");
	//m_StartTri->LockY();
	//m_StartTri->SetResizeGrab(0);
	//m_StartTri->SetPixelsPerSec(m_PixelsPerSec,true);
	
	m_PosTri = new Fl_TriEvent(90, 5, 30, 20, "");
	m_PosTri->LockY();
	m_PosTri->SetResizeGrab(0);
	m_PosTri->SetPixelsPerSec(m_PixelsPerSec,true);
	
	m_EndTri = new Fl_TriEvent(50, 5, 20, 10, "");
	m_EndTri->LockY();
	m_EndTri->SetResizeGrab(0);	
	m_EndTri->SetPixelsPerSec(m_PixelsPerSec,true);
	
	show();
}

void Fl_EventMap::draw()
{
	clear_damage(FL_DAMAGE_ALL);
		
	fl_color(BG_COLOUR);
	fl_rectf(x(),y(),w(),h());
	// draw the section markers
	fl_color(LINE_COLOUR);
	
	if (m_GridSizeY)
	{
		// draw the snap points
		for (int n=y(); n<y()+h(); n+=m_GridSizeY)
		{
			fl_line(x(),n,x()+w(),n);
		}
	}

	// draw the bar markers	
	
	if (m_DrawBeats)
	{
		int BarLengthPixels=(int)(m_BarLength*(float)m_PixelsPerSec);
		int BeatLengthPixels=(int)(BarLengthPixels/(float)m_BeatsBar);
		
		for (int n=x(); n<x()+w(); n+=BarLengthPixels)
		{
			fl_line(n,y(),n,y()+h());
			
			if (m_Type!=ARRANGE_MAP)
			{
				// draw the beat markers
				for (int m=n+1; m<n+BarLengthPixels; m+=BeatLengthPixels)
				{
					fl_line(m,y(),m,y()+h());
				}
			}
		}
	}
		
	if (m_Type==ARRANGE_MAP)
	{
		fl_color(FL_BLACK);
		//m_StartLoop=m_StartTri->GetStartTime();
		//m_EndLoop=m_EndTri->GetStartTime();
	
		int StartPixels=(int)(m_StartLoop*(float)m_PixelsPerSec)+x();
		int EndPixels=(int)(m_EndLoop*(float)m_PixelsPerSec)+x();
		
		fl_line(StartPixels,y()+15,StartPixels,y()+h());
		fl_line(EndPixels,y()+15,EndPixels,y()+h());
	}
	
	Fl_Group::draw();
	
	SetFirstUpdate();
}

int Fl_EventMap::handle(int  event)
{	
	if (!Fl_Group::handle(event) && event==FL_PUSH)
	{	
		if (m_Type!=ARRANGE_MAP)
		{
			AddEvent(Fl::event_x(),Fl::event_y(),20, Fl_SEvent::NO_TYPE);
		}
	}

	for (map<int,Fl_SEvent*>::iterator i=m_EventMap.begin();
		 i!=m_EventMap.end(); i++)
	{
		Fl_SEvent *pEvent = i->second;
		
		if (pEvent->Killed())
		{
			RemoveEvent(i->first);
		}
	}
	
	return event;
}

void Fl_EventMap::SetZoomLevel(float s) 
{ 
	m_Zoom=s; 
	m_PixelsPerSec=int(100/m_Zoom);
	
	vector<EventInfo> EventVec;
	for (map<int,Fl_SEvent*>::iterator i=m_EventMap.begin();
		 i!=m_EventMap.end(); i++)
	{
		i->second->SetPixelsPerSec(m_PixelsPerSec);
	}
	
	//m_StartTri->SetPixelsPerSec(m_PixelsPerSec);
	m_EndTri->SetPixelsPerSec(m_PixelsPerSec);
	
	redraw();
}
	
void Fl_EventMap::SetSnapGap(float s)
{
	m_GridSizeX=(int)(s*(float)m_PixelsPerSec);

	for (map<int,Fl_SEvent*>::iterator i=m_EventMap.begin();
		 i!=m_EventMap.end(); i++)
	{
		i->second->SetSnapGap(s);
	}
		
	redraw();
}

vector<EventInfo> Fl_EventMap::GetEvents(float Time)
{
	assert(m_PixelsPerSec!=0);
	m_Pos = (int)(Time*m_PixelsPerSec);
	
	// line marker update 
	// all in all, some orrible code...
	if (m_Update && window()->visible() && m_Pos!=m_LastPos)
	{
		window()->make_current();
		int DrawPos=m_Pos+x();
		
		m_PosTri->resize(DrawPos,m_PosTri->y(),m_PosTri->w(),m_PosTri->h());
		redraw();
		
/*		if (DrawPos>m_ClipX && DrawPos<m_ClipX+m_ClipW)
		{		
			XSetFunction(fl_display,fl_gc,GXxor);
			if (!m_FirstUpdate) fl_line(m_LastPos,15+m_ClipY,m_LastPos,m_ClipY+m_ClipH);
			fl_line(DrawPos,15+m_ClipY,DrawPos,m_ClipY+m_ClipH);
			XSetFunction(fl_display,fl_gc,GXcopy);
			m_LastPos=DrawPos;
		}*/
		//if (Last!=NULL) Last->make_current();
	}
	m_FirstUpdate=false;
	
	vector<EventInfo> EventVec;
	for (map<int,Fl_SEvent*>::iterator i=m_EventMap.begin();
		 i!=m_EventMap.end(); i++)
	{
		Fl_SEvent *pEvent = i->second;
		
		bool State=pEvent->UpdateState(Time);
	
		EventInfo::Type type=EventInfo::ON;
			
		if (pEvent->AtStart()) type=EventInfo::START;
		if (pEvent->AtEnd())   type=EventInfo::END;
	
		// Collect the events happening now, or just finished
		if(State || pEvent->AtEnd())
		{
			EventVec.push_back(EventInfo(pEvent->GetID(),pEvent->GetGroup(),
						type,pEvent->GetStartTime()));
		}
	}
	return EventVec;
}

int Fl_EventMap::GetGroupFromY(int y)
{
	return y/m_GridSizeY;
}

int Fl_EventMap::AddEventTime(float st, int g, float lt, Fl_SEvent::Type EType, bool CallBack)
{
	int ex,ey,ew;
	ex=(int)(st*(float)m_PixelsPerSec)+x();
	ew=(int)(lt*(float)m_PixelsPerSec);
	ey=g*m_GridSizeY+y();

	return AddEvent(ex, ey, ew, EType, CallBack);
}

int Fl_EventMap::AddEvent(int x, int y, int w, Fl_SEvent::Type EType, bool CallBack)
{	
	Fl_SEvent *NewEvent;
	if (m_Type==PERCUSSION_MAP)
	{
		NewEvent = new Fl_CircEvent(x,y,w,m_GridSizeY);
	}
	else
	{
		NewEvent = new Fl_SEvent(x,y,w,m_GridSizeY);
	}
	
	if (m_Type==MELODY_MAP) NewEvent->LockResize(false);
	
	NewEvent->SetPixelsPerSec(m_PixelsPerSec, true);
	NewEvent->color(EVENT_COLOUR);
	NewEvent->labelsize(10);
	NewEvent->SetID(m_NextID++);
	NewEvent->SetType(EType);
	char Name[256];
	if (m_Type==ARRANGE_MAP) 
	{
		if(NewEvent->GetType()==Fl_SEvent::MELODY)
			sprintf(Name,"Melody %d",NewEvent->GetID());	
		
		if(NewEvent->GetType()==Fl_SEvent::PERCUSSION)
			sprintf(Name,"Percussion %d",NewEvent->GetID());	
		
	}
	else sprintf(Name,"This shouldnt be read!");	
	
	NewEvent->SetName(Name);
	NewEvent->SetGridX(m_GridSizeX);
	NewEvent->SetGridY(m_GridSizeY);
	
	map<int,Fl_SEvent*>::iterator i=m_EventMap.find(NewEvent->GetID());
	if (i!=m_EventMap.end())
	{
		cerr<<"Event ID not unique, not added!"<<endl;
		delete NewEvent;
		return -1;
	}
	
	add(NewEvent);
	NewEvent->SnapX();
	NewEvent->SnapY();
	
	NewEvent->SetPixelsPerSec(m_PixelsPerSec,true);
	
	m_EventMap[NewEvent->GetID()]=NewEvent;
	
	int ID = NewEvent->GetID();
	if (cb_NewEvent && CallBack) cb_NewEvent(NewEvent,&ID);
	
	redraw();
	return NewEvent->GetID();
}

void Fl_EventMap::CopyEvent(int x, int y, int w, int ID, float LengthTime)
{
	// make the new widget
	int NewID = AddEvent(x,y,w,m_EventMap[ID]->GetType(),false);
	
	// set the length of time exactly, as rounding 
	// errors cause a reinit of the sample memory
	GetEvent(NewID)->SetLengthTime(LengthTime);
	
	//string Name="Copy of "+m_EventMap[ID]->GetName();
	//GetEvent(NewID)->SetName(Name);
	
	GetEvent(NewID)->SetName(m_EventMap[ID]->GetName());
	
	if (cb_CopyEvent) cb_CopyEvent(GetEvent(NewID),&ID);
}

void Fl_EventMap::RemoveAllEvents()
{
	for (map<int,Fl_SEvent*>::iterator i=m_EventMap.begin();
		 i!=m_EventMap.end(); i++)
	{
		Fl_SEvent *e=i->second;
		remove(e);
		m_EventMap.erase(i);
		delete(e);
	}	
	m_EventMap.clear();
	m_NextID=0;
	
	redraw();
}	

void Fl_EventMap::RemoveEvent(int ID)
{
	map<int,Fl_SEvent*>::iterator i=m_EventMap.find(ID);
	if (i==m_EventMap.end())
	{
		cerr<<"Event ID not found to delete"<<endl;
		return;
	}	
	
	Fl_SEvent *e=i->second;
	remove(e);
	m_EventMap.erase(i);
	delete(e);
}	

Fl_SEvent *Fl_EventMap::GetEvent(int ID)
{
	map<int,Fl_SEvent*>::iterator i=m_EventMap.find(ID);
	if (i==m_EventMap.end())
	{
		cerr<<"Event ID not found"<<endl;
		return NULL;
	}	
	
	return m_EventMap[ID];
}	

void Fl_EventMap::SetEventLength(int ID, float l) 
{ 
	Fl_SEvent *e=GetEvent(ID);
	if(e) e->SetLengthTime(l); 
}

void Fl_EventMap::CopyFrom(Fl_EventMap *Other)
{
	m_Type         = Other->m_Type;
	m_Zoom         = Other->m_Zoom;
	m_GridSizeX    = Other->m_GridSizeX;
	m_GridSizeY    = Other->m_GridSizeY;
	m_PixelsPerSec = Other->m_PixelsPerSec;
	m_StartLoop    = Other->m_StartLoop;
	m_EndLoop      = Other->m_EndLoop;
	m_Pos          = Other->m_Pos;
	m_LastPos      = Other->m_LastPos;
	m_BarLength    = Other->m_BarLength;
	m_BeatsBar     = Other->m_BeatsBar;
	m_NextID	   = Other->m_NextID;
	
	for (map<int,Fl_SEvent*>::iterator i=Other->m_EventMap.begin();
		 i!=Other->m_EventMap.end(); i++)
	{
		Fl_SEvent *NewEvent;

		if (m_Type==PERCUSSION_MAP)
		{
			NewEvent = new Fl_CircEvent(*(Fl_CircEvent*)i->second);
		}
		else
		{
			NewEvent = new Fl_SEvent(*i->second);
		}

		add(NewEvent);	
		m_EventMap[NewEvent->GetID()]=NewEvent;
	}
}

/////////////////////////////////////////////////////

istream &operator>>(istream &s, Fl_EventMap &o)
{
	string dummy;
	s>>dummy;
	if (dummy!="EventMap") cerr<<"Error in stream before Fl_EventMap"<<endl;
	s>>(int&)o.m_Type;
	s>>o.m_Zoom;
	s>>o.m_GridSizeX;
	s>>o.m_GridSizeY;
	s>>o.m_PixelsPerSec;
	s>>o.m_StartLoop;
	s>>o.m_EndLoop;
	s>>o.m_Pos;
	s>>o.m_LastPos;
	s>>o.m_BarLength;
	s>>o.m_BeatsBar;
		
	int Num;
	
	s>>Num;
	float st,lt;
	int g,t;
	
	for (int n=0; n<Num; n++)
	{	
		s>>st>>g>>lt>>t;	
		o.AddEventTime(st,g,lt,(Fl_SEvent::Type)(t), false);
	}
		
	return s;	
}

ostream &operator<<(ostream &s, Fl_EventMap &o)
{ 
	s<<"EventMap ";
	s<<(int)o.m_Type<<" ";
	s<<o.m_Zoom<<" ";
	s<<o.m_GridSizeX<<" ";
	s<<o.m_GridSizeY<<" ";
	s<<o.m_PixelsPerSec<<" ";
	s<<o.m_StartLoop<<" ";
	s<<o.m_EndLoop<<" ";
	s<<o.m_Pos<<" ";
	s<<o.m_LastPos<<" ";
	s<<o.m_BarLength<<" ";
	s<<o.m_BeatsBar<<" ";
	
	s<<o.m_EventMap.size()<<" ";
	
	for (map<int,Fl_SEvent*>::iterator i=o.m_EventMap.begin();
		 i!=o.m_EventMap.end(); i++)
	{
		s<<i->second->GetStartTime()<<" "<<i->second->GetGroup()<<" "<<
		i->second->GetLengthTime()<<" "<<(int)(i->second->GetType())<<" ";
	}
	s<<endl;
	
	return s;
}

