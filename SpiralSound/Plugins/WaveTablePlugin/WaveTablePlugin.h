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

#include "../SpiralPlugin.h"
#include <FL/Fl_Pixmap.H>

#ifndef WaveTablePLUGIN
#define WaveTablePLUGIN

static const int NUM_TABLES = 8;
static const int DEFAULT_TABLE_LEN = 1024;

class WaveTablePlugin : public SpiralPlugin
{
public:
 	WaveTablePlugin();
	virtual ~WaveTablePlugin();

	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType  *CreateGUI();
	virtual void 		Execute();
	virtual void	    StreamOut(ostream &s);
	virtual void	    StreamIn(istream &s);
	
	// has to be defined in the plugin	
	virtual void UpdateGUI() { Fl::check(); }

	enum Type{SINE,SQUARE,SAW,REVSAW,TRIANGLE,PULSE1,PULSE2,INVSINE};
	
	void WriteWaves();
	
	void NoteTrigger(int V,int s,int v);
	void SetOctave(int o) {m_Octave=o;}
	void SetFineFreq(float s) {m_FineFreq=s;}
	void SetType(Type t) {m_Type=t;}
	void SetModAmount(float s) {m_ModAmount=s;}
	int   GetOctave() {return m_Octave;}
	float GetFineFreq() {return m_FineFreq;}
	Type  GetType() {return m_Type;}
	float GetModAmount() {return m_ModAmount;}
	
private:

	// Voice specific parameter
	int   m_Note;
	float m_CyclePos;
	
	// Common voice parameters
	Type   m_Type;
	int    m_Octave;
	float  m_FineFreq;
	float  m_ModAmount;
		
	Sample m_Table[NUM_TABLES];
	int    m_TableLength;
	
	friend istream &operator>>(istream &s, WaveTablePlugin &o);
	friend ostream &operator<<(ostream &s, WaveTablePlugin &o);
};

istream &operator>>(istream &s, WaveTablePlugin &o);
ostream &operator<<(ostream &s, WaveTablePlugin &o);

#endif
