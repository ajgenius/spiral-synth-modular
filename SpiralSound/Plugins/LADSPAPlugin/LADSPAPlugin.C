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
#include "LADSPAInfo.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "SpiralIcon.xpm"

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
	m_Version=4;

	m_PluginInfo.Name="LADSPA";
	m_PluginInfo.Width=500;
	m_PluginInfo.Height=320;
	m_PluginInfo.NumInputs=0;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Nuffink yet");

	m_PluginIndex = 0;
	m_MaxInputPortCount = m_LADSPAInfo.GetMaxInputPortCount();
	m_InputPortCount = 0;

// For receiving from GUI
	m_AudioCH->Register("SetGain",&(m_InData.Gain));
	m_AudioCH->Register("SetAmped",&(m_InData.Amped));
	m_AudioCH->RegisterData("SetPluginIndex", ChannelHandler::INPUT,&(m_InData.PluginIndex),sizeof(m_InData.PluginIndex));

// For sending to GUI
	m_AudioCH->RegisterData("GetName",ChannelHandler::OUTPUT,m_Name,256);
	m_AudioCH->RegisterData("GetMaker",ChannelHandler::OUTPUT,m_Maker,256);
	m_AudioCH->RegisterData("GetMaxInputPortCount",ChannelHandler::OUTPUT,&(m_MaxInputPortCount),sizeof(m_MaxInputPortCount));
	m_AudioCH->RegisterData("GetInputPortCount",ChannelHandler::OUTPUT,&(m_InputPortCount),sizeof(m_InputPortCount));
	m_AudioCH->RegisterData("GetPluginIndex",ChannelHandler::OUTPUT,&(m_PluginIndex),sizeof(m_PluginIndex));

	m_OutData.InputPortNames = (char *)malloc(256 * m_MaxInputPortCount);
	m_OutData.InputPortSettings = (PortSettings *)malloc(sizeof(PortSettings) * m_MaxInputPortCount);
	m_OutData.InputPortValues = (float *)calloc(m_MaxInputPortCount, sizeof(float));
	m_InData.InputPortSettings = (PortSettings *)malloc(sizeof(PortSettings) * m_MaxInputPortCount);

	if (m_OutData.InputPortNames &&
	    m_OutData.InputPortSettings &&
	    m_InData.InputPortSettings) {
		m_AudioCH->RegisterData("GetInputPortNames", ChannelHandler::OUTPUT, m_OutData.InputPortNames, 256 * m_MaxInputPortCount);
		m_AudioCH->RegisterData("GetInputPortSettings", ChannelHandler::OUTPUT, m_OutData.InputPortSettings, sizeof(PortSettings) * m_MaxInputPortCount);
		m_AudioCH->RegisterData("GetInputPortValues", ChannelHandler::OUTPUT, m_OutData.InputPortValues, sizeof(float) * m_MaxInputPortCount);
		m_AudioCH->RegisterData("SetInputPortSettings", ChannelHandler::INPUT, m_InData.InputPortSettings, sizeof(PortSettings) * m_MaxInputPortCount);
	} else {
		cerr<<"Memory allocation error"<<endl;
	}
}

LADSPAPlugin::~LADSPAPlugin()
{
// Free allocated buffers
	if (m_OutData.InputPortNames)  free(m_OutData.InputPortNames);
	if (m_OutData.InputPortSettings) free(m_OutData.InputPortSettings);
	if (m_OutData.InputPortValues) free(m_OutData.InputPortValues);
	if (m_InData.InputPortSettings)  free(m_InData.InputPortSettings);
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
	return new LADSPAPluginGUI(m_PluginInfo.Width, m_PluginInfo.Height,
	                           this, m_AudioCH, m_HostInfo, m_LADSPAInfo.GetPluginList());
}

void LADSPAPlugin::Execute()
{
	if (PlugDesc)
	{
		// convert inputs if exist (use default if not)
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
				// Copy values into OutData value buffer for display in GUI
				m_OutData.InputPortValues[n] = m_LADSPABufVec[n][0];
			}
			else // Use default
			{
				for (int i=0; i<m_HostInfo->BUFSIZE; i++) {
					m_LADSPABufVec[n][i]=m_PortDefault[n];
				}
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
			case (SETPORTSETTINGS) :
				SetPortSettings();
				break;
			case (SELECTPLUGIN) :
				vector<LADSPAInfo::PluginEntry> pe = m_LADSPAInfo.GetPluginList();
				UpdatePlugin(pe[m_InData.PluginIndex].UniqueID);
				break;
		}
	}
}

void LADSPAPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";

	switch (m_Version)
	{
		case 4:
		{
			s<<m_Gain<<" ";
			s<<m_UniqueID<<" ";
			s<<m_PortMin.size()<<" ";
			assert(m_PortMin.size()==m_PortMax.size());
			assert(m_PortMin.size()==m_PortClamp.size());
			assert(m_PortMin.size()==m_PortDefault.size());
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
			for (vector<float>::iterator i=m_PortDefault.begin();
			     i!=m_PortDefault.end(); i++)
			{
				s<<*i<<" ";
			}
		}
		break;
		case 3:
		{
// Here for consistency - should never actually happen, as
// version is always 4!
//			s<<m_Gain<<" ";
//			s<<m_Filename<<" ";
//			s<<m_Label<<" ";
//			s<<m_PortMin.size()<<" ";
//			assert(m_PortMin.size()==m_PortMax.size());
//			assert(m_PortMin.size()==m_PortClamp.size());
//			for (vector<float>::iterator i=m_PortMin.begin();
//				 i!=m_PortMin.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<float>::iterator i=m_PortMax.begin();
//				 i!=m_PortMax.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<bool>::iterator i=m_PortClamp.begin();
//				 i!=m_PortClamp.end(); i++)
//			{
//				s<<*i<<" ";
//			}
		}
		break;
		case 2:
		{
//			s<<m_Gain<<" ";
//			s<<m_Filename<<" ";
//			s<<m_Label<<" ";
//			s<<m_PortMin.size()<<" ";
//			assert(m_PortMin.size()==m_PortMax.size());
//			for (vector<float>::iterator i=m_PortMin.begin();
//				 i!=m_PortMin.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<float>::iterator i=m_PortMax.begin();
//				 i!=m_PortMax.end(); i++)
//			{
//				s<<*i<<" ";
//			}
		}
		break;
		case 1:
		{
//			s<<m_Gain<<" ";
//			s<<m_Filename<<" ";
//			s<<m_Label<<" ";
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
		case 4:
		{
			s>>m_Gain;

			unsigned long UniqueID;
			s>>UniqueID;
			int PortCount;
			s>>PortCount;
			float min,max;
			bool clamp;
			float defolt;

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
			for (int n=0; n<PortCount; n++)
			{
				s>>defolt;
				m_PortDefault.push_back(defolt);
			}

			UpdatePlugin(UniqueID, false);
		}
		break;

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
			// Get Unique ID from filename and label
				unsigned long id = m_LADSPAInfo.GetIDFromFilenameAndLabel(Filename, Label);
				if (id) UpdatePlugin(id, false);
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
			// Get Unique ID from filename and label
				unsigned long id = m_LADSPAInfo.GetIDFromFilenameAndLabel(Filename, Label);
				if (id) UpdatePlugin(id, false);
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
			// Get Unique ID from filename and label
				unsigned long id = m_LADSPAInfo.GetIDFromFilenameAndLabel(Filename, Label);
				if (id) UpdatePlugin(id, false);
			}
		}
		break;
	}
}

bool LADSPAPlugin::UpdatePlugin(unsigned long UniqueID, bool PortClampReset)
{
	// first call with same info, to clear the ports
	UpdatePluginInfoWithHost();

	if (PlugDesc) {
		if (PlugDesc->deactivate) PlugDesc->deactivate(PlugInstHandle);
		PlugDesc->cleanup(PlugInstHandle);
	}

	PlugDesc = m_LADSPAInfo.GetDescriptorByID(UniqueID, true);

	if (PlugDesc) {
	// Create instance
		if (!(PlugInstHandle = PlugDesc->instantiate(PlugDesc, m_HostInfo->SAMPLERATE))) {
			cerr << "WARNING: Could not instantiate plugin " << UniqueID << endl;
			m_LADSPAInfo.UnloadLibraryByID(UniqueID);
			PlugDesc = 0;
			return 0;
		}

		// Find number of input and output ports
		m_PluginInfo.NumInputs = m_PluginInfo.NumOutputs = 0;
		for (unsigned long i = 0; i < PlugDesc->PortCount; i++) {
			if (LADSPA_IS_PORT_INPUT(PlugDesc->PortDescriptors[i])) {
				m_PluginInfo.NumInputs++;
			} else if (LADSPA_IS_PORT_OUTPUT(PlugDesc->PortDescriptors[i])) {
				m_PluginInfo.NumOutputs++;
			}
		}

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
				m_PortClamp.push_back(true);
				m_PortDefault.push_back(0.0f);
			}
		}

		int lbl_length;
		char *lbl_start;

		m_UniqueID = PlugDesc->UniqueID;
		m_InputPortCount = m_PluginInfo.NumInputs;

		lbl_length = strlen(PlugDesc->Name);
		lbl_length = lbl_length > 255 ? 255 : lbl_length;
		strncpy(m_Name, PlugDesc->Name, lbl_length);
		m_Name[lbl_length] = '\0';

		lbl_length = strlen(PlugDesc->Maker);
		lbl_length = lbl_length > 255 ? 255 : lbl_length;
		strncpy(m_Maker, PlugDesc->Maker, lbl_length);
		m_Maker[lbl_length] = '\0';

		lbl_start = m_OutData.InputPortNames;
		for (unsigned long n = 0; n < m_InputPortCount; n++) {
			lbl_length = m_PluginInfo.PortTips[n].size();
			lbl_length = lbl_length > 255 ? 255 : lbl_length;
			strncpy(lbl_start, m_PluginInfo.PortTips[n].c_str(), lbl_length);
			lbl_start[lbl_length] = '\0';
			lbl_start += 256;

			m_OutData.InputPortSettings[n].Min = m_PortMin[n];
			m_OutData.InputPortSettings[n].Max = m_PortMax[n];
			m_OutData.InputPortSettings[n].Clamp = m_PortClamp[n];
			m_OutData.InputPortSettings[n].Default = m_PortDefault[n];
		}

		return true;
	}

	cerr << "Error loading LADSPA Plugin.\n";

	return false;
}

void LADSPAPlugin::SetPortSettings(void)
{
	for (unsigned long n = 0; n < m_InputPortCount; n++) {
		m_PortMin[n] = m_InData.InputPortSettings[n].Min;
		m_PortMax[n] = m_InData.InputPortSettings[n].Max;
		m_PortClamp[n] = m_InData.InputPortSettings[n].Clamp;
		m_PortDefault[n] = m_InData.InputPortSettings[n].Default;
	}
}
