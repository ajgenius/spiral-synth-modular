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

#include <string>
#include <pthread.h>
#include <deque>
#include "Types.h"
#include "Sample.h"

using namespace std;

#ifndef SAMPLELOADER_CLOCK
#define SAMPLELOADER_CLOCK

namespace spiralcore
{

class AsyncSampleLoader
{
public:
	static AsyncSampleLoader* Get();
	static void Shutdown();
	
	bool AddToQueue(Sample *s, const string &Filename);
	void LoadQueue();
			
private:
	AsyncSampleLoader();
	~AsyncSampleLoader();
	
	static void LoadLoop();

	pthread_t  m_LoaderThread;
	static pthread_mutex_t* m_Mutex;
	
	struct LoadItem
	{
		string Name;
		Sample *SamplePtr;
	};
	
	// two loaderstacks, so we can get a lock on at least one of them at any time
	static deque<LoadItem> m_LoadQueue;
	static AsyncSampleLoader *m_Singleton;
};

}

#endif



