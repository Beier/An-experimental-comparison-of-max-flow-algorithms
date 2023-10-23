
#include "LinkedList.h"


LinkedList<void*>::Node** LinkedListNodeManager::unusedNodes = new  LinkedList<void*>::Node*[1024];
int LinkedListNodeManager::unusedNodesLength = 1024;
int LinkedListNodeManager::unusedNodesCount = 0;

LinkedListNodeManager::BulkAllocation* LinkedListNodeManager::bulkAllocations = new BulkAllocation[1];
int LinkedListNodeManager::bulkAllocationsLength = 1;
int LinkedListNodeManager::bulkAllocationsCount = 0;
	
