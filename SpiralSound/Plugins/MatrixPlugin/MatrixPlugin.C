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
#include "MatrixPlugin.h"
#include "MatrixPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../RiffWav.h"
#include "../../NoteTable.h"

extern "C" {
SpiralPlugin* CreateInstance()
{
	return new MatrixPlugin;
}

char** GetIcon()
{
	return SpiralIcon_xpm;
}

int GetID()
{
	return 0x0012;
}
}

///////////////////////////////////////////////////////

MatrixPlugin::MatrixPlugin() :
m_TickTime(1.0f),
m_StepTime(1.0f),
m_Time(0.0f),
m_Step(0),
m_Loop(true),
m_NoteCut(false),
m_Current(0),
m_CurrentNoteCV(0),
m_CurrentTriggerCV(0),
m_Triggered(false),
m_ClockHigh(false),
m_CopyPattern(0)
{
	m_Version=2;

	m_PluginInfo.Name="Matrix";
	m_PluginInfo.Width=555;
	m_PluginInfo.Height=270;
	m_PluginInfo.NumInputs=5;
	m_PluginInfo.NumOutputs=19;		
	m_PluginInfo.PortTips.push_back("Play Trigger");	
	m_PluginInfo.PortTips.push_back("StepTime CV");	
	m_PluginInfo.PortTips.push_back("Input Pitch CV");	
	m_PluginInfo.PortTips.push_back("Input Trigger CV");
	m_PluginInfo.PortTips.push_back("External Clock");
	m_PluginInfo.PortTips.push_back("Output Pitch");	
	m_PluginInfo.PortTips.push_back("Output Trigger");
	m_PluginInfo.PortTips.push_back("Trigger 1");
	m_PluginInfo.PortTips.push_back("Trigger 2");
	m_PluginInfo.PortTips.push_back("Trigger 3");
	m_PluginInfo.PortTips.push_back("Trigger 4");
	m_PluginInfo.PortTips.push_back("Trigger 5");
	m_PluginInfo.PortTips.push_back("Trigger 6");
	m_PluginInfo.PortTips.push_back("Trigger 7");
	m_PluginInfo.PortTips.push_back("Trigger 8");
	m_PluginInfo.PortTips.push_back("Trigger 9");
	m_PluginInfo.PortTips.push_back("Trigger 10");
	m_PluginInfo.PortTips.push_back("Trigger 11");
	m_PluginInfo.PortTips.push_back("Trigger 12");
	m_PluginInfo.PortTips.push_back("Trigger 13");
	m_PluginInfo.PortTips.push_back("Trigger 14");
	m_PluginInfo.PortTips.push_back("Trigger 15");
	m_PluginInfo.PortTips.push_back("Trigger 16");
	m_PluginInfo.PortTips.push_back("Reset Trigger");	
		
	for (int n=0; n<NUM_PATTERNS; n++)
	{
		m_Matrix[n].Length=32;
		m_Matrix[n].Speed=1.0f;
		m_Matrix[n].Octave=0;
		
		for (int x=0; x<MATX; x++)
		for (int y=0; y<MATY; y++)
		{
			m_Matrix[n].Matrix[x][y]=false;
		}
		
		m_TriggerLevel[n]=0;
	}
	
	m_AudioCH->Register("NoteCut",&m_NoteCut,ChannelHandler::INPUT);
	m_AudioCH->Register("Current",&m_Current,ChannelHandler::INPUT);
	m_AudioCH->Register("StepTime",&m_StepTime,ChannelHandler::INPUT);
	m_AudioCH->Register("Num",&m_GUIArgs.Num,ChannelHandler::INPUT);
	m_AudioCH->Register("Length",&m_GUIArgs.Length,ChannelHandler::INPUT);
	m_AudioCH->Register("Speed",&m_GUIArgs.Speed,ChannelHandler::INPUT);
	m_AudioCH->Register("X",&m_GUIArgs.X,ChannelHandler::INPUT);
	m_AudioCH->Register("Y",&m_GUIArgs.Y,ChannelHandler::INPUT);
	m_AudioCH->Register("Octave",&m_GUIArgs.Octave,ChannelHandler::INPUT);
	m_AudioCH->Register("Step",&m_Step,ChannelHandler::OUTPUT);
	m_AudioCH->RegisterData("Matrix",ChannelHandler::OUTPUT_REQUEST,&m_Matrix,sizeof(m_Matrix));
}

MatrixPlugin::~MatrixPlugin()
{
}

