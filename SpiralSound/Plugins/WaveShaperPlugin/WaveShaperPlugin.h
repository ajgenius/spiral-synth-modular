/*  WaveShaper Plugin Copyleft (C) 2001 Yves Usson 
 *  for SpiralSynthModular
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

#ifndef WaveShaperPLUGIN
#define WaveShaperPLUGIN

class WaveShaperPlugin : public SpiralPlugin
{
public:
 	WaveShaperPlugin();
	virtual ~WaveShaperPlugin();
	
	virtual PluginInfo& Initialise(const HostInfo *Host);
	virtual SpiralGUIType*  CreateGUI();
	virtual void 		Execute();
	virtual void	    StreamOut(ostream &s);
	virtual void	    StreamIn(istream &s);
		
	// has to be defined in the plugin	
	virtual void UpdateGUI() { Fl::check(); }

	void Calc(void);
	float Get(int);
	void SetCoef(int,float);
	void SetWaveType(int);
	float GetCoef(int);
	int GetWaveType(void);
private:
	float *wt;
	float m_Coefs[6];
	int m_Wave;
	void set(int,float);
	friend istream &operator>>(istream &s, WaveShaperPlugin &o);
	friend ostream &operator<<(ostream &s, WaveShaperPlugin &o);
};

istream &operator>>(istream &s, WaveShaperPlugin &o);
ostream &operator<<(ostream &s, WaveShaperPlugin &o);

#endif
