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
#include "LADSPAPlugin.h"
#include "LADSPAPluginGUI.h"
#include <stdio.h>
#include "SpiralIcon.xpm"
#include "utils.h"
#include <algorithm>

////////////////////////////////////////////

/* FIXME: No matter what, I can't let this as it!! */
static LADSPAPlugin * lg = NULL;

void describePluginLibrary(const char * pcFullFilename, void * pvPluginHandle,
					    LADSPA_Descriptor_Function pfDescriptorFunction) {
	const LADSPA_Descriptor * psDescriptor;
	long lIndex;
	unsigned long lPluginIndex;
	unsigned long lPortIndex;
	unsigned long lLength;
	LADSPA_PortRangeHintDescriptor iHintDescriptor;
	LADSPA_Data fBound;

#define testcond(c,s) { \
  if (!(c)) { \
    cerr << (s); \
    failure = 1; \
  } \
}
	for (lIndex = 0; (psDescriptor = pfDescriptorFunction(lIndex)) != NULL; lIndex++) {
		int failure = 0;
    		testcond(!LADSPA_IS_REALTIME(psDescriptor->Properties), "ERROR: PLUGIN MUST RUN REAL TIME.\n");
    		testcond(psDescriptor->instantiate, "ERROR: PLUGIN HAS NO INSTANTIATE FUNCTION.\n");
    		testcond(psDescriptor->connect_port, "ERROR: PLUGIN HAS NO CONNECT_PORT FUNCTION.\n");
		testcond(psDescriptor->run, "ERROR: PLUGIN HAS NO RUN FUNCTION.\n");
		testcond(!(psDescriptor->run_adding != 0 && psDescriptor->set_run_adding_gain == 0),
			    "ERROR: PLUGIN HAS RUN_ADDING FUNCTION BUT NOT SET_RUN_ADDING_GAIN.\n");
		testcond(!(psDescriptor->run_adding == 0 && psDescriptor->set_run_adding_gain != 0),
    			    "ERROR: PLUGIN HAS SET_RUN_ADDING_GAIN FUNCTION BUT NOT RUN_ADDING.\n");
    		testcond(psDescriptor->cleanup, "ERROR: PLUGIN HAS NO CLEANUP FUNCTION.\n");
    		testcond(!LADSPA_IS_INPLACE_BROKEN(psDescriptor->Properties),
			    "ERROR: THIS PLUGIN CANNOT USE IN-PLACE PROCESSING.\n");
    		testcond(psDescriptor->PortCount, "ERROR: PLUGIN HAS NO PORTS.\n");

		if (!failure) {
			LPluginInfo pi;
			pi.Filename = pcFullFilename;
			pi.Label = psDescriptor->Label;
			pi.Name = psDescriptor->Name;
			/* ARGH! I really can't stand this ugly hack */
			lg->m_LADSPAList.push_back(pi);
		} else {
			cerr << "Plugin ignored...\n\n";
		}
	}

	dlclose(pvPluginHandle);
}

void LADSPAPlugin::LoadPluginList(void)
{
	m_LADSPAList.clear();
	m_CurrentPlugin.Name = "";
	m_CurrentPlugin.Filename = "";
	m_CurrentPlugin.Label = "";

	lg = this;
	LADSPAPluginSearch(describePluginLibrary);
	lg = NULL;

	sort(m_LADSPAList.begin(), m_LADSPAList.end(), LPluginInfoSortAsc());
} 

////////////////////////////////////////////////////

extern "C" {
SpiralPlugin* CreateInstance()
{
	return new LADSPAPlugin;
}

char** GetIcon()
{
	return SpiralIcon_xpm;
}

int GetID()
{
	return 0x0016;
}
}

///////////////////////////////////////////////////////

