/*  SpiralLoops
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

#ifndef LOOP
#define LOOP

#include "SpiralSound/Filter.h"
#include "GUI/LoopGUI.h"
#include "../../Sample.h"

class Loop 
{
public:
	Loop();
	~Loop();

	void LoadWav(const char *Filename);
	void SaveWav(const char *Filename);
	bool GetOutput(Sample &data);
	void AllocateMem(int Length);
	void Clear();
	void Hold() {m_StoreBuffer=m_HoldBuffer;}
	void Double();
	void MatchLength(int Len);
	void Crop();
	void StreamIn(istream &s);
	void StreamOut(ostream &s);

	void SetGUI(LoopGUI *s)  { m_GUI=s; }
	void SetId(int Id)       { m_Id=Id; }
	void SetCutoff(int c)    { m_Filter.SetCutoff(c); }
	void SetResonance(int r) { m_Filter.SetResonance(r); }
	void SetPos(int Pos)     { m_Pos=Pos; }
	void SetLength(int Len)  { m_LoopPoint=Len; }
	void SetSpeed(float Speed) { m_Speed=Speed; }
	void SetVolume(float Vol)  { m_Volume=Vol; }
	void SetPlaying(bool Playing) { m_Playing=Playing; }
	void SetRecordingSource(const float *s) { m_RecordingSource=s; }
	void Record(bool r) { m_Recording=r; if (!r) EndRecordBuf(); }
	void SetMasterStatus(bool m) { m_Master=m; }
	void FilterBypass(bool s) { m_Filter.FilterBypass(s); }
	void DelMe()              { m_DelMe=true; }
	void SetBalance(float s)  { m_Balance=s; m_LeftVol=(2-s)/2; m_RightVol=(1+s-1.0f)/2;}	
	void Trigger()				{ m_Pos=0; SetPlaying(true); }
	void SetEffects(bool s) { m_EffectsOn=s; }
	
	const float  GetVolume() {return m_Volume;}
	const float  GetCutoff() {return m_Filter.GetCutoff();}
	const float  GetResonance() {return m_Filter.GetCutoff();}
	const float  GetBalance() {return m_Balance;}
	const bool   GetEffects() {return m_EffectsOn;}
	const bool   IsPlaying() {return m_Playing;}
	const float *GetLoopPtr() {return m_StoreBuffer.GetBuffer();}
	const int    GetId() {return m_Id;}
	const int    GetLoopLength() {return m_LoopPoint;}
	float 		*GetPosPtr() {return &m_Pos;}
	const int    GetTotalLength() {assert(m_StoreBuffer.GetLength()==m_HoldBuffer.GetLength()); return m_StoreBuffer.GetLength();}
	const bool   IsMaster() {return m_Master;}
	const bool   Delete()   {return m_DelMe; }
	const float  GetSpeed() {return m_Speed;}
	const float  GetLeftVol()     { return m_LeftVol; }
	const float  GetRightVol()     { return m_RightVol; }
	const float  GetCurrentAngle() { return m_LoopPoint?(m_Pos/m_LoopPoint)*360.0f:0; }
	const string& GetSampleName()  { return m_Sample; }
	
	void Cut(int Start, int End);
	void Copy(int Start, int End);
	void Paste(int Start);
	void PasteMix(int Start);
	
	void ZeroRange(int Start, int End);
	void ReverseRange(int Start, int End);
	void Halve();
	void SelectAll();
	void Move(int Start);
		
private:

	void RecordBuf(float Pos, int Length);
	void EndRecordBuf();

	Filter m_Filter;
	
	int    m_Id;
	float  m_Pos;
	int    m_IntPos;
	int    m_PlayBufPos;
	bool   m_Playing;
	bool   m_Recording;
	bool   m_Master;
	bool   m_DelMe;
	long int m_LoopPoint;
	float  m_Speed;
	float  m_Volume;
	const float *m_RecordingSource;
	Sample m_StoreBuffer;
	Sample m_HoldBuffer;
	Sample m_RecBuffer;
	int    m_RecPos;
	float  m_Balance;
	float  m_LeftVol,m_RightVol;
	
	bool   m_FirstRecord;
	bool   m_FixedRecord;
	int    m_RecLength;
	bool   m_EffectsOn;
	
	string m_Sample;
	
	LoopGUI *m_GUI;
	
	friend istream &operator>>(istream &s, Loop &o);
	friend ostream &operator<<(ostream &s, Loop &o);	
};

istream &operator>>(istream &s, Loop &o);
ostream &operator<<(ostream &s, Loop &o);

#endif
