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

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "Fl_SEvent.h"


#ifndef EVENTMAP
#define EVENTMAP

using namespace std;

/////////////////////////////////////////////////////////

class EventInfo
{
public:
	enum Type{START,ON,END};

	EventInfo(int ID, int Group, Type t, float s) 
		{m_ID=ID; m_Group=Group; m_Type=t; m_SectionStart=s;}
	int m_ID;
	int m_Group;
	Type m_Type;
	float m_SectionStart;
};

/////////////////////////////////////////////////////////

class Fl_EventMap : public Fl_Group
{
public:

	enum ModeType {ARRANGE_MAP,MELODY_MAP,PERCUSSION_MAP};

	Fl_EventMap(int x, int y, int w, int h, const char* label=0);
	
	virtual void draw();
	virtual int  handle(int  event);
	void CreateWindow();
	
	vector<EventInfo> GetEvents(float Time);
	
	void SetUpdate(bool s) {m_Update=s;}
	void SetZoomLevel(float s);
	void SetGridSizeX(int s)  { m_GridSizeX=s; }
	void SetGridSizeY(int s)  { m_GridSizeY=s; }

	void SetType(ModeType s)       { m_Type=s; }
	ModeType GetType()             { return m_Type; }

	// Sort this hideaous mess out!!
	void  SetSeq(void* s)          { m_SeqPointer=s; }
	void *GetSeq()                 { return m_SeqPointer; }
	float GetStartTime()           { return m_StartLoop;}
	float GetEndTime()             { return m_EndLoop;}
	void  SetEndTime(float s)      { m_EndLoop=s;}
	void  SetBeatsBar(int s)       { m_BeatsBar=s; redraw();}
	void  SetBarLength(float s)    { m_BarLength=s; redraw(); }
	void  SetSnapGap(float s);
//	void  SetKeyColMap(int *s)     { m_KeyColMap=s; }
	int  *GetKeyColMap()           { return m_KeyColMap; }
	int   GetX()                   { return x(); }
	int   GetY()                   { return y(); }
	void  SetUpdateLineClip(int X, int Y, int W, int H) 
	           			           { m_ClipX=X; m_ClipY=Y; m_ClipW=W; m_ClipH=H; }
    void  DrawBeats(bool s)        { m_DrawBeats=s; redraw(); }   
	int   AddEventTime(float st, int g, float lt, Fl_SEvent::Type EType, bool CallBack=true);
	void  SetEventLength(int ID, float l);
	int   AddEvent(int x, int y, int w, Fl_SEvent::Type EType, bool CallBack=true);
	void  CopyEvent(int x, int y, int w, int ID, float LengthTime);
	void  RemoveAllEvents();
	void  RemoveEvent(int ID);
	Fl_SEvent *GetEvent(int ID);
	void  CopyFrom(Fl_EventMap *Other);
	
	void  SetNewEventCallback(Fl_Callback* s)   { cb_NewEvent=s; }
	void  SetCopyEventCallback(Fl_Callback* s)  { cb_CopyEvent=s; }
	void  SetRightMouseCallback(Fl_Callback* s) { cb_RightMouse=s; }

	void  TriggerStart();
	void  TriggerEnd();

	void SetFirstUpdate() { m_FirstUpdate=true; }
		
protected:

	int  GetGroupFromY(int y);
	
private:

	void *m_SeqPointer;

	ModeType m_Type;

	bool  m_Update;
	float m_Zoom;
	int   m_GridSizeX;
	int   m_GridSizeY;
	int   m_PixelsPerSec;
	int   m_NextID;
	float m_StartLoop;
	float m_EndLoop;
	int   m_Pos;
	int   m_LastPos;
	
	bool  m_DrawBeats;
	float m_BarLength;
	int   m_BeatsBar;
	
	int   m_ClipX,m_ClipY,m_ClipW,m_ClipH;
	
	Fl_TriEvent *m_StartTri;
	Fl_TriEvent *m_PosTri;
	Fl_TriEvent *m_EndTri;

	int m_KeyColMap[12];
	
	map<int,Fl_SEvent*> m_EventMap;
	
	void (*cb_NewEvent)(Fl_Widget*, void*);
public:
	// needed to be called by the event widgets
	void (*cb_RightMouse)(Fl_Widget*, void*);
	void (*cb_CopyEvent)(Fl_Widget*, void*);
private:

	bool m_FirstUpdate;

	friend istream &operator>>(istream &s, Fl_EventMap &o);
	friend ostream &operator<<(ostream &s, Fl_EventMap &o);	
};

istream &operator>>(istream &s, Fl_EventMap &o);
ostream &operator<<(ostream &s, Fl_EventMap &o);

#endif
