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
#include <FL/fl_file_chooser.h>

#include "Loop.h"
#include "../../RiffWav.h"
//#include "SpiralLoopsInfo.h"

static const int   RECBUFFERSIZE = 16384;
static const float RECORD_GAIN = 1.0f;

Loop::Loop() : 
m_Id(0),
m_Pos(0),
m_IntPos(0),
m_PlayBufPos(0),
m_Playing(true),
m_Recording(false),
m_Master(false),
m_DelMe(false),
m_LoopPoint(0),
m_Speed(1.0f),
m_Volume(1.0f),
m_RecordingSource(NULL),
m_Balance(1.0f),
m_LeftVol(1.0f),
m_RightVol(1.0f),
m_FirstRecord(true),
m_FixedRecord(false),
m_RecLength(0),
m_EffectsOn(false)
{
}

Loop::~Loop() 
{
}

void Loop::LoadWav(const char *Filename)
{
	WavFile wav;
	if (wav.Open(Filename, WavFile::READ))
	{
		Clear();
		AllocateMem(wav.GetSize());		
		wav.Load(m_StoreBuffer);
		//((Fl_Loop*)(m_GUI->GetGUI()))->DrawEveryThing();
		m_GUI->UpdateDataPtr();		
	}
}

void Loop::SaveWav(const char *Filename)
{
	WavFile wav;
	if (wav.Open(Filename, WavFile::WRITE, WavFile::MONO))
	{
		wav.Save(m_StoreBuffer);
	}
	
	m_Sample=Filename;
}

bool Loop::GetOutput(Sample &data)
{
	if (!m_Recording && !m_Playing)
	{
		return false;
	}
	
	if (!m_Recording && m_StoreBuffer.GetLength()==0)
	{
		return false;
	}
	
	if (m_Recording)
	{	
		RecordBuf(m_Pos, data.GetLength());
			
		if (!m_StoreBuffer.GetLength())
		{
			return false;
		}	
	}

	int Pos;

	for (int n=0; n<data.GetLength(); n++)
	{		
		Pos=static_cast<int>(m_Pos);
		
		// brute force fix
		if (Pos>0 && Pos<m_LoopPoint)
		{
			data.Set(n,m_StoreBuffer[m_Pos]*m_Volume);
		}
		else data.Set(n,0);
		
		m_Pos+=m_Speed;
		
		if (static_cast<int>(m_Pos)>=m_LoopPoint)
		{
			m_Pos=0;
		}
	}
	
	m_Filter.GetOutput(0,data);
	
	// causes problems if we save to a hold buffer 
	// while we're recording, as we might not know
	// the size of the final buffer
	if (!m_Recording)
	{
		for (int n=0; n<data.GetLength(); n++)
		{	
			if (m_PlayBufPos>=m_LoopPoint)
			{
				m_PlayBufPos=0;
			}	
	
			m_HoldBuffer.Set(m_PlayBufPos,data[n]);		
			m_PlayBufPos++;	
		}
	}
	
	m_IntPos=static_cast<int>(m_Pos);
	
	m_GUI->UpdatePos();
	
	return true;
}

void Loop::AllocateMem(int Length)
{		
	// We might need to keep these values (if loading workspace)
	if (m_LoopPoint>Length) m_LoopPoint=Length;
	if (m_Pos>Length) m_Pos=0;
	
	if (m_LoopPoint==0) m_LoopPoint=Length;
	
	if (!m_StoreBuffer.Allocate(Length) || 
	    !m_HoldBuffer.Allocate(Length))
	{
		cerr<<"AllocateMem can't allocate any more memory!"<<endl;
		Clear();
	}
}

void Loop::Clear()
{
	m_StoreBuffer.Clear();
	m_HoldBuffer.Clear();

	//m_LoopPoint=0;
}

