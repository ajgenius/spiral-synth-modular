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
#include "NoteSnapPlugin.h"
#include "NoteSnapPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"

extern "C" {
SpiralPlugin* CreateInstance()
{
	return new NoteSnapPlugin;
}

char** GetIcon()
{
	return SpiralIcon_xpm;
}

int GetID()
{
	return 0x0018;
}
}

///////////////////////////////////////////////////////

NoteSnapPlugin::NoteSnapPlugin()
{
	m_PluginInfo.Name="Note Snap";
	m_PluginInfo.Width=220;
	m_PluginInfo.Height=125;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("Output");
}

NoteSnapPlugin::~NoteSnapPlugin()
{
}

PluginInfo &NoteSnapPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *NoteSnapPlugin::CreateGUI()
{
	return NULL;
}

void NoteSnapPlugin::Execute()
{	
	float Freq=0, OldFreq=0;
	float Out=0;
	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		Freq=GetInputPitch(0,n);
		
		if (Freq!=OldFreq) // if it's changed
		{
			for (int i=0; i<131; i++) // for every note
			{			
				if (Freq>=NoteTable[i] && Freq<NoteTable[i+1])
				{	
					Out=NoteTable[i];
				}
			}
		}
		
		OldFreq=Freq;
		SetOutputPitch(0,n,Out);		
	}
}
 
int main()
{
	Fl::visual(FL_DOUBLE|FL_RGB);
	
	HostInfo host;
	host.BUFSIZE=256;
	host.SAMPLERATE=44100;
	host.OUTPUTFILE="/dev/dsp";
	host.MIDIFILE="/dev/midi";
	host.POLY=1;
	host.GUI_COLOUR=100;
	
	
	SpiralPlugin* test = CreateInstance();
	test->Initialise(&host);
    test->CreateGUI();
		
    for (;;) 
	{
    	if (!Fl::check()) break;
		test->Execute();    	
  	}
	
	delete test;	
	
	return 1;	
}
