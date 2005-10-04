#include <iostream>

#ifndef ALLOCATOR
#define ALLOCATOR

using namespace std;

class Allocator
{
public:
	virtual char *New(unsigned int size)=0;
	virtual void Delete(char *mem)=0;
};

///////////////////////////////////////////////////

class MallocAllocator : public Allocator
{
public:
	MallocAllocator() {}

	virtual char *New(unsigned int size);
	virtual void Delete(char *mem);
	virtual ~MallocAllocator() {}
	
protected:
};

/////////////////////////////////////////////////////

class RealtimeAllocator : public Allocator
{
public:
	RealtimeAllocator(unsigned int size);

	virtual char *New(unsigned int size);
	virtual void Delete(char *mem);

protected:
	
	char *m_Buffer;	
	unsigned int m_Position;
	unsigned int m_Size;
};

#endif
