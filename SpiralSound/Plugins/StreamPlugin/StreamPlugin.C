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
#include "StreamPlugin.h"
#include "StreamPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../RiffWav.h"
#include "../../NoteTable.h"

static const float TRIG_THRESH = 0.1;
static const float BUFSECONDS = 1.0f;

extern "C" {
SpiralPlugin* CreateInstance()
{
	return new StreamPlugin;
}

char** GetIcon()
{
	return SpiralIcon_xpm;
}

int GetID()
{
	return 0x0119;
}
}

///////////////////////////////////////////////////////

StreamPlugin::StreamPlugin() :
m_SampleSize(256),
m_Pos(0),
m_StreamPos(0),
m_GlobalPos(0),
m_Mode(STOPM)
{
	m_PluginInfo.Name="Stream";
	m_PluginInfo.Width=245;
	m_PluginInfo.Height=165;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=2;
	m_PluginInfo.PortTips.push_back("Pitch CV");			
	m_PluginInfo.PortTips.push_back("Left Out");	
	m_PluginInfo.PortTips.push_back("Right Out");	
						
	m_StreamDesc.Volume     = 1.0f;
	m_StreamDesc.Pitch      = 1.0f;
	m_StreamDesc.PitchMod   = 1.0f;
	m_StreamDesc.SamplePos  = -1;
	m_StreamDesc.Loop       = false;	
	m_StreamDesc.Note       = 0;	
	m_StreamDesc.Pathname   = "None";
	m_StreamDesc.TriggerUp  = true;
	m_StreamDesc.SampleRate = 44100;
	m_StreamDesc.Stereo     = false;

	m_AudioCH->Register("Volume",&m_StreamDesc.Volume);
	m_AudioCH->Register("Pitch",&m_StreamDesc.PitchMod,ChannelHandler::INPUT);
	m_AudioCH->RegisterData("FileName",ChannelHandler::INPUT,&m_FileNameArg,sizeof(m_FileNameArg));
	m_AudioCH->Register("Time",&m_TimeArg);
	m_AudioCH->Register("TimeOut",&m_TimeOut,ChannelHandler::OUTPUT);
}

StreamPlugin::~StreamPlugin()
{
	cerr << "I'm deleted" << endl;
}

PluginInfo &StreamPlugin::Initialise(const HostInfo *Host)
{	
	PluginInfo& Info = SpiralPlugin::Initialise(Host);
	return Info;
}

SpiralGUIType *StreamPlugin::CreateGUI()
{
	return new StreamPluginGUI(m_PluginInfo.Width,
						m_PluginInfo.Height,
						this,m_AudioCH,m_HostInfo);
}

void StreamPlugin::Execute()
{
	float CVPitch=0;
	
	if (m_File.IsOpen())
	{
		for (int n=0; n<m_HostInfo->BUFSIZE; n++)
		{
			CVPitch=GetInput(0,n)*10.0f;			
		
			if (m_Pos<0) 
			{
				m_Pos=m_SampleSize-1;
				m_StreamPos-=m_SampleSize;	
				if (m_StreamPos<0) 
				{
					m_StreamPos=m_File.GetSize()/2-m_SampleSize;				
					m_GlobalPos=m_StreamPos;
				}
				m_File.SeekToChunk(m_StreamPos);
				m_File.LoadChunk(m_SampleSize, m_SampleL, m_SampleR);
			}
			else
			{
				if (m_Pos>=m_SampleSize) 
				{
					//m_SampleSize=(int)(BUFSECONDS*m_HostInfo->SAMPLERATE*m_StreamDesc.PitchMod);				
					m_Pos=0;
					m_StreamPos+=m_SampleSize;	
					if (m_StreamPos>=m_File.GetSize()/2) 
					{
						m_StreamPos=0;				
						m_GlobalPos=0;
					}
					m_File.SeekToChunk(m_StreamPos);
					m_File.LoadChunk(m_SampleSize, m_SampleL, m_SampleR);
				}
			}
			 
			SetOutput(0,n,m_SampleL[m_Pos]*m_StreamDesc.Volume);
			SetOutput(1,n,m_SampleR[m_Pos]*m_StreamDesc.Volume);
			
			m_Pos+=m_StreamDesc.PitchMod+CVPitch;
			m_GlobalPos+=m_StreamDesc.PitchMod+CVPitch;
		}
		
		//if (((StreamPluginGUI*)m_GUI)->visible()) 
		//	((StreamPluginGUI*)m_GUI)->SetTime(GetTime());
		//Joe must fix this.
		m_TimeOut=GetTime();
	}
}

void StreamPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch(m_AudioCH->GetCommand())
		{
			case (LOAD)	: OpenStream(m_FileNameArg); break;
	                case (RESTART)	: Restart(); break;
	                case (STOP)     : Stop(); break;
        	        case (PLAY)	: Play(); break;
        	        case (HALF)     : m_StreamDesc.PitchMod/=2; break;
        	        case (RESET)    : m_StreamDesc.PitchMod=1; break;
        	        case (DOUBLE)   : m_StreamDesc.PitchMod*=2; break;
        	        case (NUDGE)    : Nudge(); break;
			case (SET_TIME) : SetTime(m_TimeArg); break;
		}
	}
}

void StreamPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";
	
	s<<m_StreamDesc.Volume<<" "<<
	m_StreamDesc.PitchMod<<" "<<
	m_StreamDesc.Pathname.size()<<" "<<
	m_StreamDesc.Pathname<<" ";
		
	s<<m_Pos<<" ";
	s<<m_StreamPos<<" ";
	s<<m_GlobalPos<<" ";
	s<<m_Pitch<<" "<<endl;
}

void StreamPlugin::StreamIn(istream &s)
{
	int version;
	s>>version;
	
	s>>m_StreamDesc.Volume>>
	m_StreamDesc.PitchMod;
	char Buf[4096];	
	int size;		
	s>>size;
	s.ignore(1);
	s.get(Buf,size+1);
	m_StreamDesc.Pathname=Buf;
	
	if (m_StreamDesc.Pathname!="None") 
		OpenStream(m_StreamDesc.Pathname);

	s>>m_Pos;
	s>>m_StreamPos;
	s>>m_GlobalPos;
	s>>m_Pitch;
}

void StreamPlugin::OpenStream(const string &Name)
{
	m_StreamPos=0;				
	m_GlobalPos=0;
	if (m_File.IsOpen()) m_File.Close(); 
	m_File.Open(Name,WavFile::READ);
	m_SampleL.Allocate(m_SampleSize);
	m_SampleR.Allocate(m_SampleSize);
	m_StreamDesc.Pathname=Name;
	m_StreamDesc.SampleRate=m_File.GetSamplerate();
	m_StreamDesc.Stereo=m_File.IsStereo();
	m_StreamDesc.Pitch = m_StreamDesc.SampleRate/(float)m_HostInfo->SAMPLERATE;												
	if (m_StreamDesc.Stereo) 
	{
		m_StreamDesc.Pitch*=2;	
		//((StreamPluginGUI*)m_GUI)->SetMaxTime(GetLength());
	}
	//else ((StreamPluginGUI*)m_GUI)->SetMaxTime(GetLength()/2);
	//Joe this must use the chanel handler
	
}

void StreamPlugin::Restart() 
{ 
	m_StreamPos=0; m_GlobalPos=0; 
}
	
void StreamPlugin::Play() 
{ 
	if (m_Mode==PLAYM) return;
	m_StreamDesc.PitchMod=m_Pitch; 
	m_Mode=PLAYM;
}

void StreamPlugin::Stop() 
{ 
	if (m_Mode==STOPM) return;
	m_Pitch=m_StreamDesc.PitchMod; 
	m_StreamDesc.PitchMod=0.0f;
	m_Mode=STOPM; 
}

float StreamPlugin::GetLength() 
{ 
	if (m_StreamDesc.Stereo) return (m_File.GetSize()/(float)m_StreamDesc.SampleRate)*0.5f;
	else return m_File.GetSize()/(float)m_StreamDesc.SampleRate;
}
	
void StreamPlugin::SetTime(float t) 
{ 
	m_GlobalPos=m_StreamDesc.SampleRate*t;
	m_StreamPos=(int)(m_StreamDesc.SampleRate*t);
	m_Pos=m_SampleSize;	
}

