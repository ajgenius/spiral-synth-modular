// Copyright (C) 2004 David Griffiths <dave@pawfal.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <unistd.h>
#include <sndfile.h>
#include "AsyncSampleLoader.h"

using namespace spiralcore;

AsyncSampleLoader *AsyncSampleLoader::m_Singleton=NULL;
deque<AsyncSampleLoader::LoadItem> AsyncSampleLoader::m_LoadQueue;
pthread_mutex_t* AsyncSampleLoader::m_Mutex;

AsyncSampleLoader* AsyncSampleLoader::Get()
{
	if (!m_Singleton) m_Singleton = new AsyncSampleLoader();
	return m_Singleton;
}

void AsyncSampleLoader::Shutdown()
{
	if (m_Singleton) delete m_Singleton;
}

AsyncSampleLoader::AsyncSampleLoader()
{
	m_Mutex = new pthread_mutex_t;
	pthread_mutex_init(m_Mutex,NULL);
}

AsyncSampleLoader::~AsyncSampleLoader()
{
	Shutdown();
}

bool AsyncSampleLoader::AddToQueue(Sample *s, const string &Filename)
{
	LoadItem NewItem;
	NewItem.Name=Filename;
	NewItem.SamplePtr=s;
	
	// spinlock
	for (int n=0; n<5; n++)
	{
		if (pthread_mutex_trylock(m_Mutex))
		{
			m_LoadQueue.push_back(NewItem);
			pthread_mutex_unlock(m_Mutex);
			return true;
		}
	}
	cerr<<"Could not get a lock on the loaderqueue, not loading ["<<Filename<<"]"<<endl;
	return false;
}

void AsyncSampleLoader::LoadQueue()
{
	pthread_create(&m_LoaderThread,NULL,(void*(*)(void*))LoadLoop,NULL);
}

void AsyncSampleLoader::LoadLoop()
{		
	pthread_mutex_lock(m_Mutex);
	while (m_LoadQueue.size())
	{
		LoadItem Item = *m_LoadQueue.begin();
		m_LoadQueue.pop_front();
		pthread_mutex_unlock(m_Mutex);
			
		SF_INFO info;
		info.format=0;
		SNDFILE* file = sf_open (Item.Name.c_str(), SFM_READ, &info) ;
		if (!file)
		{
			cerr<<"Error opening ["<<Item.Name<<"] : "<<sf_strerror (file)<<endl;
		}
		
		Item.SamplePtr->Allocate(info.frames);
			
		// mix down to mono if need be
		if (info.channels>1)
		{
			float *Buffer = new float[info.frames*info.channels];
			sf_readf_float(file,Buffer,info.frames*info.channels);
			int from=0;
			for (int n=0; n<info.frames; n++)
			{
				for (int c=0; c<info.channels; c++)
				{
					Item.SamplePtr->Set(n,((*Item.SamplePtr)[n]+Buffer[from++])/2.0f);
				}
			}
		}
		else
		{
			sf_readf_float(file, Item.SamplePtr->GetNonConstBuffer(), info.frames);	
		}
		sf_close(file);
	}
	pthread_mutex_unlock(m_Mutex);
}