void Loop::RecordBuf(float Pos, int Length)
{
	if (!m_RecordingSource) return;

	static float OldPos=Pos;

	if (m_FirstRecord)
	{
		// Find out if we want a fixed length record
		// based on the last sample length, or not
		if (m_StoreBuffer.GetLength())
		{
			m_FixedRecord=true;
		}
		else
		{
			m_FixedRecord=false;
			m_RecBuffer.Allocate(RECBUFFERSIZE);
			m_StoreBuffer.Clear();
			m_RecPos=0;
		}

		m_FirstRecord=false;
		m_RecLength=0;
		OldPos=Pos;
	}
	
	if (m_FixedRecord)
	{
		m_RecLength=m_LoopPoint;
				
		if (Pos>=m_StoreBuffer.GetLength())
		{
			Pos=0;
		}
			
		for (int n=0; n<Length; n++)
		{			
			// just add directly to the old buffer
			float temp=m_StoreBuffer[static_cast<int>(Pos)]+m_RecordingSource[n]*RECORD_GAIN;
			
			
			// fill in all the samples between the speed jump with the same value
			m_StoreBuffer.Set((int)Pos,temp);
			
			for (int i=static_cast<int>(OldPos); i<=static_cast<int>(Pos); i++)
			{
				m_StoreBuffer.Set(i,temp);
			}
			
			OldPos=Pos;
			Pos+=m_Speed;
			
			if (Pos>=m_StoreBuffer.GetLength())
			{
				Pos-=m_StoreBuffer.GetLength();				
				
				// remember to fill up to the end of the last buffer
				for (int i=static_cast<int>(OldPos); i<m_StoreBuffer.GetLength(); i++)
				{
					m_StoreBuffer.Set(i,temp);
				}
				
				// and the beggining of this one
				for (int i=0; i<Pos; i++)
				{
					m_StoreBuffer.Set(i,temp);
				}
				
				OldPos=0;	
			}
			
		}
	}
	else
	{
		for (int n=0; n<Length; n++)
		{							
			// see if we need a new buffer
			if (m_RecPos>=RECBUFFERSIZE)
			{
				// put the two buffers together
				m_StoreBuffer.Add(m_RecBuffer);				
				m_RecPos=0;
			}
				
			long temp=(long)(m_RecordingSource[n]*RECORD_GAIN);
			m_RecBuffer.Set(m_RecPos,temp);
			
			m_RecLength++;
			m_RecPos++;
		}
	}
}

void Loop::EndRecordBuf()
{
	m_FirstRecord=true;	
	m_LoopPoint=m_StoreBuffer.GetLength();
	
	if (!m_FixedRecord)
	{
		// reallocate the hold buffer for the new size
		// (if the size has changed)
		m_HoldBuffer.Allocate(m_LoopPoint);			
	}
}

void Loop::Crop()
{
	if (m_LoopPoint<m_StoreBuffer.GetLength())
	{
		m_StoreBuffer.CropTo(m_LoopPoint);		
		m_HoldBuffer.CropTo(m_LoopPoint); 
	}
}

void Loop::Double()
{
	Crop();
	
	m_StoreBuffer.Add(m_StoreBuffer);
	m_HoldBuffer.Add(m_HoldBuffer);
	m_LoopPoint=m_StoreBuffer.GetLength();
}

void Loop::MatchLength(int Len)
{
	// set the length, and make sure enough data is allocated
	if (m_StoreBuffer.GetLength()>Len)
	{
		SetLength(Len);
		return;
	}
	else
	{
		// if it's empty
		if (!m_StoreBuffer.GetLength())
		{			
			AllocateMem(Len);
			m_StoreBuffer.Zero();
		}
		else 
		// there is something in the buffer already, but we need to
		// add on some extra data to make the length the same
		{
			int ExtraLen=Len-m_StoreBuffer.GetLength();
			m_StoreBuffer.Expand(ExtraLen);
			m_HoldBuffer.Expand(ExtraLen);		
		}
	}
}

void Loop::Cut(int Start, int End)
{ 
	//m_Parent->Cut(m_StoreBuffer,Start,End); 
	if (m_StoreBuffer.GetLength()<m_LoopPoint)
	{
		m_LoopPoint=m_StoreBuffer.GetLength();
	}
	if (m_Pos>m_LoopPoint)
	{
		m_Pos=0;
	}
	
	m_HoldBuffer.Allocate(m_StoreBuffer.GetLength());
}

void Loop::Copy(int Start, int End)
{ 
	//m_Parent->Copy(m_StoreBuffer,Start,End); 
}

