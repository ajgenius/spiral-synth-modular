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
#include "../../RiffWav.h"
#include <FL/Fl_Pixmap.H>

#ifndef StreamPLUGIN
#define StreamPLUGIN

static const int NUDGESIZE=44100/4;

struct SampleDesc
{
	string Pathname;
	float  Volume;
	float  Pitch;
	float  PitchMod;
	bool   Loop;
	int    Note;
	bool   TriggerUp;	
	float  SamplePos;
	int    SampleRate;
	bool   Stereo;
};

class StreamPlugin : public SpiralPlugin
{
public:
	enum Mode{PLAYM,STOPM};

	enum GUICommands{NONE,LOAD,RESTART,STOP,PLAY,HALF,RESET,DOUBLE,NUDGE,SET_TIME};

 	StreamPlugin();
	virtual ~StreamPlugin();
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void ExecuteCommands();
	virtual void StreamOut(ostream &s);
	virtual void StreamIn(istream &s);
	
	// has to be defined in the plugin	
	virtual void UpdateGUI() { Fl::check(); }
	
	void OpenStream(const string &Name);
	
	void  SetVolume(float s) { m_StreamDesc.Volume=s; }
	float GetVolume()          { return m_StreamDesc.Volume; }
	void  SetPitch(float s)  { m_StreamDesc.PitchMod=s; }
	float GetPitch()           { return m_StreamDesc.PitchMod; }
	void  SetLoop(bool s)    { m_StreamDesc.Loop=s; }
	bool  GetLoop()            { return m_StreamDesc.Loop; }
	float GetLength();
	float GetTime() { return m_GlobalPos/(float)m_StreamDesc.SampleRate; }
	void  SetTime(float t);
	void  Nudge() { m_Pos-=NUDGESIZE; }
		
	void Restart();		
	void Play();
	void Stop();
	
private:
	
	WavFile    m_File;	
	Sample     m_SampleL;
	Sample     m_SampleR;
	SampleDesc m_StreamDesc;		
	int   m_SampleSize;
	float m_Pos;
	int   m_StreamPos;
	float m_GlobalPos;
	float m_Pitch;
	Mode  m_Mode;

	char m_FileNameArg[256];
	float m_TimeArg;
	float m_TimeOut;
	float m_MaxTime;
};

#endif
