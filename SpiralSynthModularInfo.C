/*  SpiralSynthModular
 *  Copyleft (C) 2002 David Griffiths <dave@pawfal.org>
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

#include "SpiralSynthModularInfo.h"
#include "SpiralSynthPluginLocation.h"
#include "FL/fl_draw.h"

string SpiralInfo::LOCALE      = "EN";
int    SpiralInfo::BUFSIZE     = 512;
int    SpiralInfo::FRAGSIZE    = 256;
int    SpiralInfo::FRAGCOUNT   = 8;
int    SpiralInfo::SAMPLERATE  = 44100;
long   SpiralInfo::MAXSAMPLE   = 32767;
float  SpiralInfo::VALUECONV   = 1.0f/MAXSAMPLE;
bool   SpiralInfo::WANTMIDI    = false;
int    SpiralInfo::FILTERGRAN  = 50;
string SpiralInfo::OUTPUTFILE  = "/dev/dsp";
string SpiralInfo::MIDIFILE    = "/dev/midi";
int    SpiralInfo::POLY        = 1;
bool   SpiralInfo::USEPLUGINLIST = false;

//int SpiralInfo::GUI_COLOUR = 139;
//int SpiralInfo::GUIBG_COLOUR = 0;
//int SpiralInfo::GUIBG2_COLOUR = 49;//45;

int SpiralSynthModularInfo::GUICOL_Tool=179;
int SpiralSynthModularInfo::GUICOL_Button=181;
int SpiralSynthModularInfo::GUICOL_Canvas=181;
int SpiralSynthModularInfo::GUICOL_Device=181;
string SpiralSynthModularInfo::BGIMG;

vector<string> SpiralSynthModularInfo::PLUGINVEC;
string SpiralSynthModularInfo::PLUGIN_PATH;

SpiralSynthModularInfo* SpiralSynthModularInfo::m_SpiralSynthModularInfo=NULL;

SpiralSynthModularInfo* SpiralSynthModularInfo::Get()
{
	if (!m_SpiralSynthModularInfo)
	{
		m_SpiralSynthModularInfo = new SpiralSynthModularInfo;
	}

	return m_SpiralSynthModularInfo;
}

SpiralSynthModularInfo::SpiralSynthModularInfo()
{
	BGIMG="None";
	PLUGIN_PATH = PLUGIN_PATH_LOCATION;
}

void SpiralSynthModularInfo::StreamInPrefs(istream &s)
{
	// call base class
	SpiralInfo::StreamInPrefs(s);

	char temp[256];
	s>>temp>>temp;
	s>>temp>>temp>>GUICOL_Tool;
	s>>temp>>temp>>GUICOL_Button;
	s>>temp>>temp>>GUICOL_Canvas;
	s>>temp>>temp>>GUICOL_Device;
	s>>temp>>temp>>BGIMG;
	s>>temp>>temp>>PLUGIN_PATH;
	s>>temp>>temp;
	string st;

	PLUGINVEC.clear();

	if (USEPLUGINLIST)
	{
		while(st!="end" && !s.eof())
		{
			s>>st;
			if (st!="end") PLUGINVEC.push_back(st);
		}
	}
#if __APPLE__
	// ignore custom paths, plugins are encapsulated in the app anyway
	// this prevents the program to fail if the user move the application icon
	PLUGIN_PATH = PLUGIN_PATH_LOCATION;
#endif
}

void SpiralSynthModularInfo::StreamOutPrefs(ostream &s)
{
	// call base class
	SpiralInfo::StreamOutPrefs(s);

	s<<endl<<"SpiralSynthModular Info:"<<endl<<endl;

	s<<"ToolBoxColour     = "<<GUICOL_Tool<<endl;
	s<<"ButtonColour      = "<<GUICOL_Button<<endl;
	s<<"CanvasColour      = "<<GUICOL_Canvas<<endl;
	s<<"DeviceColour      = "<<GUICOL_Device<<endl;
	s<<"BGImagePNG        = "<<BGIMG<<endl<<endl;

	s<<"PluginPath        = "<<PLUGIN_PATH<<endl<<endl;

	s<<"PluginList        = "<<endl;
	for (vector<string>::iterator i=PLUGINVEC.begin();
		 i!=PLUGINVEC.end(); i++)
	{
		s<<*i<<endl;
	}
    s<<"end"<<endl;
}