void Loop::Paste(int Start)
{ 
	//m_Parent->Paste(m_StoreBuffer,Start); 
	if (m_StoreBuffer.GetLength()<m_LoopPoint)
	{
		m_LoopPoint=m_StoreBuffer.GetLength();
	}
	if (m_Pos>m_LoopPoint)
	{
		m_Pos=0;
	}
	
	m_HoldBuffer.Allocate(m_StoreBuffer.GetLength());
}

void Loop::PasteMix(int Start)
{ 
	//m_Parent->PasteMix(m_StoreBuffer,Start); 
}

void Loop::ZeroRange(int Start, int End)
{
	for (int n=Start; n<End; n++)
	{
		m_StoreBuffer.Set(n,0);
	}
}

void Loop::ReverseRange(int Start, int End)
{
	m_StoreBuffer.Reverse(Start,End);
}

void Loop::Halve()
{
	m_StoreBuffer.Shrink(m_StoreBuffer.GetLength()/2);
	m_HoldBuffer.Shrink(m_HoldBuffer.GetLength()/2);

	if (m_StoreBuffer.GetLength()<m_LoopPoint)
	{
		m_LoopPoint=m_StoreBuffer.GetLength();
	}
	if (m_Pos>m_LoopPoint)
	{
		m_Pos=0;
	}
}

void Loop::SelectAll()
{
}

void Loop::Move(int Start)
{
	m_StoreBuffer.Move(Start);
}

void Loop::StreamIn(istream &s)
{
	s>>m_Id>>m_Pos>>m_IntPos>>m_PlayBufPos>>m_Playing>>m_Master>>m_LoopPoint>>
	  m_Speed>>m_Volume>>m_Balance>>m_LeftVol>>m_RightVol>>m_Filter>>
	  m_EffectsOn;
}

void Loop::StreamOut(ostream &s)
{
	s<<m_Id<<" "<<m_Pos<<" "<<m_IntPos<<" "<<m_PlayBufPos<<" "<<m_Playing
	 <<" "<<m_Master<<" "<<m_LoopPoint<<" "<<m_Speed<<" "<<m_Volume<<" "
	 <<m_Balance<<" "<<m_LeftVol<<" "<<m_RightVol<<" "<<m_Filter<<" "
	 <<m_EffectsOn<<" ";
}

istream &operator>>(istream &s, Loop &o)
{
	s>>o.m_Id>>o.m_Pos>>o.m_IntPos>>o.m_PlayBufPos>>o.m_Playing>>o.m_Master>>o.m_LoopPoint>>
	  o.m_Speed>>o.m_Volume>>o.m_Balance>>o.m_LeftVol>>o.m_RightVol>>o.m_Filter>>
	  o.m_EffectsOn;
	  
	char Buf[4096];
	int size;		
	s>>size;
	s.ignore(1);
	s.get(Buf,size+1);
	o.m_Sample=Buf;
		
	if (o.m_Sample!="")
	{
		o.LoadWav(o.m_Sample.c_str());
	}
	
	return s;	
}

ostream &operator<<(ostream &s, Loop &o)
{
	// if this sample is not saved, get the user to pick a filename.
	if (o.m_Sample=="")
	{
		char *fn=fl_file_chooser("Save loop as Wav file", "*.wav", NULL);
		if (fn && fn!="")
		{	
			o.m_Sample=fn;
			o.SaveWav(o.m_Sample.c_str());
		}
	}
	else
	{
		if (fl_ask("Save loop sample %s?",o.m_Sample.c_str()))
		{
			o.SaveWav(o.m_Sample.c_str());
		}					
	}
	
	s<<o.m_Id<<" "<<o.m_Pos<<" "<<o.m_IntPos<<" "<<o.m_PlayBufPos<<" "<<o.m_Playing
	 <<" "<<o.m_Master<<" "<<o.m_LoopPoint<<" "<<o.m_Speed<<" "<<o.m_Volume<<" "
	 <<o.m_Balance<<" "<<o.m_LeftVol<<" "<<o.m_RightVol<<" "<<o.m_Filter<<" "
	 <<o.m_EffectsOn<<" "<<o.m_Sample.size()<<" "<<o.m_Sample<<" ";
	 	
	return s;
}