LADSPAPlugin::LADSPAPlugin() : 
PlugHandle(0), 
PlugDesc(NULL),
m_Gain(1.0f),
m_Amped(false)
{
	m_Version=3;
	
	m_PluginInfo.Name="LADSPA";
	m_PluginInfo.Width=600;
	m_PluginInfo.Height=300;
	m_PluginInfo.NumInputs=0;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Nuffink yet");	
	
	m_AudioCH->Register("Gain",&m_Gain);
	m_AudioCH->Register("Amped",&m_Amped);
	m_AudioCH->RegisterData("Desc",ChannelHandler::OUTPUT,&PlugDesc,sizeof(PlugDesc));
	m_AudioCH->Register("Num",&m_GUIArgs.Num);
	m_AudioCH->Register("Value",&m_GUIArgs.Value);
	m_AudioCH->Register("Clamp",&m_GUIArgs.Clamp);
	m_AudioCH->RegisterData("Filename",ChannelHandler::INPUT,&m_GUIArgs.Filename,sizeof(m_GUIArgs.Filename));
	m_AudioCH->RegisterData("Label",ChannelHandler::INPUT,&m_GUIArgs.Label,sizeof(m_GUIArgs.Label));
	
	LoadPluginList();
}

LADSPAPlugin::~LADSPAPlugin()
{
}

PluginInfo &LADSPAPlugin::Initialise(const HostInfo *Host)
{
	PluginInfo& Info = SpiralPlugin::Initialise(Host);
	LADSPA_Data *NewPort = new LADSPA_Data[m_HostInfo->BUFSIZE];
	m_LADSPABufVec.push_back(NewPort);
	return Info; 
}

SpiralGUIType *LADSPAPlugin::CreateGUI()
{
	return new LADSPAPluginGUI(m_PluginInfo.Width,m_PluginInfo.Height,this,m_AudioCH,m_HostInfo,m_LADSPAList);
}

void LADSPAPlugin::Execute()
{
	if (PlugDesc)
	{
		// convert inputs if exist (zero if not)
		for (int n=0; n<m_PluginInfo.NumInputs; n++)
		{
			if (GetInput(n))
			{
				if (m_PortClamp[n]) {
					// scale input to match hinted range
					float Offset=m_PortMin[n];
					float Scale=m_PortMax[n]-m_PortMin[n];

					//cerr<<n<<" ["<<Scale<<"] ["<<Offset<<"]"<<endl;

					for (int i=0; i<m_HostInfo->BUFSIZE; i++)
					{
						m_LADSPABufVec[n][i]=Offset+(GetInput(n,i)*0.5f+0.5f)*Scale;
						//cerr<<Scale<<" "<<Offset<<" "<<m_LADSPABufVec[n][i]<<endl;
					}
				} else {
					// pass input as is
					for (int i=0; i<m_HostInfo->BUFSIZE; i++)
					{
						m_LADSPABufVec[n][i]=GetInput(n,i);
					}
				}
				// Update the GUI outputs with the first value in the buffer
				//((LADSPAPluginGUI*)m_GUI)->UpdatePortDisplay(n,m_LADSPABufVec[n][0]);
			}
			else // zero
			{
				for (int i=0; i<m_HostInfo->BUFSIZE; i++) m_LADSPABufVec[n][i]=0;
			}
		}
		
		// run plugin
   		PlugDesc->run(PlugInstHandle,m_HostInfo->BUFSIZE);
		
		// convert outputs
		for (int n=0; n<m_PluginInfo.NumOutputs; n++)
		{
			/*if (m_Amped)
			{
				for (int i=0; i<m_HostInfo->BUFSIZE; i++) 
				{
					SetOutput(n,i,m_LADSPABufVec[n+m_PluginInfo.NumInputs][i]*m_Gain*10);
				}
			}
			else*/
			{
				for (int i=0; i<m_HostInfo->BUFSIZE; i++) 
				{
					SetOutput(n,i,m_LADSPABufVec[n+m_PluginInfo.NumInputs][i]*m_Gain);
				}
			}
		}
	}
}

void LADSPAPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch(m_AudioCH->GetCommand())
		{
			case (SETMIN)       : SetMin(m_GUIArgs.Num,m_GUIArgs.Value); break;
			case (SETMAX)       : SetMax(m_GUIArgs.Num,m_GUIArgs.Value); break;
			case (SETCLAMP)     : SetPortClamp(m_GUIArgs.Num,m_GUIArgs.Clamp); break;
			case (UPDATEPLUGIN) : UpdatePlugin(m_GUIArgs.Num); break;
		};
	}
}

void LADSPAPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";
	
	switch (m_Version)
	{
		case 3:
		{
			s<<m_Gain<<" ";
			s<<m_CurrentPlugin.Filename<<" ";
			s<<m_CurrentPlugin.Label<<" ";
			s<<m_PortMin.size()<<" ";
			assert(m_PortMin.size()==m_PortMax.size());
			assert(m_PortMin.size()==m_PortClamp.size());
			for (vector<float>::iterator i=m_PortMin.begin();
				 i!=m_PortMin.end(); i++)
			{
				s<<*i<<" ";
			}
			for (vector<float>::iterator i=m_PortMax.begin();
				 i!=m_PortMax.end(); i++)
			{
				s<<*i<<" ";
			}
			for (vector<bool>::iterator i=m_PortClamp.begin();
				 i!=m_PortClamp.end(); i++)
			{
				s<<*i<<" ";
			}
		}
		break;
		case 2:
// Here for consistency - should never actually happen, as
// version is always 3!
		{
			s<<m_Gain<<" ";
			s<<m_CurrentPlugin.Filename<<" ";
			s<<m_CurrentPlugin.Label<<" ";
			s<<m_PortMin.size()<<" ";
			assert(m_PortMin.size()==m_PortMax.size());
			for (vector<float>::iterator i=m_PortMin.begin();
				 i!=m_PortMin.end(); i++)
			{
				s<<*i<<" ";
			}
			for (vector<float>::iterator i=m_PortMax.begin();
				 i!=m_PortMax.end(); i++)
			{
				s<<*i<<" ";
			}
		}
		break;

		case 1:
		{
			s<<m_Gain<<" ";
			s<<m_CurrentPlugin.Filename<<" ";
			s<<m_CurrentPlugin.Label<<" ";
		}
		break;
	}
}

void LADSPAPlugin::StreamIn(istream &s)
{
	int version;
	s>>version;

	switch (version)
	{
		case 3:
		{
			s>>m_Gain;

			string Filename,Label;
			s>>Filename>>Label;
			int PortCount;
			s>>PortCount;
			float min,max;
			bool clamp;

			for (int n=0; n<PortCount; n++)
			{
				s>>min;
				m_PortMin.push_back(min);
			}

			for (int n=0; n<PortCount; n++)
			{
				s>>max;
				m_PortMax.push_back(max);
			}
			for (int n=0; n<PortCount; n++)
			{
				s>>clamp;
				m_PortClamp.push_back(clamp);
			}

			if (Filename!="None")
			{
				UpdatePlugin(Filename.c_str(), Label.c_str(), false);
			}
			
			m_CurrentPlugin.Ports.reserve(PortCount);
			
			for (int n=0; n<PortCount; n++)
			{
				m_CurrentPlugin.Ports[n].Min=m_PortMin[n];
				m_CurrentPlugin.Ports[n].Max=m_PortMax[n];
				m_CurrentPlugin.Ports[n].Clamped=m_PortClamp[n];
			}
		}
		break;

		case 2:
		{
			s>>m_Gain;

			string Filename,Label;
			s>>Filename>>Label;
			int PortCount;
			s>>PortCount;
			float min,max;
			for (int n=0; n<PortCount; n++)
			{
				s>>min;
				m_PortMin.push_back(min);
			}

			for (int n=0; n<PortCount; n++)
			{
				s>>max;
				m_PortMax.push_back(max);
			}

			for (int n=0; n<PortCount; n++)
			{
			// Set PortClamp to true as default
				m_PortClamp.push_back(true);
			}

			if (Filename!="None")
			{
				UpdatePlugin(Filename.c_str(), Label.c_str(), false);
			}
			
			m_CurrentPlugin.Ports.reserve(PortCount);
			
			for (int n=0; n<PortCount; n++)
			{
				m_CurrentPlugin.Ports[n].Min=m_PortMin[n];
				m_CurrentPlugin.Ports[n].Max=m_PortMax[n];
				m_CurrentPlugin.Ports[n].Clamped=m_PortClamp[n];
			}
		}
		break;

		case 1:
		{
			s>>m_Gain;
	
			string Filename,Label;
			s>>Filename>>Label;
			
			if (Filename!="None")
			{
				UpdatePlugin(Filename.c_str(), Label.c_str());
			}
		}
		break;
	}
}

