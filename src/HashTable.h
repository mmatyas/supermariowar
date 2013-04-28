#ifndef __HASHTABLE_H_
#define __HASHTABLE_H_

#include "global.h"

struct HashNode
{
	HashNode * next;
	CObject * data;
};

class HashTable
{
	public:
		HashTable();
		~HashTable();

		CObject * Add(CObject * pObject);
		CObject * Get(int iNetworkID);
		CObject * Remove(int iNetworkID);

		void Clear();

	private:
		int size;
		int capacity;
		HashNode * table[MAXOBJECTS * 8];
};

#endif //__HASHTABLE_H_

