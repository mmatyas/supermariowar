#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "global.h"

struct HashNode {
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

#endif // HASHTABLE_H
