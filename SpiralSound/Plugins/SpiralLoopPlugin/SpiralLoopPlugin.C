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
#include "SpiralLoopPlugin.h"
#include "SpiralLoopPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../RiffWav.h"
#include "../../NoteTable.h"

static const float TRIG_THRESH = 0.1;
static const int   NOTETRIG    = NUM_SAMPLES*2+1;

extern "C" {
SpiralPlugin* CreateInstance()
{
	return new SpiralLoopPlugin;
}

char** GetIcon()
{
	return SpiralIcon_xpm;
}

int GetID()
{
	return 0x001a;
}

string GetGroupName()
{
	return "SpiralSound";
}
}

///////////////////////////////////////////////////////

SpiralLoopPlugin::SpiralLoopPlugin()
{
	m_PluginInfo.Name="SpiralLoop";
	m_PluginInfo.Width=300;
	m_PluginInfo.Height=320;
	m_PluginInfo.NumInputs=2;
	m_PluginInfo.NumOutputs=9;		
	m_PluginInfo.PortTips.push_back("Input");
	m_PluginInfo.PortTips.push_back("Play Trigger");	
	m_PluginInfo.PortTips.push_back("Output");
	m_PluginInfo.PortTips.push_back("LoopTrigger 0");
	m_PluginInfo.PortTips.push_back("LoopTrigger 1");
	m_PluginInfo.PortTips.push_back("LoopTrigger 2");
	m_PluginInfo.PortTips.push_back("LoopTrigger 3");
	m_PluginInfo.PortTips.push_back("LoopTrigger 4");
	m_PluginInfo.PortTips.push_back("LoopTrigger 5");
	m_PluginInfo.PortTips.push_back("LoopTrigger 6");
	m_PluginInfo.PortTips.push_back("LoopTrigger 7");			
	m_Loop = new Loop;
	
	m_Version=2;
}

SpiralLoopPlugin::~SpiralLoopPlugin()
{
}

PluginInfo &SpiralLoopPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *SpiralLoopPlugin::CreateGUI()
{
	m_GUI = new SpiralLoopPluginGUI(m_Loop, m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_HostInfo);
	m_GUI->hide();
	return m_GUI;
}

void SpiralLoopPlugin::Execute()
{
	if (InputExists(0)) m_Loop->SetRecordingSource(GetInput(0)->GetBuffer());
	else m_Loop->SetRecordingSource(NULL);

	for (int n=0; n<8; n++) GetOutputBuf(n+1)->Zero();

	
	// get the triggers active this frame
	vector<int> TriggerVec;
	((SpiralLoopPluginGUI*)m_GUI)->GetGUI()->
		CheckTriggers(m_Loop->GetCurrentAngle(),TriggerVec);

	for (vector<int>::iterator i=TriggerVec.begin();
		 i!=TriggerVec.end(); i++)
	{
		GetOutputBuf(*i+1)->Set(1);
	}
 
	m_Loop->GetOutput(*GetOutputBuf(0));	

	static bool Triggered=false;
	if (GetInput(1,0)>TRIG_THRESH) 
	{
		if (!Triggered)
		{
			m_Loop->Trigger();		
			Triggered=true;
		}
	}
	else Triggered=false;
}

void SpiralLoopPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";
	
	if (m_Version==1)
	{
		s<<*m_Loop<<" ";
		s<<*((SpiralLoopPluginGUI*)m_GUI)->GetGUI()<<" ";
	}
	else
	{
		m_Loop->StreamOut(s);
		s<<*((SpiralLoopPluginGUI*)m_GUI)->GetGUI()<<" ";
	}
}

void SpiralLoopPlugin::StreamIn(istream &s)
{
	int version;
	s>>version;
	
	if (version==1)
	{
		s>>*m_Loop;
		s>>*((SpiralLoopPluginGUI*)m_GUI)->GetGUI();
	}
	else
	{
		m_Loop->StreamIn(s);
		s>>*((SpiralLoopPluginGUI*)m_GUI)->GetGUI();
	}
}
	
bool SpiralLoopPlugin::SaveExternalFiles(const string &Dir)
{
	char temp[256];
	sprintf(temp,"%sSpiralLoopSample%d.wav",Dir.c_str(),GetID());	
	m_Loop->SaveWav(temp);
	return true;
}

void SpiralLoopPlugin::LoadExternalFiles(const string &Dir)
{
	char temp[256];
	sprintf(temp,"%sSpiralLoopSample%d.wav",Dir.c_str(),GetID());	
	m_Loop->LoadWav(temp);
}
