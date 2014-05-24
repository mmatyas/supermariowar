#include "HashTable.h"

HashTable::HashTable()
{
	size = 0;
	capacity = MAXOBJECTS * 8;

	for (int k = 0; k < capacity; k++)
		table[k] = NULL;
}

HashTable::~HashTable()
{}

CObject * HashTable::Add(CObject * pObject)
{
	int iID = pObject->iNetworkID % capacity;

    if(table[iID] == NULL) {
		HashNode * pNode = new HashNode;
		pNode->data = pObject;
		pNode->next = NULL;
		table[iID] = pNode;
    } else {
		//printf("Collision - Slot: %d  ID: %d\n", iID, pObject->iNetworkID);

		HashNode * pSearchNode = table[iID];

        if(pSearchNode->data->iNetworkID == pObject->iNetworkID) {
			//printf("  + Duplicate found!\n");
			CObject * pData = pSearchNode->data;
			pSearchNode->data = pObject;
			return pData;
		}

        while(pSearchNode->next) {
            if(pSearchNode->next->data->iNetworkID == pObject->iNetworkID) {
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

    while(pNode) {
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

    while(pNode) {
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
    for (int k = 0; k < capacity; k++) {
        while(table[k]) {
			HashNode * pNode = table[k];
			table[k] = pNode->next;

			delete pNode;
		}
	}

	size = 0;
}

