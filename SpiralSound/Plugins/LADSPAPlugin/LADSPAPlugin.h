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
#include "ladspa.h"

#ifndef LADSPAPLUGIN
#define LADSPAPLUGIN

static const unsigned int NUM_PORTS = 8;

class LADSPAPlugin : public SpiralPlugin
{
public:
 	LADSPAPlugin();
	virtual ~LADSPAPlugin();
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void StreamOut(ostream &s);
	virtual void StreamIn(istream &s);
	
	// has to be defined in the plugin	
	virtual void UpdateGUI() { Fl::check(); }
	
	void SetGain(float s) { m_Gain=s; }
	float GetGain() { return m_Gain; }
	void SetMin(int n,float min) { m_PortMin[n]=min; }
	void SetMax(int n,float max) { m_PortMax[n]=max; }
	void SetPortClamp(int n,bool i) { m_PortClamp[n]=i; }
	void SetAmped(bool s) { m_Amped=s; }
	bool GetAmped() { return m_Amped; }
	
	bool UpdatePlugin(const char * filename, const char * label, bool PortClampReset=true);
	const LADSPA_Descriptor * GetPlugDesc(void) { return PlugDesc; }
	
private:
	void * PlugHandle;
	const LADSPA_Descriptor * PlugDesc;
	
	vector<LADSPA_Data*> m_LADSPABufVec;
	LADSPA_Handle PlugInstHandle;
	vector<int> m_PortID;
	vector<float> m_PortMin;
	vector<float> m_PortMax;	
	vector<bool> m_PortClamp;
	
	float m_Gain;
	bool m_Amped;
};

#endif