PluginInfo &MatrixPlugin::Initialise(const HostInfo *Host)
{	
	PluginInfo& Info = SpiralPlugin::Initialise(Host);
	m_TickTime = 1.0f/(float)m_HostInfo->SAMPLERATE;
	return Info;
}

SpiralGUIType *MatrixPlugin::CreateGUI()
{
	return new MatrixPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void MatrixPlugin::Execute()
{	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		if (InputExists(1)) m_StepTime = GetInput(1,n);
		
		// inc time 
		m_Time+=m_TickTime;
		
		SetOutputPitch(0,n,m_CurrentNoteCV);
		SetOutput(1,n,m_CurrentTriggerCV);
                
		if (m_Step+1 >= m_Matrix[m_Current].Length) SetOutput(18, n, 1);
        else SetOutput(18, n, 0);

		if (GetInputPitch(0,n)>0) 
		{
			if (!m_Triggered)
			{
				float Freq=GetInputPitch(0,n);
				// Notes 0 to 16 trigger patterns 0 to 16
				// No other notes catered for
				for (int i=0; i<NUM_PATTERNS; i++)
				{
					if (feq(Freq,NoteTable[i],0.01f))
					{						
						m_Current=i;
						break;
					}
				}
				
				// make it so the next note to trigger
				// will be the first one
				
				//if (m_GUI) ((MatrixPluginGUI*)m_GUI)->UpdateValues();
								
				m_Time=m_StepTime*(1/m_Matrix[m_Current].Speed);
				m_Step=-1;

				m_Triggered=true;
			}
		}
		else
		{
			m_Triggered=false;
		}


		// set the individual triggers
		for (int t=0; t<NUM_PATTERNS; t++) SetOutput(t+2,n,m_TriggerLevel[t]);


		bool ExternalClock = InputExists(4);
		bool ExternalClockTriggered=false;
		
		if (ExternalClock)
		{
			if (GetInput(4,n)>0)
			{
				if(!m_ClockHigh)
				{
					m_ClockHigh=true;
					ExternalClockTriggered=true;
				}
			}
			else
			{
				if (m_ClockHigh)
				{
					m_ClockHigh=false;
					ExternalClockTriggered=true;
				}
			}
			
			// reset the position on a signal from input 1
			if (InputExists(0) && GetInput(0,n)!=0)
			{
				m_Step=-1;
				ExternalClockTriggered=true;
			}
		}
			
			
				
		// An external clock pulse overrides the internal timing
		if ((!ExternalClock && m_Time>=m_StepTime*(1/m_Matrix[m_Current].Speed)) ||
			(ExternalClock && ExternalClockTriggered)) 
		{		
			m_Time=0;
			m_Step++;
			
			if (m_Step >= m_Matrix[m_Current].Length) m_Step=0;
			
			//if (m_GUI) ((MatrixPluginGUI*)m_GUI)->SetLED(m_Step);
			
			// Reset the values
			m_CurrentTriggerCV=0;
			if (m_NoteCut) m_CurrentNoteCV=0;
			for (int t=0; t<NUM_PATTERNS; t++) 
			{
				SetOutput(t+2,n,0);
				m_TriggerLevel[t]=0;
			}
			
			
			// Scan the matrix at current time 
			for (int i=0; i<MATY; i++)
			{
				if (m_Matrix[m_Current].Matrix[m_Step][i])
				{
				
					m_CurrentNoteCV=NoteTable[i+m_Matrix[m_Current].Octave*12];
					m_CurrentTriggerCV=1;
					m_TriggerLevel[i]=1;
				}
			}
			
			// reset the triggers between steps to clear them
			// otherwise consecutive events wont get triggered
			SetOutput(1,n,0);
			for (int t=0; t<NUM_PATTERNS; t++) SetOutput(t+2,n,0);
		}
	}
}

void MatrixPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch (m_AudioCH->GetCommand())
		{
			case MAT_LENGTH   :
				m_Matrix[m_Current].Length=m_GUIArgs.Length; 
			break;
			
			case MAT_SPEED    : 
				m_Matrix[m_Current].Speed=m_GUIArgs.Speed; 
			break;		
			
			case MAT_ACTIVATE : 
				m_Matrix[m_Current].Matrix[m_GUIArgs.X][m_GUIArgs.Y]=true; 
			break;		
			
			case MAT_DEACTIVATE : 
				m_Matrix[m_Current].Matrix[m_GUIArgs.X][m_GUIArgs.Y]=false; 
			break;
			case MAT_OCTAVE : 
				m_Matrix[m_Current].Octave=m_GUIArgs.Octave; 
			break;
			case COPY : 
				CopyPattern(); 
			break;
			case PASTE : 
				PastePattern(); 
			break;
			case CLEAR : 
				ClearPattern(); 
			break;
			case TUP : 
				if (CanTransposeUp()) TransposeUp(); 
			break;
			case TDOWN : 
				if (CanTransposeDown()) TransposeDown(); 
			break;
		}
	}
}

