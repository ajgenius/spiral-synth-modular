/*
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

#include "Midi.h"
#include "NoteTable.h"
#include "unistd.h"
#include "sys/types.h"
#include "signal.h"
#include "pthread.h"

#ifdef KEYBOARD_SUPPORT
#include <FL/Fl.h>
#endif

static const int MIDI_SCANBUFSIZE=256;
static const int MIDI_KEYOFFSET=0;

static const unsigned char STATUS_START            = 0x80;		
static const unsigned char STATUS_NOTE_OFF         = 0x80;
static const unsigned char STATUS_NOTE_ON          = 0x90;
static const unsigned char STATUS_AFTERTOUCH       = 0xa0;
static const unsigned char STATUS_CONTROL_CHANGE   = 0xb0;
static const unsigned char STATUS_PROG_CHANGE      = 0xc0;
static const unsigned char STATUS_CHANNEL_PRESSURE = 0xd0;
static const unsigned char STATUS_PITCH_WHEEL      = 0xe0;
static const unsigned char STATUS_END              = 0xf0;
static const unsigned char SYSEX_START             = 0xf0;
static const unsigned char SYSEX_TERMINATOR        = 0xf7;
static const unsigned char MIDI_CLOCK              = 0xf8;
static const unsigned char ACTIVE_SENSE            = 0xfe;

static int NKEYS = 30;
static char KEYMAP[30]={'z','s','x','d','c','v','g','b','h','n','j','m','q',
						'2','w','3','e','r','5','t','6','y','7','u','i','9',
						'o','0','p','['};
							
MidiDevice *MidiDevice::m_Singleton;
string MidiDevice::m_DeviceName;
 
MidiDevice::MidiDevice() :
m_Poly(1)
{
	Open();

#ifdef KEYBOARD_SUPPORT
	m_Oct=4;
	m_CurrentVoice=0;
	
	for (int n=0; n<256; n++)
	{	
		m_KeyVoice[n]=' ';
	}
#endif
}

MidiDevice::~MidiDevice() 
{
	Close();
}

void MidiDevice::Close()
{
	pthread_mutex_lock(m_Mutex);
	pthread_cancel(m_MidiReader); 
	pthread_mutex_unlock(m_Mutex);
	pthread_mutex_destroy(m_Mutex);
		
	close(m_MidiFd);
	close(m_MidiWrFd);
	cerr<<"Closed midi device"<<endl;
}


void MidiDevice::Open()
{
	//if (!SpiralInfo::WANTMIDI) return;
	
	m_MidiFd = open(m_DeviceName.c_str(),O_RDONLY|O_SYNC);  
	if (!m_MidiFd) 
	{
		cerr<<"Couldn't open midi for reading ["<<m_DeviceName<<"]"<<endl;
		return;
	}
	
	m_MidiWrFd = open(m_DeviceName.c_str(),O_WRONLY);  
	if (!m_MidiWrFd) 
	{
		cerr<<"Couldn't open midi for writing ["<<m_DeviceName<<"]"<<endl;
		return;
	}
	
	cerr<<"Opened midi device ["<<m_DeviceName<<"]"<<endl;
	
	m_Mutex = new pthread_mutex_t;
    pthread_mutex_init(m_Mutex, NULL);
    int ret=pthread_create(&m_MidiReader,NULL,(void*(*)(void*))MidiDevice::MidiReaderCallback,(void*)this);	
}


// returns the next event off the list, or an 
// empty event if the list is exhausted
MidiEvent MidiDevice::GetEvent(int Device)
{
	if (Device<0 || Device>15)
	{
		cerr<<"GetEvent: Invalid Midi device "<<Device<<endl;
		return MidiEvent(MidiEvent::NONE,0,0);
	}

#ifdef KEYBOARD_SUPPORT
	CheckKeyboard();
#endif

	pthread_mutex_lock(m_Mutex);
	if (m_EventVec[Device].size()==0)
	{
		pthread_mutex_unlock(m_Mutex);
		return MidiEvent(MidiEvent::NONE,0,0);
	}
	
	MidiEvent event(m_EventVec[Device].front());
	m_EventVec[Device].pop();
	pthread_mutex_unlock(m_Mutex);
		
	return event;
}

void MidiDevice::SendEvent(int Device,const MidiEvent &Event)
{
	if (Device<0 || Device>15)
	{
		cerr<<"SendEvent: Invalid Midi device "<<Device<<endl;		
	}

	char message[3];
	
	message[1]=Event.GetNote()+MIDI_KEYOFFSET;
	message[2]=(char)Event.GetVolume();
	
	if (Event.GetType()==MidiEvent::ON)
	{
		message[0]=STATUS_NOTE_ON+Device;
		write(m_MidiWrFd,message,3);
		//cerr<<"sending "<<message<<endl;
	}
	
	if (Event.GetType()==MidiEvent::OFF)
	{
		message[0]=STATUS_NOTE_OFF+Device;
		write(m_MidiWrFd,message,3);	
		//cerr<<"sending "<<message<<endl;
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Reader thread functions

// little helper to strip out the realtime and unused messages
void MidiDevice::ReadByte(unsigned char *c)
{
	*c=MIDI_CLOCK;
	do read(m_MidiFd,c,1);				
	while (*c>=STATUS_END);		
}

// collect events deals with the byte level messages, and sorts 
// and filters them into distinct messages we can handle easily
void MidiDevice::CollectEvents()
{		
	unsigned char buf[1];	
	int count,n,nn;
	bool MessageSent;
	unsigned char data[3],last=0;
	
	// constantly scan for relevent input,
	// and write it to the pipe

	// filters out unhandled messages, and attempts to build 
	// coherent messages to send to the midi handler
	bool InSysex=false;
		
	for(;;)
	{
		ReadByte(buf);
		
		if (*buf>=STATUS_START) // we've got a status byte
		{											
			if (*buf==SYSEX_TERMINATOR) InSysex=false;

			// find out if it's an opcode
			if(*buf>=STATUS_START && *buf<=STATUS_END)   
			{				
				InSysex=false;
				last=data[0]=*buf;
			
				if (data[0]>=STATUS_PROG_CHANGE && data[0]<STATUS_PITCH_WHEEL) 
				{
					ReadByte(&data[1]); //one byte
					data[2]=0;
				}
				else // get the next two bytes
				{					
					ReadByte(&data[1]); 
					ReadByte(&data[2]); 
				}
				AddEvent(data);													
			}			
			else // its a sysex or other message like active sense
			{ 
				if (*buf==SYSEX_START) InSysex=true;
				cerr<<"Unhandled midi message: "; printf("%x\n",(int)*buf);				
			}
		}
		else // more data (running status) 
		{
			if (!InSysex) 
			{
				data[0]=last;
				data[1]=*buf;
				
				if (data[0]>=STATUS_PROG_CHANGE && data[0]<STATUS_PITCH_WHEEL) 
				{					
					data[2]=0;  //one byte
				}
				else // get the next byte
				{						
					ReadByte(&data[2]); 					
				}
				
				AddEvent(data);							  											
			}
		}			
	}
}
		
// addevent converts the midi bytecode into midi message objects and 
// stacks them onto the event list to be picked up by the app
void MidiDevice::AddEvent(unsigned char* midi)
{	
	MidiEvent::type MessageType=MidiEvent::NONE;
	int Volume=0,Note=0,EventDevice=0;	
	
	// note off
	if (midi[0] >= STATUS_NOTE_OFF && midi[0] < STATUS_NOTE_ON) 
	{	
		MessageType=MidiEvent::OFF;
		Note=midi[1]-MIDI_KEYOFFSET;
		EventDevice=midi[0]-STATUS_NOTE_OFF;
	}
	// note on
	else if (midi[0] >= STATUS_NOTE_ON && midi[0] < STATUS_AFTERTOUCH)
	{
		Volume = midi[2];
		
		// cope with Roland equipment, where note on's 
		// with zero velocity are sent as note offs.
		if (Volume) MessageType=MidiEvent::ON;
		else MessageType=MidiEvent::OFF;

		Note=midi[1]-MIDI_KEYOFFSET;
		EventDevice=midi[0]-STATUS_NOTE_ON;
	}
	// aftertouch
	else if (midi[0] >= STATUS_AFTERTOUCH && midi[0] < STATUS_CONTROL_CHANGE)
	{			
		MessageType=MidiEvent::AFTERTOUCH;
		Note=midi[1]-MIDI_KEYOFFSET;		
		Volume=midi[2];			
		EventDevice=midi[0]-STATUS_AFTERTOUCH;
	}
	// parameter
	else if (midi[0] >= STATUS_CONTROL_CHANGE && midi[0] < STATUS_PROG_CHANGE)
	{	
		MessageType=MidiEvent::PARAMETER;
		Note=midi[1];
		Volume=midi[2];
		EventDevice=midi[0]-STATUS_CONTROL_CHANGE;
	}
	// channel pressure
	else if (midi[0] >= STATUS_CHANNEL_PRESSURE && midi[0] < STATUS_PITCH_WHEEL)
	{			
		MessageType=MidiEvent::CHANNELPRESSURE;		
		Volume=midi[1];			
		EventDevice=midi[0]-STATUS_CHANNEL_PRESSURE;
	}
	// note pitchbend
	else if (midi[0] >= STATUS_PITCH_WHEEL && midi[0] < STATUS_END)
	{			
		MessageType=MidiEvent::PITCHBEND;
		// should probably take the first byte into account too?
		Volume=midi[2];			
		EventDevice=midi[0]-STATUS_PITCH_WHEEL;
	}

	if (EventDevice<0 || EventDevice>15)
	{
		cerr<<"Error - Midi device "<<EventDevice<<" ??"<<endl;
		return;
	}

	pthread_mutex_lock(m_Mutex);
	m_EventVec[EventDevice].push(MidiEvent(MessageType,Note,Volume));
	pthread_mutex_unlock(m_Mutex);
}

///////////////////////////////////////////////////////////////////////////////

// Parse the keyboard to generate midi messages
#ifdef KEYBOARD_SUPPORT
void MidiDevice::CheckKeyboard()
{
	Fl::check();
	MidiEvent::type MessageType=MidiEvent::NONE;
	int Volume=0,Note=0,EventDevice=0;

	if (Fl::event_key(FL_F+1)) m_Oct=0; 
	if (Fl::event_key(FL_F+2)) m_Oct=1; 
	if (Fl::event_key(FL_F+3)) m_Oct=2; 
	if (Fl::event_key(FL_F+4)) m_Oct=3; 
	if (Fl::event_key(FL_F+5)) m_Oct=4; 
	if (Fl::event_key(FL_F+6)) m_Oct=5; 
	if (Fl::event_key(FL_F+7)) m_Oct=6; 
	if (Fl::event_key(FL_F+8)) m_Oct=7; 
	if (Fl::event_key(FL_F+9)) m_Oct=8; 
	if (Fl::event_key(FL_F+10)) m_Oct=9; 
	if (Fl::event_key(FL_F+11)) m_Oct=10; 

	int  note=0;
	char KeyChar=0;
	bool KeyPressed=false;

	for (int key=0; key<NKEYS; key++)
	{	
		KeyChar=KEYMAP[key];
		
		// check if a key's been pressed
		if (Fl::event_key(KeyChar)) 
		{
			KeyPressed=true;
	
			// check it was pressed last time
			bool Found=false;
			for (int n=0; n<m_Poly; n++)
			{
				if (m_KeyVoice[n]==KeyChar)
				{
					Found=true;
				}				
			}
			if (!Found)
			{
				Volume = 127;
				MessageType=MidiEvent::ON;
				Note=(m_Oct*12)+note;
				
				// Move to the next voice
				m_CurrentVoice++;
	
				if (m_CurrentVoice>=m_Poly)
				{
					m_CurrentVoice=0;
				}
				
				// this should be the current voice we are using
				m_KeyVoice[m_CurrentVoice]=KeyChar;
			}
		}
		else // it's not pressed down 
		{
			//see if the note was pressed down last time			
			for (int n=0; n<m_Poly; n++)
			{
				if (m_KeyVoice[n]==KeyChar)
				{				
					MessageType=MidiEvent::OFF;
					Note=(m_Oct*12)+note;
					m_KeyVoice[n]=' ';
				}				
			}
		}
		note++;
	}
	
	if (MessageType!=MidiEvent::NONE)
	{
		pthread_mutex_lock(m_Mutex);
		m_EventVec[EventDevice].push(MidiEvent(MessageType,Note,Volume));
		pthread_mutex_unlock(m_Mutex);
	}
}
#endif
