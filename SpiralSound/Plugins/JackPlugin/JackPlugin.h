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
#include "../../RiffWav.h"
#include <FL/Fl_Pixmap.H>
#include <map>
#include <glib.h>
#include <jack/jack.h>

typedef jack_default_audio_sample_t sample_t;

#ifndef JackPLUGIN
#define JackPLUGIN

const int NUM_INPUTS  = 8;
const int NUM_OUTPUTS = 8;

class JackClient
{
public:
	static JackClient *Get()      { if(!m_Singleton) m_Singleton=new JackClient; return m_Singleton; }
	static void PackUpAndGoHome() { if(m_Singleton)  { delete m_Singleton; m_Singleton=NULL; } }
	
	bool   Attach();
	void   Detach();
	bool   IsAttached()                   { return m_Attached; }
	void   SetCallback(void(*Run)(void*, bool m),void *Context) { RunCallback=Run; RunContext=Context; }					
	void   GetPortNames(vector<string> &InputNames,vector<string> &OutputNames);
	void   ConnectInput(int n, const string &JackPort);
	void   ConnectOutput(int n, const string &JackPort);
	string GetInputName(int ID)           { return m_InputPortMap[ID]->Name; }
	string GetOutputName(int ID)          { return m_OutputPortMap[ID]->Name; }
	void   SetInputBuf(int ID, float* s);
	void   SetOutputBuf(int ID, float* s);

protected:
	JackClient();
	~JackClient();
	
	static int  Process(long unsigned int n, void *o);
	static int  OnBufSizeChange(long unsigned int n, void *o);
	static int  OnSRateChange(long unsigned int n, void *o);
	static void OnJackShutdown(void *o);

private:

	class JackPort
	{		
		public:
		JackPort::JackPort() :
			Connected(false),Buf(NULL),Port(NULL) {}
		
		string         Name;
		bool           Connected;
		float*         Buf;
		jack_port_t*   Port;
		string         ConnectedTo;
	};

	static JackClient*        m_Singleton;
	static jack_client_t*     m_Client;
	static map<int,JackPort*> m_InputPortMap;	
	static map<int,JackPort*> m_OutputPortMap;	
	static long unsigned int  m_BufferSize;
	static long unsigned int  m_SampleRate;	
	static bool               m_Attached;
	
	static void(*RunCallback)(void*, bool m);
	static void *RunContext;
};

///////////////////////////////////////////////////

class JackPlugin : public SpiralPlugin
{
public:
 	JackPlugin();
	virtual ~JackPlugin();

	virtual PluginInfo& Initialise(const HostInfo *Host);
	virtual SpiralGUIType*  CreateGUI();
	virtual void 		Execute();
	virtual void	    StreamOut(ostream &s) {}
	virtual void	    StreamIn(istream &s)  {}
	
	// has to be defined in the plugin	
	virtual void UpdateGUI() { Fl::check(); }
		
	void Attach() { JackClient::Get()->Attach(); }
	void Detach() { JackClient::Get()->Detach(); }
	void GetPortNames(vector<string> &InputNames,vector<string> &OutputNames) { JackClient::Get()->GetPortNames(InputNames,OutputNames); }
	void ConnectInput(int n, const string &JackPort)  { JackClient::Get()->ConnectInput(n,JackPort); }
	void ConnectOutput(int n, const string &JackPort) { JackClient::Get()->ConnectOutput(n,JackPort); }
private:
	static int m_RefCount;
	static int m_NoExecuted;
};

#endif