void MatrixPlugin::PastePattern() {
     m_Matrix[m_Current].Length = m_Matrix[m_CopyPattern].Length;
     m_Matrix[m_Current].Speed = m_Matrix[m_CopyPattern].Speed;
     m_Matrix[m_Current].Octave = m_Matrix[m_CopyPattern].Octave;
     for (int y=0; y<MATY; y++) {
         for (int x=0; x<MATX; x++) {
             m_Matrix[m_Current].Matrix[x][y] = m_Matrix[m_CopyPattern].Matrix[x][y];
         }
     }
}

void MatrixPlugin::ClearPattern() {
     for (int y=0; y<MATY; y++) {
         for (int x=0; x<MATX; x++) {
             m_Matrix[m_Current].Matrix[x][y] = 0;
         }
     }
}

void MatrixPlugin::TransposeUp() {
     int x, y;
     for (y=MATY-1; y>=0; y--) {
         for (x=0; x<MATX; x++) {
             m_Matrix[m_Current].Matrix[x][y] = m_Matrix[m_Current].Matrix[x][y-1];
         }
     }
     for (x=0; x<MATX; x++) {
         m_Matrix[m_Current].Matrix[x][0] = 0;
     }
}

void MatrixPlugin::TransposeDown() 
{
    int x, y;
    for (y=0; y<MATY-1; y++) 
	{
        for (x=0; x<MATX; x++) 
		{
            m_Matrix[m_Current].Matrix[x][y] = m_Matrix[m_Current].Matrix[x][y+1];
        }
    }
    for (x=0; x<MATX; x++) 
	{
    	m_Matrix[m_Current].Matrix[x][MATY-1] = 0;
    }
}

bool MatrixPlugin::CanTransposeDown() 
{
    for (int x=0; x<MATX; x++) if (m_Matrix[m_Current].Matrix[x][0]) return False;
    return True;
}

bool MatrixPlugin::CanTransposeUp() 
{
     for (int x=0; x<MATX; x++) if (m_Matrix[m_Current].Matrix[x][MATY-1]) return False;
     return True;
}

void MatrixPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";
	s<<m_Current<<" "<<m_Time<<" "<<m_Step<<" "<<m_Loop<<" "<<m_NoteCut<<" "<<endl;
	
	for (int n=0; n<NUM_PATTERNS; n++)	
	{
		s<<m_Matrix[n].Length<<" "<<m_Matrix[n].Speed<<" "<<m_Matrix[n].Octave<<endl;
		
		for (int y=0; y<MATY; y++)
		{
			for (int x=0; x<MATX; x++)
			{
				if (m_Matrix[n].Matrix[x][y]) s<<x<<" "<<y<<"  ";
			}
		}
		s<<"-1 ";
	}
}

void MatrixPlugin::StreamIn(istream &s)
{
	int version;
	s>>version;
	
	switch (version)
	{
		case 1:
		{
			s>>m_Current>>m_Time>>m_Step>>m_Loop>>m_NoteCut;
	
			for (int n=0; n<NUM_PATTERNS; n++)	
			{	
				s>>m_Matrix[n].Length>>m_Matrix[n].Speed>>m_Matrix[n].Octave;
		
				for (int y=0; y<MATY; y++)
				for (int x=0; x<MATX; x++)
				{
					s>>m_Matrix[n].Matrix[x][y];
				}
			}
		} break;
	
		case 2:
		{
			s>>m_Current>>m_Time>>m_Step>>m_Loop>>m_NoteCut;
	
			for (int n=0; n<NUM_PATTERNS; n++)	
			{	
				s>>m_Matrix[n].Length>>m_Matrix[n].Speed>>m_Matrix[n].Octave;
		
				int x=0,y=0;
				while(x!=-1)
				{
					s>>x;
					if (x!=-1)
					{						
						s>>y;
						if (y!=-1) m_Matrix[n].Matrix[x][y]=true;
					}
				}
			}
		} break;
	}
}