bool LADSPAPlugin::UpdatePlugin(int n)
{
	return UpdatePlugin(m_LADSPAList[n].Filename.c_str(),m_LADSPAList[n].Label.c_str());
}

bool LADSPAPlugin::UpdatePlugin(const char * filename, const char * label, bool PortClampReset=true)
{
	// first call with same info, to clear the ports
	UpdatePluginInfoWithHost();
	
	if (PlugHandle) {
		if (PlugDesc->deactivate) PlugDesc->deactivate(PlugInstHandle);
		PlugDesc->cleanup(PlugInstHandle);
		unloadLADSPAPluginLibrary(PlugHandle);
		PlugHandle = 0;
	}
	
	if ((PlugHandle = loadLADSPAPluginLibrary(filename))) {
		if (!(PlugDesc = findLADSPAPluginDescriptor(PlugHandle, filename, label))) {
			unloadLADSPAPluginLibrary(PlugHandle);
			PlugHandle = 0;
		} else {
			/* Now we can instantiate the LADSPA Plugin and wire it to the datas bytes */
			if (!(PlugInstHandle = PlugDesc->instantiate(PlugDesc, m_HostInfo->SAMPLERATE))) {
				cerr << "LADSPA Plugin error to instantiate...\n";
				unloadLADSPAPluginLibrary(PlugHandle);
				PlugDesc = 0;
				PlugHandle = 0;
				return 0;
			}
			
			m_PluginInfo.NumInputs=getPortCountByType(PlugDesc, LADSPA_PORT_INPUT);
			m_PluginInfo.NumOutputs=getPortCountByType(PlugDesc, LADSPA_PORT_OUTPUT);
			
/////////////////////////////////
// LADSPA Buffers

			for(vector<LADSPA_Data*>::iterator i=m_LADSPABufVec.begin();
				i!=m_LADSPABufVec.end(); i++)
			{
				if (*i) delete[] (*i);
			}
			m_LADSPABufVec.clear();						
			
			unsigned long c=0;			
			for (unsigned int n=0; n<PlugDesc->PortCount; n++)
			{
				if (LADSPA_IS_PORT_INPUT(PlugDesc->PortDescriptors[n]))
				{
					LADSPA_Data *NewPort = new LADSPA_Data[m_HostInfo->BUFSIZE];
					m_LADSPABufVec.push_back(NewPort);
					PlugDesc->connect_port(PlugInstHandle, n, m_LADSPABufVec[c]);
					m_PortID.push_back(n);															
					c++;
				}					
			}			
					
			for (unsigned int n=0; n<PlugDesc->PortCount; n++)
			{
				if (LADSPA_IS_PORT_OUTPUT(PlugDesc->PortDescriptors[n]))
				{
					LADSPA_Data *NewPort = new LADSPA_Data[m_HostInfo->BUFSIZE];
					m_LADSPABufVec.push_back(NewPort);
					PlugDesc->connect_port(PlugInstHandle, n, m_LADSPABufVec[c]);
					m_PortID.push_back(n);
					c++;
				}					
			}		
			
			// activate the plugin now
			if (PlugDesc->activate)
				PlugDesc->activate(PlugInstHandle);

/////////////////////////////////
// SSM Buffers

			// Clear i/o buffers
			RemoveAllInputs();
			RemoveAllOutputs();

			// Reallocate the i/o buffers required
			for (int n=0; n<m_PluginInfo.NumInputs; n++) AddInput();			
			for (int n=0; n<m_PluginInfo.NumOutputs; n++) AddOutput();
			
//////////////////////////////
// Update the GUI stuff

			m_CurrentPlugin.Name=PlugDesc->Name;
			m_CurrentPlugin.Maker=PlugDesc->Maker;
			m_CurrentPlugin.Filename=filename;
			m_CurrentPlugin.Label=label;
			
			m_CurrentPlugin.Ports.clear();
			m_PluginInfo.PortTips.clear();
			
			string desc;
			c=0;
			for (unsigned int i = 0; i < PlugDesc->PortCount; i++) 
			{
				if (LADSPA_IS_PORT_INPUT(PlugDesc->PortDescriptors[i])) 
				{
					desc = string(PlugDesc->PortNames[i]) +
					       (LADSPA_IS_PORT_CONTROL(PlugDesc->PortDescriptors[i]) ? " (CV)" : " (AU)");
					m_PluginInfo.PortTips.push_back(desc.c_str());
							
					LPluginInfo::LPortDetails PortDetails;
					PortDetails.Name=m_PluginInfo.PortTips[c].c_str();
					m_CurrentPlugin.Ports.push_back(PortDetails);
					
					c++;
				}
			}
			
			for (unsigned int i = 0; i < PlugDesc->PortCount; i++) 
			{	
				if (LADSPA_IS_PORT_OUTPUT(PlugDesc->PortDescriptors[i])) {

					desc = string(PlugDesc->PortNames[i]) +
					       (LADSPA_IS_PORT_CONTROL(PlugDesc->PortDescriptors[i]) ? " (CV)" : " (AU)");
						   
					m_PluginInfo.PortTips.push_back(desc.c_str());
				}
			}
						
			UpdatePluginInfoWithHost();

			if (PortClampReset)
			{
				m_PortMin.clear();
				m_PortMax.clear();			
				m_PortClamp.clear();

				for (int n=0; n<m_PluginInfo.NumInputs; n++)
				{
					float Max=1.0f, Min=-1.0f;
					int Port=m_PortID[n];
						
					// Get the bounding hints for the port
					LADSPA_PortRangeHintDescriptor HintDesc=PlugDesc->PortRangeHints[Port].HintDescriptor;
					if (LADSPA_IS_HINT_BOUNDED_BELOW(HintDesc))					
					{
						Min=PlugDesc->PortRangeHints[Port].LowerBound;
						if (LADSPA_IS_HINT_SAMPLE_RATE(HintDesc))
						{
							Min*=m_HostInfo->SAMPLERATE;
						}
					}
					if (LADSPA_IS_HINT_BOUNDED_ABOVE(HintDesc))					
					{
							Max=PlugDesc->PortRangeHints[Port].UpperBound;
						if (LADSPA_IS_HINT_SAMPLE_RATE(HintDesc))
						{
							Max*=m_HostInfo->SAMPLERATE;
						}
					}
								
					m_PortMin.push_back(Min);
					m_PortMax.push_back(Max);
// PortClamp defaults to true
					m_PortClamp.push_back(true);
					
					m_CurrentPlugin.Ports[n].Min=Min;
					m_CurrentPlugin.Ports[n].Max=Max;
					m_CurrentPlugin.Ports[n].Clamped=true;
				}
			}								
			return true;
		}
	}
	
	cerr << "Error loading LADSPA Plugin.\n";
	
	return false;
}

