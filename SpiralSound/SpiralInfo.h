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
#include <string>
#include <stdlib.h>
#include "Sample.h"

#ifndef SpiralINFO
#define SpiralINFO

using namespace std;

// todo: better place for these util funcs
float RandFloat(float s=0.0f, float e=1.0f);

// Loads info from the resource file, overload and implement
// as singleton for each app. App specifc info may be added
// after the standard items.

class SpiralInfo
{
public:

    void LoadPrefs();
	void SavePrefs();
	
	static void Alert(string Text);
	static void Log(string Text);
	
	static int    BUFSIZE;
	static int    FRAGSIZE;
	static int    FRAGCOUNT;
	static int    SAMPLERATE;
	static long   MAXSAMPLE;
	static float  VALUECONV;
	static bool   WANTMIDI;
	static int    FILTERGRAN;
	static string OUTPUTFILE;
	static string MIDIFILE;
	static bool   USEPLUGINLIST;
	static int    POLY;
	static string LOCALE;
        static unsigned GUI_COLOUR;
        static unsigned SCOPE_BG_COLOUR;
        static unsigned SCOPE_FG_COLOUR;
        static unsigned SCOPE_SEL_COLOUR;
        static unsigned SCOPE_IND_COLOUR;
        static unsigned SCOPE_MRK_COLOUR;

        string GetHomeDir() {return m_HomeDir;}

protected:
	SpiralInfo() : m_HomeDir(getenv("HOME")), m_Version(1) {}
	virtual ~SpiralInfo() {}

	virtual string GetResFileName() { return "Spiralrc"; }
	
	virtual void StreamInPrefs(istream &s);
	virtual void StreamOutPrefs(ostream &s);
	
	string m_HomeDir;
	int m_Version;
};

#endif
