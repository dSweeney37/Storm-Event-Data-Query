//#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <cstring>
#include "defn.h"




bst* BuildBST(annual_storms* pArrAnnual, int pAnnualSize, std::string pYear, std::string pType, std::string pLow, std::string  pHigh);
heap** BuildMaxHeap(annual_storms* pArrAnnual, int pAnnualSize, std::string pYear, std::string pType, int pHeapSize);
int CountBSTNodes(bst* pNode);
int CountFatalities(fatality_event* ptr2Fatality);
int ExtractMax(heap** pMaxHeap, int pHeapSize, std::string pPrintMode);
int FindBSTHeight(bst* pNode);
void FindEvent(annual_storms* pArrAnnual, hash_table_entry** pHashTable, int pEventID, int pModVal, int pState);
int FindHashTableSize(int pNumOfEvents);
int FindHeapSize(annual_storms* pArrAnnual, int pAnnualSize, std::string pYear);
void HashData(hash_table_entry** pHashTable, int pHashSize, int pEventsSum);
int HashVal(int pEventID, int pModVal);
bst* InsertNode(bst* pNode, char* ptr2Char, int pEventID, std::string pType);
void LoadFatalityValues(hash_table_entry** pHashTable, storm_event* pArrStorm, int pModVal, std::string pFileName);
void LoadHashStormValues(hash_table_entry** pHashTable, storm_event* pArrStorm, int pModVal, std::string pFileName);
void MaxHeapify(heap** pMaxHeap, int pHeapIndex, int pHeapSize);
int Month2Int(std::string pMonth);
bst* NewNode(char* ptr2Type, int pEventID);
void PrintBST(annual_storms* pArrAnnual, hash_table_entry** pHashTable, int pModVal, bst* pBST, std::string pType);
void PrintFatality(fatality_event* ptr2Fatality);
void PrintStormEvent(storm_event* pArrStorm, int pStormIndex, int pState);
void Query(annual_storms* pArrAnnual, int pAnnualSize, hash_table_entry** pHashTable, int pModVal);
bool TestForPrime(int val);
