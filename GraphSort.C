/*  Graph Sort
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

#include <iostream>
#include "GraphSort.h"

//#define GRAPHSORT_TRACE

//////////////////////////////////////////////////////////

GraphSort::GraphSort()
{
}

GraphSort::~GraphSort()
{
}

void GraphSort::Clear() 
{ 
	//m_Sorted.clear();
	m_Graph.clear(); 
}


const list<int> &GraphSort::GetSortedList()
{
	return m_Sorted;
}

void GraphSort::Sort()
{	
	// walk back from all the roots
	m_Sorted.clear();
	list<int> RootNodes;
	bool FoundRoot=false;
	
	for (map<int,Node>::iterator i=m_Graph.begin();
		 i!=m_Graph.end(); i++)
	{
		// if there are no outputs, this must be a root
		if (i->second.Outputs.empty())
		{
			FoundRoot=true;
			RecursiveWalk(i->first);
		}
	}
	
	// no roots found - try looking for a terminal node and recursing from
	// there, this makes circular graphs work.
	if (!FoundRoot)
	{
		for (map<int,Node>::iterator i=m_Graph.begin();
			 i!=m_Graph.end(); i++)
		{
			// if there are no outputs, this must be a root
			if (i->second.IsTerminal)
			{	
				RecursiveWalk(i->first);
			}
		}
	}
	
	#ifdef GRAPHSORT_TRACE
	for(list<int>::iterator i=m_Sorted.begin();
		i!=m_Sorted.end(); i++) 
	{
		cerr<<*i<<" ";
	}
	cerr<<endl;
	#endif
}

void GraphSort::RecursiveWalk(int node)
{
	// check it's not been logged already 
	// (don't want to execute the node twice)
	if(find(m_Sorted.begin(),m_Sorted.end(),node)==m_Sorted.end())
	{
		#ifdef GRAPHSORT_TRACE
		//cerr<<"adding "<<node<<" to list"<<endl;
		#endif
		m_Sorted.push_back(node);	
	}
	else
	{	
		#ifdef GRAPHSORT_TRACE
		//cerr<<"Node "<<node<<" already logged, ending this path"<<endl;
		#endif
		return;
	}
	
	// branch back into the inputs
	map<int,Node>::iterator i=m_Graph.find(node);
	for(list<int>::iterator ii=i->second.Inputs.begin();
			 ii!=i->second.Inputs.end(); ii++)
	{
		RecursiveWalk(*ii);
	}
}

void GraphSort::Dump()
{
	for (map<int,Node>::iterator i=m_Graph.begin();
		 i!=m_Graph.end(); i++)
	{
		cerr<<"Node "<<i->first<<endl;
		cerr<<"i=";
		for (list<int>::iterator ii=i->second.Inputs.begin();
			 ii!=i->second.Inputs.end(); ii++)
		{
			cerr<<*ii<<" ";
		}
		
		cerr<<endl<<"o=";
		
		for (list<int>::iterator oi=i->second.Outputs.begin();
			 oi!=i->second.Outputs.end(); oi++)
		{
			cerr<<*oi<<" ";
		}
		
		cerr<<endl;cerr<<endl;
	}
}

void GraphSort::AddConnection(int SID, bool STerminal, int DID, bool DTerminal)
{
	map<int,Node>::iterator si=m_Graph.find(SID);
	if (si==m_Graph.end())
	{
		Node newnode;
		newnode.IsTerminal = STerminal;
		m_Graph[SID]=newnode;
		#ifdef GRAPHSORT_TRACE		
		cerr<<"added "<<SID<<endl;
		#endif
	}

	map<int,Node>::iterator di=m_Graph.find(DID);
	if (di==m_Graph.end())
	{
		Node newnode;
		newnode.IsTerminal = DTerminal;
		m_Graph[DID]=newnode;
		#ifdef GRAPHSORT_TRACE
		cerr<<"added "<<DID<<endl;
		#endif
	}
	
	m_Graph[SID].Outputs.push_back(DID);
	m_Graph[DID].Inputs.push_back(SID);
	
	Sort();
}

void GraphSort::RemoveConnection(int SID, int DID)
{
	map<int,Node>::iterator si=m_Graph.find(SID);
	if (si==m_Graph.end())
	{
		cerr<<"GraphSort::RemoveConnection - can't find source node"<<endl;
	}

	map<int,Node>::iterator di=m_Graph.find(DID);
	if (di==m_Graph.end())
	{
		cerr<<"GraphSort::RemoveConnection - can't find dest node"<<endl;
	}
	
	list<int>::iterator soi = find(si->second.Outputs.begin(), si->second.Outputs.end(), DID);
	if (soi==si->second.Outputs.end()) cerr<<"GraphSort::RemoveConnection - can't find soi"<<endl;
	else si->second.Outputs.erase(soi);
	
	list<int>::iterator dii = find(di->second.Inputs.begin(), di->second.Inputs.end(), SID);
	if (dii==di->second.Inputs.end()) cerr<<"GraphSort::RemoveConnection - can't find dii"<<endl;
	else di->second.Inputs.erase(dii);
	
	// check to remove the nodes
	if (si->second.Outputs.empty() && si->second.Inputs.empty())
	{
		m_Graph.erase(si);
		#ifdef GRAPHSORT_TRACE
		cerr<<"removed "<<SID<<endl;
		#endif
	}

	if (di->second.Outputs.empty() && di->second.Inputs.empty())
	{
		m_Graph.erase(di);
		#ifdef GRAPHSORT_TRACE
		cerr<<"removed "<<DID<<endl;
		#endif
	}
	
	Sort();
}	
