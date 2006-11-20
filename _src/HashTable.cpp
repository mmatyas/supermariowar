#include "HashTable.h"

HashTable::HashTable()
{
	size = 0;
	capacity = MAXOBJECTS * 8;

	for(int k = 0; k < capacity; k++)
		table[k] = NULL;
}

HashTable::~HashTable()
{}

CObject * HashTable::Add(CObject * pObject)
{
	int iID = pObject->iNetworkID % capacity;

	if(table[iID] == NULL)
	{
		HashNode * pNode = new HashNode;
		pNode->data = pObject;
		pNode->next = NULL;
		table[iID] = pNode;
	}
	else
	{
		//printf("Collision - Slot: %d  ID: %d\n", iID, pObject->iNetworkID);

		HashNode * pSearchNode = table[iID];
		
		if(pSearchNode->data->iNetworkID == pObject->iNetworkID)
		{
			//printf("  + Duplicate found!\n");
			CObject * pData = pSearchNode->data;
			pSearchNode->data = pObject;
			return pData;
		}

		while(pSearchNode->next)
		{
			if(pSearchNode->next->data->iNetworkID == pObject->iNetworkID)
			{
				//printf("  + Duplicate found!\n");
				CObject * pData = pSearchNode->next->data;
				pSearchNode->next->data = pObject;
				return pData;
			}
			pSearchNode = pSearchNode->next;
		}

		HashNode * pNode = new HashNode;
		pNode->data = pObject;
		pNode->next = NULL;

		pSearchNode->next = pNode;
	}

	size++;

	return NULL;
}

CObject * HashTable::Get(int iNetworkID)
{
	HashNode * pNode = table[iNetworkID % capacity];

	while(pNode)
	{
		if(pNode->data->iNetworkID == iNetworkID)
			return pNode->data;

		pNode = pNode->next;
	}

	return NULL;
}

CObject * HashTable::Remove(int iNetworkID)
{
	int iID = iNetworkID % capacity;

	HashNode * pNode = table[iID];
	HashNode * pPrevNode = NULL;

	while(pNode)
	{
		if(pNode->data->iNetworkID == iNetworkID)
			break;

		pPrevNode = pNode;
		pNode = pNode->next;
	}

	if(!pNode)
		return NULL;

	CObject * pData = pNode->data;

	//Head of list
	if(!pPrevNode)
		table[iID] = pNode->next;
	else
		pPrevNode->next = pNode->next;

	size--;

	delete pNode;
	return pData;
}

void HashTable::Clear()
{
	for(int k = 0; k < capacity; k++)
	{
		while(table[k])
		{
			HashNode * pNode = table[k];
			table[k] = pNode->next;

			delete pNode;
		}
	}

	size = 0;
}


/*
//Hash table unit test

	bool fUsed[4096];

	for(int j = 0; j < 4096; j++)
		fUsed[j] = false;

	HashTable ht;

	for(int k = 0; k < 1000000; k++)
	{
		int iAction = rand() % 10;

		if(iAction == 0)
		{
			CObject * pObject = new CObject(&menu_cpu, 0, 0);
			pObject->iNetworkID = rand() % 4096;
			
			bool expectCollision = false;
			if(fUsed[pObject->iNetworkID])
				expectCollision = true;

			fUsed[pObject->iNetworkID] = true;
			 
			pObject = ht.Add(pObject);
			delete pObject;

			if((pObject && !expectCollision) || (!pObject && expectCollision))
				printf("Failure! ID: %d  Ret: %d  Expect: %d\n", pObject->iNetworkID, pObject, expectCollision);
				
		}
		else if(iAction == 1)
		{
			int id = rand() % 4096;
			CObject * pObject = ht.Remove(id);
			delete pObject;

			bool expectRemove = false;
			if(fUsed[id])
				expectRemove = true;

			fUsed[id] = false;

			if((pObject && !expectRemove) || (!pObject && expectRemove))
			{
				printf("Remove Failed!  ID: %d  Ret: %d  Expect: %d\n", id, pObject, expectRemove);
			}
		}
		else
		{
			int id = rand() % 4096;
			CObject * pObject = ht.Get(id);

			bool expectGet = false;
			if(fUsed[id])
				expectGet = true;

			if((pObject && !expectGet) || (!pObject && expectGet))
			{
				printf("Get Failed!  ID: %d  Ret: %d  Expect: %d\n", id, pObject, expectGet);
			}
		}
	}
*/

