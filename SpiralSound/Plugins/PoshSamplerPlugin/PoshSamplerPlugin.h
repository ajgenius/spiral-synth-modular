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

#ifndef PoshSamplerPLUGIN
#define PoshSamplerPLUGIN

static const int NUM_SAMPLES = 8;

struct SampleDesc
{
	string Pathname;
	float  Volume;
	float  Velocity;
	float  Pitch;
	float  PitchMod;
	bool   Loop;
	bool   PingPong;
	int    Note;
	int    Octave;
	bool   TriggerUp;	
	float  SamplePos;
	int    SampleRate;
	bool   Stereo;
	long   PlayStart;
	long   LoopStart;
	long   LoopEnd;
};

class PoshSamplerPlugin : public SpiralPlugin
{
public:
 	PoshSamplerPlugin();
	virtual ~PoshSamplerPlugin();
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void StreamOut(ostream &s);
	virtual void StreamIn(istream &s);
	
	virtual bool	    SaveExternalFiles(const string &Dir);
	virtual void	    LoadExternalFiles(const string &Dir);
	
	void LoadSample(int n, const string &Name);
	void SaveSample(int n, const string &Name);
	Sample* GetSample(int n)  { return m_SampleVec[n]; }
	
	void  SetVolume(int n, float s) { m_SampleDescVec[n]->Volume=s; }
	float GetVolume(int n)          { return m_SampleDescVec[n]->Volume; }
	void  SetPitch(int n, float s)  { m_SampleDescVec[n]->PitchMod=s; }
	float GetPitch(int n)           { return m_SampleDescVec[n]->PitchMod; }
	void  SetLoop(int n, bool s)    { m_SampleDescVec[n]->Loop=s; }
	bool  GetLoop(int n)            { return m_SampleDescVec[n]->Loop; }
	void  SetPingPong(int n, bool s){ m_SampleDescVec[n]->PingPong=s; }
	bool  GetPingPong(int n)        { return m_SampleDescVec[n]->PingPong; }
	void  SetNote(int n, int s)     { m_SampleDescVec[n]->Note=s; }
	int   GetNote(int n)            { return m_SampleDescVec[n]->Note; }
	void  SetOctave(int n, int s)   { m_SampleDescVec[n]->Octave=s-6; }
	int   GetOctave(int n)          { return m_SampleDescVec[n]->Octave+6; }
	//void  SetSampleNum(int n, int s){ m_SampleDescVec[n]->Note=s; }
	//int   GetSampleNum(int n)       { return m_SampleDescVec[n]->Note; }
	
	void SetPlayStart(int n, long s) { m_SampleDescVec[n]->PlayStart=s; }
	long GetPlayStart(int n)         { return m_SampleDescVec[n]->PlayStart; }
	void SetLoopStart(int n, long s) { m_SampleDescVec[n]->LoopStart=s; }
	long GetLoopStart(int n)         { return m_SampleDescVec[n]->LoopStart; }
	void SetLoopEnd(int n, long s)   { m_SampleDescVec[n]->LoopEnd=s; }
	long GetLoopEnd(int n)           { return m_SampleDescVec[n]->LoopEnd; }
	void SetRecord(bool s)    		 { m_Recording=s; }
		
	void  Cut(int n, long s, long e);
	void  Copy(int n, long s, long e);
	void  Paste(int n, long s, long e);
	void  Mix(int n, long s, long e);
	void  Crop(int n, long s, long e);
	void  Reverse(int n, long s, long e);
	void  Amp(int n, long s, long e);
		
	vector<Sample*> m_SampleVec;
	vector<SampleDesc*> m_SampleDescVec;
			
private:
	Sample m_CopyBuffer;
	bool   m_Recording;
};

#endif
