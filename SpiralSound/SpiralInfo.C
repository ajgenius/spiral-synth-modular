/*  SpiralSound
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

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "FL/fl_draw.h"
#include "FL/fl_message.h"

#include "SpiralInfo.h"

float RandFloat(float s, float e)
{
	return s+((rand()%10000/10000.0)*(e-s));
}

void SpiralInfo::LoadPrefs()
{
	string rcfn(m_HomeDir+"/"+GetResFileName());
	ifstream i(rcfn.c_str());
	
	if (!i)
	{
		cerr<<"Creating "<<rcfn<<endl;
		SavePrefs();
		return;
	}
		
	StreamInPrefs(i);
}

void SpiralInfo::SavePrefs()
{
	string rcfn(m_HomeDir+"/"+GetResFileName());
	ofstream o(rcfn.c_str());
	
	StreamOutPrefs(o);
}	

void SpiralInfo::StreamInPrefs(istream &s)
{
	char temp[256];
	s>>temp>>temp>>temp;
	s>>temp>>temp>>m_Version;
	s>>temp>>temp>>LOCALE;
	s>>temp>>temp>>BUFSIZE;
	s>>temp>>temp>>FRAGSIZE;
	s>>temp>>temp>>FRAGCOUNT;
	s>>temp>>temp>>SAMPLERATE;
	s>>temp>>temp>>WANTMIDI;
	s>>temp>>temp>>FILTERGRAN;
	s>>temp>>temp>>OUTPUTFILE;
	s>>temp>>temp>>MIDIFILE;
	s>>temp>>temp>>REALTIMEOUT;
	s>>temp>>temp>>POLY;
	//s>>temp>>temp>>GUI_COLOUR;
	//s>>temp>>temp>>GUIBG_COLOUR;
	//s>>temp>>temp>>GUIBG2_COLOUR;	
}

void SpiralInfo::StreamOutPrefs(ostream &s)
{
	s<<"SpiralSound resource file"<<endl<<endl;
	s<<"Version           = "<<m_Version<<endl;	
	s<<"Locale            = "<<LOCALE<<endl;	
	s<<"BufferSize        = "<<BUFSIZE<<endl;	
	s<<"FragmentSize      = "<<FRAGSIZE<<endl;	
	s<<"FragmentCount     = "<<FRAGCOUNT<<endl;	
	s<<"Samplerate        = "<<SAMPLERATE<<endl;
	s<<"WantMidi          = "<<WANTMIDI<<endl;
	s<<"FilterGranularity = "<<FILTERGRAN<<endl;
	s<<"Output            = "<<OUTPUTFILE<<endl;
	s<<"Midi              = "<<MIDIFILE<<endl;
	s<<"WantRealtimeOut   = "<<REALTIMEOUT<<endl;
	s<<"Polyphony         = "<<POLY<<endl;
	//s<<"GUIColour         = "<<GUI_COLOUR<<endl;
	//s<<"GUIBGColour       = "<<GUIBG_COLOUR<<endl;
	//s<<"GUIBG2Colour      = "<<GUIBG2_COLOUR<<endl;
}

void SpiralInfo::Alert(string Text)
{
	fl_message(Text.c_str());
}

void SpiralInfo::Log(string Text)
{

}
