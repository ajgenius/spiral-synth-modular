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
#include "SeqSelectorPlugin.h"
#include "SeqSelectorPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"

extern "C" {
SpiralPlugin* CreateInstance()
{
	return new SeqSelectorPlugin;
}

char** GetIcon()
{
	return SpiralIcon_xpm;
}

int GetID()
{
	return 0x0015;
}
}

///////////////////////////////////////////////////////

SeqSelectorPlugin::SeqSelectorPlugin()
{
	m_PluginInfo.Name="SeqSelector";
	m_PluginInfo.Width=400;
	m_PluginInfo.Height=300;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=8;
	m_PluginInfo.PortTips.push_back("Trigger");	
	m_PluginInfo.PortTips.push_back("CV One");	
	m_PluginInfo.PortTips.push_back("CV Two");	
	m_PluginInfo.PortTips.push_back("CV Three");	
	m_PluginInfo.PortTips.push_back("CV Four");	
	m_PluginInfo.PortTips.push_back("CV Five");	
	m_PluginInfo.PortTips.push_back("CV Six");	
	m_PluginInfo.PortTips.push_back("CV Seven");	
	m_PluginInfo.PortTips.push_back("CV Eight");	
	
	m_Pos=0;
	m_Triggered=false;
	m_UseRange=false;	

	for(int i=0; i<8; i++)
	{	
		m_OutTemp[i]=0;
	}
}

SeqSelectorPlugin::~SeqSelectorPlugin()
{
}

PluginInfo &SeqSelectorPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *SeqSelectorPlugin::CreateGUI()
{
	m_GUI = new SeqSelectorPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_HostInfo);
	m_GUI->hide();
	return m_GUI;
}

void SeqSelectorPlugin::Execute()
{
	
	SeqSelectorPluginGUI *ssp=(SeqSelectorPluginGUI*)m_GUI;
	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		// Sends momentary spike of value when triggered, 
		// works on the note off, rather than the first
		// detection of a trigger value
		if (GetInput(0,n)>0.1)
		{
			m_Triggered=true;
		}
		else
		{			
			if (m_Triggered==true && ssp->GetNumLines()>0)			
			{								
				m_Pos++;
				
				if (m_UseRange)
				{
					if (m_Pos>=m_End)
					{
						m_Pos=m_Begin;
					}
				}
				else
				{
					if (m_Pos>=ssp->GetNumLines())
					{
						m_Pos=0;
					}
				}
								
				for(int i=0; i<8; i++)
				{			
					SetOutputPitch(i,n,NoteTable[(int)ssp->GetVal(m_Pos,i)]);
				}														
				ssp->SetLED(m_Pos);
			}					
			else
			{
				for(int i=0; i<8; i++)
				{
					// zero frequency
					SetOutputPitch(i,n,-1);		
				}
			}
			
			m_Triggered=false;						
		}		
	}
}

void SeqSelectorPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";
	((SeqSelectorPluginGUI*)m_GUI)->StreamOut(s);
}

void SeqSelectorPlugin::StreamIn(istream &s)
{	
	int version;
	s>>version;
	((SeqSelectorPluginGUI*)m_GUI)->StreamIn(s);
}
