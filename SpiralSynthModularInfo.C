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

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <dlfcn.h>

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
bool   SpiralInfo::REALTIMEOUT = true;

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

	// default plugins + path, check these before dist...
	// this is one of the two plugin lists, th other is in
	// the SpiralSound/Plugins/PluginList.txt for the
	// configure/make/install scripts

	PLUGIN_PATH = PLUGIN_PATH_LOCATION;

#ifdef SSM_EXPLICIT_PLUGIN_LIST
	PLUGINVEC.push_back("OutputPlugin.so");
	PLUGINVEC.push_back("DiskWriterPlugin.so");
	PLUGINVEC.push_back("ScopePlugin.so");
	PLUGINVEC.push_back("MidiPlugin.so");
	PLUGINVEC.push_back("KeyboardPlugin.so");
	PLUGINVEC.push_back("ControllerPlugin.so");
	PLUGINVEC.push_back("MatrixPlugin.so");
	PLUGINVEC.push_back("SeqSelectorPlugin.so");
	PLUGINVEC.push_back("SequencerPlugin.so");
	PLUGINVEC.push_back("PoshSamplerPlugin.so");
	PLUGINVEC.push_back("WaveTablePlugin.so");
	PLUGINVEC.push_back("OscillatorPlugin.so");
	PLUGINVEC.push_back("LFOPlugin.so");
	PLUGINVEC.push_back("NoisePlugin.so");
	PLUGINVEC.push_back("EnvelopePlugin.so");
	PLUGINVEC.push_back("SampleHoldPlugin.so");
	PLUGINVEC.push_back("NoteSnapPlugin.so");
	PLUGINVEC.push_back("MixerPlugin.so");
	PLUGINVEC.push_back("StereoMixerPlugin.so");
	PLUGINVEC.push_back("AmpPlugin.so");
	PLUGINVEC.push_back("RingModPlugin.so");
	PLUGINVEC.push_back("FilterPlugin.so");
	PLUGINVEC.push_back("SVFilterPlugin.so");	
	PLUGINVEC.push_back("MoogFilterPlugin.so");	
	PLUGINVEC.push_back("FormantPlugin.so");
	PLUGINVEC.push_back("AnotherFilterPlugin.so");
	PLUGINVEC.push_back("EchoPlugin.so");	
	PLUGINVEC.push_back("DelayPlugin.so");	
	PLUGINVEC.push_back("EnvFollowerPlugin.so");	
	PLUGINVEC.push_back("SmoothPlugin.so");
	PLUGINVEC.push_back("LADSPAPlugin.so");	
	PLUGINVEC.push_back("XFadePlugin.so");
	PLUGINVEC.push_back("DistributorPlugin.so");
	PLUGINVEC.push_back("SplitterPlugin.so");
	PLUGINVEC.push_back("StreamPlugin.so");
	PLUGINVEC.push_back("OperatorPlugin.so");
	PLUGINVEC.push_back("CounterPlugin.so");
	PLUGINVEC.push_back("FlipflopPlugin.so");
	PLUGINVEC.push_back("SwitchPlugin.so");
	PLUGINVEC.push_back("BeatMatchPlugin.so");
	PLUGINVEC.push_back("LogicPlugin.so");
#else
	// Scan plugin path for plugins.
	DIR *dp;
	struct dirent *ep;
	struct stat sb;
	void *handle;
	string fullpath;
	const char *path = PLUGIN_PATH.c_str();

	dp = opendir(path);
	if (!dp) {
		cerr << "WARNING: Could not open path " << path << endl;
	} else {
		while ((ep = readdir(dp))) {

		// Need full path
			fullpath = path;
			fullpath.append(ep->d_name);

		// Stat file to get type
			if (!stat(fullpath.c_str(), &sb)) {

			// We only want regular files
				if (S_ISREG(sb.st_mode)) {

				// We're not fussed about resolving symbols yet, since we are just
				// checking if it's a DLL.
					handle = dlopen(fullpath.c_str(), RTLD_LAZY);

					if (!handle) {
						cerr << "WARNING: File " << path << ep->d_name
							<< " could not be examined" << endl;
						cerr << "dlerror() output:" << endl;
						cerr << dlerror() << endl;
					} else {

					// It's a DLL. Add name to list
						PLUGINVEC.push_back(ep->d_name);
					}
				}
			}
		}
	}
#endif
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

#ifdef SSM_EXPLICIT_PLUGIN_LIST
	PLUGINVEC.clear();

	while(st!="end" && !s.eof())
	{
		s>>st;
		if (st!="end") PLUGINVEC.push_back(st);
	}
#endif
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
