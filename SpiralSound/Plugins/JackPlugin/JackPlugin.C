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

#include <stdio.h>
#include <limits.h>

#include "JackPlugin.h"
#include "JackPluginGUI.h"
#include "SpiralIcon.xpm"

static const HostInfo* host;
JackClient*       JackClient::m_Singleton  = NULL;
bool              JackClient::m_Attached   = false;
long unsigned int JackClient::m_BufferSize = 0;
long unsigned int JackClient::m_SampleRate = 0;
void            (*JackClient::RunCallback)(void*, bool m)=NULL;
void             *JackClient::RunContext   = NULL;	
jack_client_t    *JackClient::m_Client     = NULL;
map<int,JackClient::JackPort*> JackClient::m_InputPortMap;	
map<int,JackClient::JackPort*> JackClient::m_OutputPortMap;	

int               JackPlugin::m_RefCount   = 0;
int               JackPlugin::m_NoExecuted = 0;

///////////////////////////////////////////////////////

JackClient::JackClient()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////

JackClient::~JackClient()	
{	
	Detach();
}

/////////////////////////////////////////////////////////////////////////////////////////////

bool JackClient::Attach()
{
	if (m_Attached) return true;

	if (!(m_Client = jack_client_new("SSM"))) 
	{
		cerr<<"jack server not running?"<<endl;
		return false;
	}

	jack_set_process_callback(m_Client, JackClient::Process, 0);
	jack_set_buffer_size_callback(m_Client, JackClient::OnBufSizeChange, 0);
	jack_set_sample_rate_callback (m_Client, JackClient::OnSRateChange, 0);
	jack_on_shutdown (m_Client, JackClient::OnJackShutdown, this);

	m_InputPortMap.clear();
	m_OutputPortMap.clear();

	// create the ports 
	for (int n=0; n<NUM_INPUTS; n++)
	{
		char Name[256];
		sprintf(Name,"In %d",n);
	
		JackPort *NewPort = new JackPort;
		NewPort->Name=Name;
		NewPort->Buf=NULL;		
		NewPort->Port = jack_port_register (m_Client, Name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
		m_InputPortMap[n]=NewPort;
	}

	for (int n=0; n<NUM_OUTPUTS; n++)
	{
		char Name[256];
		sprintf(Name,"Out %d",n);
	
		JackPort *NewPort = new JackPort;
		NewPort->Name=Name;
		NewPort->Buf=NULL;		
		NewPort->Port = jack_port_register (m_Client, Name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
		m_OutputPortMap[n]=NewPort;
	}

	// tell the JACK server that we are ready to roll 
	if (jack_activate (m_Client)) 
	{
		cerr<<"cannot activate client"<<endl;
		return false;
	}
	
	m_Attached=true;
	
	cerr<<"connected to jack..."<<endl;
		
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void JackClient::Detach()
{
	if (m_Client)
	{
		cerr<<"Detaching from JACK"<<endl;
		jack_client_close(m_Client);
		m_Client=NULL;
		m_Attached=false;
	}
	
	// tells ssm to go back to non callback mode
	RunCallback(RunContext, false);
}

/////////////////////////////////////////////////////////////////////////////////////////////

int JackClient::Process(jack_nframes_t nframes, void *o)
{	
	for (int n=0; n<NUM_INPUTS; n++)
	{
		if (jack_port_connected(m_InputPortMap[n]->Port))
		{
			sample_t *in = (sample_t *) jack_port_get_buffer(m_InputPortMap[n]->Port, nframes);
			memcpy (m_InputPortMap[n]->Buf, in, sizeof (sample_t) * m_BufferSize);
		}			
	}
	
	for (int n=0; n<NUM_OUTPUTS; n++)
	{
		//if (m_OutputPortMap[n]->Connected)
		{
			if (m_OutputPortMap[n]->Buf)
			{ 
				sample_t *out = (sample_t *) jack_port_get_buffer(m_OutputPortMap[n]->Port, nframes);
				memcpy (out, m_OutputPortMap[n]->Buf, sizeof (sample_t) * m_BufferSize);
			}
			else // no output availible, clear
			{ 
				sample_t *out = (sample_t *) jack_port_get_buffer(m_OutputPortMap[n]->Port, nframes);
				memset (out, 0, sizeof (sample_t) * m_BufferSize);
			}
		}
	}
		
	if(RunCallback&&RunContext) 
	{
		// do the work
		RunCallback(RunContext, true);
	}
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

int JackClient::OnBufSizeChange(long unsigned int n, void *o)
{
	m_BufferSize=n;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

int JackClient::OnSRateChange(long unsigned int n, void *o)
{
	m_SampleRate=n;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void JackClient::OnJackShutdown(void *o)
{
	cerr<<"Shutdown"<<endl;
	m_Attached=false;
	// tells ssm to go back to non callback mode
	RunCallback(RunContext, false);
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void JackClient::GetPortNames(vector<string> &InputNames, vector<string> &OutputNames)
{
	InputNames.clear();
	OutputNames.clear();

	if (!m_Attached) return;

	//Outputs first
	const char **PortNameList=jack_get_ports(m_Client,NULL,NULL,JackPortIsOutput);	
	
	int n=0;
	while(PortNameList[n]!=NULL)
	{		
		OutputNames.push_back(PortNameList[n]);
		n++;
	}	
	
	delete PortNameList;
	
	//Inputs second
	PortNameList=jack_get_ports(m_Client,NULL,NULL,JackPortIsInput);
	
	n=0;
	while(PortNameList[n]!=NULL)
	{		
		InputNames.push_back(PortNameList[n]);
		n++;
	}
	
	delete PortNameList;		
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Input means input of SSM, so this connects jack sources to the plugin outputs
void JackClient::ConnectInput(int n, const string &JackPort)
{
	if (!IsAttached()) return;

	cerr<<"JackClient::ConnectInput: connecting source ["<<JackPort<<"] to dest ["<<m_InputPortMap[n]->Name<<"]"<<endl;

	if (m_InputPortMap[n]->ConnectedTo!="")
	{
		if (jack_disconnect (m_Client, m_InputPortMap[n]->ConnectedTo.c_str(), jack_port_name(m_InputPortMap[n]->Port))) 
			cerr<<"JackClient::ConnectInput: cannot disconnect input port ["
				<<m_InputPortMap[n]->ConnectedTo<<"] from ["<<m_InputPortMap[n]->Name<<"]"<<endl;
	}
	
	m_InputPortMap[n]->ConnectedTo = JackPort;
		
	if (jack_connect (m_Client, JackPort.c_str(), jack_port_name(m_InputPortMap[n]->Port))) 
		cerr<<"JackClient::ConnectInput: cannot connect input port ["
			<<JackPort<<"] to ["<<m_InputPortMap[n]->Name<<"]"<<endl;
			
	m_InputPortMap[n]->Connected=true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Output means output of SSM, so this connects plugin inputs to a jack destination
void JackClient::ConnectOutput(int n, const string &JackPort)
{
	if (!IsAttached()) return;
	cerr<<"JackClient::ConnectOutput: connecting source ["<<m_OutputPortMap[n]->Name<<"] to dest ["<<JackPort<<"]"<<endl;

	if (m_OutputPortMap[n]->ConnectedTo!="")
	{
		if (jack_disconnect (m_Client, jack_port_name(m_OutputPortMap[n]->Port), m_OutputPortMap[n]->ConnectedTo.c_str())) 
			cerr<<"JackClient::ConnectOutput: cannot disconnect output port ["
				<<m_OutputPortMap[n]->ConnectedTo<<"] from ["<<m_OutputPortMap[n]->Name<<"]"<<endl;
	}
	
	m_OutputPortMap[n]->ConnectedTo = JackPort;
	if (jack_connect (m_Client, jack_port_name(m_OutputPortMap[n]->Port), JackPort.c_str()))
		cerr<<"JackClient::ConnectOutput: cannot connect output port ["
			<<m_OutputPortMap[n]->Name<<"] to ["<<JackPort<<"]"<<endl;
	m_OutputPortMap[n]->Connected=true; 
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Input means input of SSM, so this connects jack sources to the plugin outputs
void JackClient::DisconnectInput(int n)
{
	if (!IsAttached()) return;
	cerr<<"JackClient::DisconnectInput: Disconnecting input "<<n<<endl;

	if (m_InputPortMap[n]->ConnectedTo!="")
	{
		if (jack_disconnect (m_Client, m_InputPortMap[n]->ConnectedTo.c_str(), jack_port_name(m_InputPortMap[n]->Port))) 
			cerr<<"JackClient::ConnectInput: cannot disconnect input port ["
				<<m_InputPortMap[n]->ConnectedTo<<"] from ["<<m_InputPortMap[n]->Name<<"]"<<endl;
	}

	m_InputPortMap[n]->Connected=false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Output means output of SSM, so this connects plugin inputs to a jack destination
void JackClient::DisconnectOutput(int n)
{
	if (!IsAttached()) return;
	cerr<<"JackClient::DisconnectInput: Disconnecting input "<<n<<endl;

	if (m_OutputPortMap[n]->ConnectedTo!="")
	{
		if (jack_disconnect (m_Client, jack_port_name(m_OutputPortMap[n]->Port), m_OutputPortMap[n]->ConnectedTo.c_str())) 
			cerr<<"JackClient::ConnectOutput: cannot disconnect output port ["
				<<m_OutputPortMap[n]->ConnectedTo<<"] from ["<<m_OutputPortMap[n]->Name<<"]"<<endl;
	}

	m_OutputPortMap[n]->Connected=false;
}
/////////////////////////////////////////////////////////////////////////////////////////////

void JackClient::SetInputBuf(int ID, float* s)  
{ 
	if(m_InputPortMap.find(ID)!=m_InputPortMap.end()) m_InputPortMap[ID]->Buf=s; 
} 

/////////////////////////////////////////////////////////////////////////////////////////////
	
void JackClient::SetOutputBuf(int ID, float* s) 
{ 
	if(m_OutputPortMap.find(ID)!=m_OutputPortMap.end()) m_OutputPortMap[ID]->Buf=s; 
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
SpiralPlugin* CreateInstance()
{
	return new JackPlugin;
}

char** GetIcon()
{	
	return SpiralIcon_xpm;
}

int GetID()
{
	return 31;
}
}

///////////////////////////////////////////////////////

JackPlugin::JackPlugin() :
m_UpdateNames(false),
m_Connected(false)
{
	m_RefCount++;
	
	m_PluginInfo.Name="Jack";
	m_PluginInfo.Width=200;
	m_PluginInfo.Height=325;
	m_PluginInfo.NumInputs=NUM_OUTPUTS;
	m_PluginInfo.NumOutputs=NUM_INPUTS;
	
	for (int n=0; n<NUM_OUTPUTS; n++)
	{
		char Temp[256];
		sprintf(Temp,"SSM Output %d",n);
		m_PluginInfo.PortTips.push_back(Temp);
	}
	
	for (int n=0; n<NUM_INPUTS; n++)
	{
		char Temp[256];
		sprintf(Temp,"SSM Input %d",n);
		m_PluginInfo.PortTips.push_back(Temp);
	}	
	
	m_AudioCH->Register("Num",&m_GUIArgs.Num);
	m_AudioCH->RegisterData("Port",ChannelHandler::INPUT,&m_GUIArgs.Port,sizeof(m_GUIArgs.Port));
	m_AudioCH->Register("NumInputPortNames",&m_NumInputPortNames,ChannelHandler::OUTPUT);
	m_AudioCH->Register("NumOutputPortNames",&m_NumOutputPortNames,ChannelHandler::OUTPUT);
	m_AudioCH->RegisterData("InputPortNames",ChannelHandler::OUTPUT,&m_InputPortNames,sizeof(m_InputPortNames));
	m_AudioCH->RegisterData("OutputPortNames",ChannelHandler::OUTPUT,&m_OutputPortNames,sizeof(m_OutputPortNames));
	m_AudioCH->Register("UpdateNames",&m_UpdateNames,ChannelHandler::OUTPUT);	
	m_AudioCH->Register("Connected",&m_Connected,ChannelHandler::OUTPUT);	
}

JackPlugin::~JackPlugin()
{
	m_RefCount--;
	if (m_RefCount==0)
	{ 
		JackClient::Get()->Detach();
		JackClient::PackUpAndGoHome();
	}
}

PluginInfo &JackPlugin::Initialise(const HostInfo *Host)
{	
	PluginInfo& Info= SpiralPlugin::Initialise(Host);
	host=Host;		
	
	JackClient::Get()->SetCallback(cb_Update,m_Parent);	

	return Info;
}

SpiralGUIType *JackPlugin::CreateGUI()
{
	return new JackPluginGUI(m_PluginInfo.Width,
							  m_PluginInfo.Height,
						      this,m_AudioCH,m_HostInfo);
}

void JackPlugin::Execute()
{

}

void JackPlugin::ExecuteCommands()
{
	// we want to process this whether we are connected to stuff or not
	JackClient* pJack=JackClient::Get();
	
	// connect the buffers up if we are plugged into something		
	for (int n=0; n<NUM_OUTPUTS; n++)
	{
		if (InputExists(n)) 
		{			
			pJack->SetOutputBuf(n,(float*)GetInput(n)->GetBuffer());		
		}
		else 
		{	
			pJack->SetOutputBuf(n,NULL);
		}
	}
	
	for (int n=0; n<NUM_INPUTS; n++)
	{
		pJack->SetInputBuf(n,(float*)GetOutputBuf(n)->GetBuffer());		
	}

	if (m_AudioCH->IsCommandWaiting())
	{
		switch (m_AudioCH->GetCommand())
		{
		  /*case ATTACH : Attach(); break;
			case DETACH : Detach(); break;
			case CONNECTINPUT  : pJack->ConnectInput(m_GUIArgs.Num,m_GUIArgs.Port);  break;
			case CONNECTOUTPUT : pJack->ConnectOutput(m_GUIArgs.Num,m_GUIArgs.Port); break; */
			case UPDATE_NAMES :
			{
				int c=0;
	
			    vector<string> InputNames,OutputNames;
				GetPortNames(InputNames,OutputNames);
				for (vector<string>::iterator i=InputNames.begin();
					 i!=InputNames.end(); ++i)
				{
					strcpy(m_InputPortNames[c],i->c_str());
					c++;
				}
		
				c=0;
		
				for (vector<string>::iterator i=OutputNames.begin();
					 i!=OutputNames.end(); ++i)
				{
					strcpy(m_OutputPortNames[c],i->c_str());
					c++;
				}
		
				m_NumInputPortNames=InputNames.size();
				m_NumOutputPortNames=OutputNames.size();
			}
			break;
			default : break;
		}
	}
	m_Connected=JackClient::Get()->IsAttached();	
}
