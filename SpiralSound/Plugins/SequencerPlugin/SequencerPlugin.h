/*  SpiralSound
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

#include "../SpiralPlugin.h"
#include <FL/Fl_Pixmap.H>
#include "../Widgets/Fl_EventMap.h"
#include <list>

#ifndef SequencerPLUGIN
#define SequencerPLUGIN

const int NUM_PATTERNS = 16;

class Track
{
public:
	class Sequence
	{
	public:
		class Pattern
		{
		public:
			class Note
			{
			public:
				float time;
				int   note;
				float vol;
			};
		
			map<int,Note> m_NoteMap;
		};
	
		float m_StartTime;
		int   m_Pattern;
	};
	
	map<int,Sequence::Pattern> m_PatternMap;
	list<Sequence>   m_SequenceList;
};

class SequencerPlugin : public SpiralPlugin
{
public:
 	SequencerPlugin();
	virtual ~SequencerPlugin();
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void ExecuteCommands();
	virtual void StreamOut(ostream &s);
	virtual void StreamIn(istream &s);
		
	bool  GetNoteCut()               { return m_NoteCut; }	
	void  ClearAll()				 { /*m_Eventmap[m_CurrentPattern]->RemoveAllEvents();*/ }
	int   GetCurrentPattern()		 { return m_CurrentPattern; }
	
private:

	Track m_Track;

	void  SetEventMap(int n, Fl_EventMap* s) { /*m_Eventmap[n]=s;*/ }
	void  SetUpdate(bool s)          { /*m_Eventmap[m_CurrentPattern]->SetUpdate(s);*/ }
	void  SetZoom(float s)           { /*m_Eventmap[m_CurrentPattern]->SetZoomLevel(s);*/ }
	void  SetNoteCut(bool s)         { m_NoteCut=s; }
	void  SetEndTime(float s)        { /*m_Eventmap[m_CurrentPattern]->SetEndTime(s);*/ m_Length=s; }
	void  SetSpeed(float s)          { m_SpeedMod=s; }
	void  SetPattern(int s);
	
	float m_Time;
	float m_Length;
	bool  m_Loop;
	bool  m_NoteCut;
	float m_SpeedMod;		
	float m_CurrentNoteCV;
	float m_CurrentTriggerCV;
	bool  m_InNoteDown;
	int   m_InNoteID;
	float m_InNoteTime;
	int   m_CurrentPattern;
	bool  m_Triggered;
};

#endif
