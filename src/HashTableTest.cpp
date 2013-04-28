
#if	_DEBUG

#include "HashTable.h"
#include "object.h"

//Hash table unit test
//neagix: we will run this at runtime in debug mode, for now

void HashTableTest() {
	bool fUsed[4096];

	for(int j = 0; j < 4096; j++)
		fUsed[j] = false;

	HashTable ht;

	for(int k = 0; k < 1000000; k++)
	{
		int iAction = GetRandMax(10);

		if(iAction == 0)
		{
//			CObject * pObject = new CObject(&menu_cpu, 0, 0);
			CObject *pObject = new MO_Coin(&spr_coin, 0.0f, 0.0f, 0, 0, 2, 0, 0, 0, true);

			pObject->iNetworkID = GetRandMax(4096);

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
			int id = GetRandMax(4096);
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
			int id = GetRandMax(4096);
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
}

#endif
