/*  SpiralSynth
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

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <limits.h>
#include <queue>
#include <string>

#ifndef MIDI
#define MIDI

using namespace std;

//#define KEYBOARD_SUPPORT


class MidiEvent
{
public:
	enum type{NONE,ON,OFF,AFTERTOUCH,PARAMETER,CHANNELPRESSURE,PITCHBEND};

	MidiEvent() {m_Type=NONE;}
	MidiEvent(type t, int note, float v) 
		{m_Type=t; m_Note=note; m_Volume=v;}
		
	type GetType() const {return m_Type;}
	float GetVolume() const {return m_Volume;}
	int GetNote() const {return m_Note;}
private:
	float m_Volume;
	type  m_Type;
	int   m_Note;
};

class MidiDevice
{
public:
	~MidiDevice();

	static void SetDeviceName(string s)   { m_DeviceName=s; }
	static MidiDevice *Get()      { if(!m_Singleton) m_Singleton=new MidiDevice; return m_Singleton; }
	static void PackUpAndGoHome() { if(m_Singleton)  delete m_Singleton; m_Singleton=NULL; }

	static void *MidiReaderCallback(void *o) { ((MidiDevice*)o)->CollectEvents(); return NULL; }

	MidiEvent GetEvent(int Device);
	void SendEvent(int Device,const MidiEvent &Event);

	void SetPoly(int s) { m_Poly=s; }

private:
	MidiDevice();

	void Open();
	void Close();
	void CollectEvents();
	void AddEvent(unsigned char* midi);

	void ReadByte(unsigned char *c);

	int  m_MidiFd;
	int  m_MidiWrFd;
	static string m_DeviceName;
	int  m_Poly;
	
	queue<MidiEvent> m_EventVec[16];
	
	static MidiDevice *m_Singleton;

	pthread_t        m_MidiReader;
	pthread_mutex_t* m_Mutex;	

#ifdef KEYBOARD_SUPPORT
	void CheckKeyboard();
	char m_KeyVoice[256];
	int m_Oct;
	int m_CurrentVoice;
#endif
};

#endif
