/*  SpiralSound
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

#include <vector>
#include <string>
#include <iostream>
#include "../Sample.h"
#include <math.h>
#include <FL/Fl.h>
#include <FL/Fl_Window.h>

#ifndef SPIRALPLUGIN
#define SPIRALPLUGIN

#define SpiralGUIType Fl_Group

static const float MAX_FREQ = 13000;

struct PluginInfo
{
	string Name;
	int Width;
	int Height;
	int NumInputs;
	int NumOutputs;
	vector<string> PortTips;
	vector<int> PortTypes;
	char BitMap[40][40][3]; 
};

struct HostInfo
{
	int    BUFSIZE;	
	int    FRAGSIZE;  
	int    FRAGCOUNT; 	
	int    SAMPLERATE;
	string OUTPUTFILE;
	string MIDIFILE;
	int    POLY;
	int    GUI_COLOUR;
};

class SpiralPlugin
{
public:
	SpiralPlugin();
	virtual ~SpiralPlugin();
		
	virtual PluginInfo& Initialise(const HostInfo *Host);
	virtual void        Execute()=0;
	virtual SpiralGUIType*  CreateGUI()=0;
	
	// stream the plugins state
	virtual void	    StreamOut(ostream &s)=0;
	virtual void	    StreamIn(istream &s)=0;
	
	// stuff here gets saved in filename_files directory
	// you must return true if this feature is used.
	virtual bool	    SaveExternalFiles(const string &Dir) { return false; }
	virtual void	    LoadExternalFiles(const string &Dir) {}

	const HostInfo*     GetHostInfo() { return m_HostInfo; }
	bool                GetOutput(unsigned int n, Sample **s);
	bool                SetInput(unsigned int n, const Sample *s);
	const Sample*       GetInput(unsigned int n) { return m_Input[n]; }
	string 				GetName() { return m_PluginInfo.Name; }
	
	bool ValueChanged();
	void SetValueChanged(bool s) { m_ValuesChanged=s; }

	void UpdatePluginInfoWithHost();
	void SetUpdateInfoCallback(int ID, void(*s)(int, void *));
	void SetUpdateCallback(void (*s)(void*,bool m)) { cb_Update=s; }
	void SetParent(void *s) { m_Parent=s; }
	void SetInPortType(PluginInfo &pinfo, int port, Sample::SampleType type);    
    void SetOutPortType(PluginInfo &pinfo, int port, Sample::SampleType type);    
	
protected:

	void  SetOutput(int n,int p, float s) 
		{ if (m_Output[n]) m_Output[n]->Set(p,s); }
		
	float GetInput(int n,int p)  
		{ if (m_Input[n]) return (*m_Input[n])[p]; else return 0.0; }
		
	void  SetOutputPitch(int n,int p, float s) 
		{ if (m_Output[n]) m_Output[n]->Set(p,(s/MAX_FREQ*2)-1.0f); }
		
	float GetInputPitch(int n,int p)  
		{ if (m_Input[n]) return ((*m_Input[n])[p]+1.0f)*MAX_FREQ/2; else return 0.0; }

	void  MixOutput(int n,int p, float s) 
		{ if (m_Output[n]) m_Output[n]->Set(p,s+(*m_Output[n])[p]); }
	
	bool InputExists(int n) { return m_Input[n]!=NULL; }
	bool OutputExists(int n) { return m_Output[n]!=NULL; }	

	void AddOutput();
	void RemoveOutput();
	void RemoveAllOutputs();

	void AddInput();
	void RemoveInput();
	void RemoveAllInputs();
		
	// the ID number assigned to us by ssm
	int GetID() { return m_HostID; }
	
	Sample* GetOutputBuf(int n) { return m_Output[n]; }	
	
	SpiralGUIType  *m_GUI;
	const HostInfo *m_HostInfo;
	PluginInfo      m_PluginInfo;
	bool 			m_ValuesChanged;
	int             m_Version;		
	
	// needed for jack
	void (*cb_Update)(void*o ,bool m);	
	void  *m_Parent;
	
private:

	vector<const Sample*> m_Input;
	vector<Sample*> m_Output;

	void (*UpdateInfo)(int n,void *);	
	int    m_HostID;	
};

#endif
