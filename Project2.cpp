/*
 Author: Daniel T. Sweeney
 Class: CSE310 - 83657
 Date: 11/5/2019
 Description: Builds a hash table from the user desiganted number of storm files and allows the user to enter a number of queries to process on the
	on the data. The range query uses a BST to retrieve info and find max uses a max-heap to retrieve info.
*/
#include "Project2.h"
using namespace std;




int main(int argc, const char* argv[]) {
	annual_storms* arrAnnual;
	bool proceed = false;
	const int MIN_YEAR = 1950, MAX_YEAR = 2019;
	hash_table_entry** hashTable;
	ifstream inFile;
	int* arrNumOfEvents = new int[stoi(argv[1])], eventSum = 0, annualSize = 0, annualIndex = 0, hashSize;
	storm_event* arrStorm;
	string  detailFileName = "details-YYYY.csv", fatalityFileName = "fatalities-YYYY.csv", sLine;



	for (int i = 0; i < stoi(argv[1]); i++) {
		if (stoi(argv[2 + i]) >= MIN_YEAR && stoi(argv[2 + i]) <= MAX_YEAR) { // Checks if the entered year is valid.
			int numOfEvents = -1; // Starts at -1 to account for the header row.



			proceed = true;
			detailFileName.replace(8, 4, argv[2 + i]); // Replaces the YYYY part of the 'detailFileName' w/the file year.
			inFile.open(detailFileName); // Opens the file.

			while (getline(inFile, sLine, '\n')) { ++numOfEvents; } // Counts the # of events within the file.
			arrNumOfEvents[annualSize++] = numOfEvents; // Loads 'numOfEvents' into an array for later initialization reference.
			eventSum += numOfEvents; // Sums all of the 'numOfEvents' together in order to calculate the size of the hash table.
			inFile.close(); // Closes the file.
		}
		else { cout << "The year " << stoi(argv[2 + i]) << " falls outside of the allowed range of "<< MIN_YEAR << "-" << MAX_YEAR <<
			" and thus could not be processed.\n"; }
	}
	
	hashSize = FindHashTableSize(eventSum); // Finds the hash table size using the 'eventSum' value.
	hashTable = new hash_table_entry * [hashSize](); // Initializes a hash table that has 'hashSize' NULL elements.
	arrAnnual = new annual_storms[annualSize]; // Initializes an annual array that holds "i" elements where "i" is the # of valid input years.
	
	for (int i = 0; i < stoi(argv[1]); i++) {
		if (stoi(argv[2 + i]) >= MIN_YEAR && stoi(argv[2 + i]) <= MAX_YEAR) { // Checks if the entered year is valid.
			detailFileName.replace(8, 4, argv[2 + i]); // Replaces the YYYY part of the 'detailFileName' w/the file year.
			fatalityFileName.replace(11, 4, argv[2 + i]); // Replaces the YYYY part of the 'fatalityFileName' w/the file year.

			arrStorm = new storm_event[arrNumOfEvents[annualIndex]](); // Initializes a 'storm_event' array using a previously stored size.

			arrAnnual[annualIndex].year = stoi(argv[2 + i]); // Loads the year of the storm events into the annual array's corresponding data member.
			arrAnnual[annualIndex].length = arrNumOfEvents[annualIndex]; // Loads the # of elements stored in 'arrStorm'.
			arrAnnual[annualIndex++].events = arrStorm; // Loads the pointer to the head of 'arrStorm' into the annual array's corresponding data member.

			LoadHashStormValues(hashTable, arrStorm, hashSize, detailFileName);
			LoadFatalityValues(hashTable, arrStorm, hashSize, fatalityFileName);
		}
	}
	if (proceed == true) { Query(arrAnnual, annualSize, hashTable, hashSize); }
	HashData(hashTable, hashSize, eventSum);
	
	delete[] arrNumOfEvents; // Deletes the array 'arrNumOfEvents' as it is no longer needed.
	delete[] arrAnnual;
	delete[] hashTable;
	return 0; // Exits the program.
}




bst* BuildBST(annual_storms* pArrAnnual, int pAnnualSize, string pYear, string pType, string pLow, string pHigh) {
	bst* BST = NULL;
	int low = 0, high = 0;



	if (pType == "month_name") {
		low = Month2Int(pLow);
		high = Month2Int(pHigh);
	}

	for (int i = 0; i < pAnnualSize; i++) {
		if (pYear == "all" || stoi(pYear) == pArrAnnual[i].year) {
			for (int j = 0; j < pArrAnnual[i].length; j++) {
				if (pType == "state" && pArrAnnual[i].events[j].state[0] >= pLow[0] && pArrAnnual[i].events[j].state[0] <= pHigh[0]) {
					BST = InsertNode(BST, pArrAnnual[i].events[j].state, pArrAnnual[i].events[j].event_id, pType);
				}
				else if (pType == "month_name" && Month2Int(pArrAnnual[i].events[j].month_name) >= low && Month2Int(pArrAnnual[i].events[j].month_name) <= high) {
					BST = InsertNode(BST, pArrAnnual[i].events[j].month_name, pArrAnnual[i].events[j].event_id, pType);
				}
			}
		}
	}
	return BST;
}




heap** BuildMaxHeap(annual_storms* pArrAnnual, int pAnnualSize, string pYear, string pType, int pHeapSize) {
	heap** arrHeap = new heap * [pHeapSize];
	int heapIndex = 0;


	for (int i = 0; i < pAnnualSize; i++) {
		if (pYear == "all" || stoi(pYear) == pArrAnnual[i].year) {
			for (int j = 0; j < pArrAnnual[i].length; j++) {
				heap* tempHeap = new heap;
				arrHeap[heapIndex++] = tempHeap;
				tempHeap->ptr2Storm = &pArrAnnual[i].events[j];

				if (pType == "damage_property") { tempHeap->max = pArrAnnual[i].events[j].damage_property; }
				else if (pType == "damage_crops") { tempHeap->max = pArrAnnual[i].events[j].damage_crops; }
				else if (pType == "fatality") { tempHeap->max = CountFatalities(pArrAnnual[i].events[j].f); }
			}
		}
	}

	for (int i = floor(pHeapSize / 2); i >= 0; i--) { MaxHeapify(arrHeap, i, pHeapSize); }
	return arrHeap;
}




int CountBSTNodes(bst* pNode) {
	int left = 0, right = 0;


	if (pNode == NULL) { return 0; }
	else {
		left = CountBSTNodes(pNode->left);
		right = CountBSTNodes(pNode->right);
		return 1 + left + right;
	}
}




int CountFatalities(fatality_event* ptr2Fatality) {
	int numOf = 0;


	while (ptr2Fatality != NULL) { 
		numOf++;
		ptr2Fatality = ptr2Fatality->next;
	}
	return numOf;
}




int ExtractMax(heap** pMaxHeap, int pHeapSize, string pPrintMode) {
	cout << "  Event ID: " << pMaxHeap[0]->ptr2Storm->event_id;
	
	if (pPrintMode != "fatality") {	cout << ", Event Type: " << pMaxHeap[0]->ptr2Storm->event_type; }
	
	if (pPrintMode == "damage_property") { cout << ", Property Damage: $" << pMaxHeap[0]->max << endl; }
	else if (pPrintMode == "damage_crops") { cout << ", Crop Damage: $" << pMaxHeap[0]->max << endl; }
	else if (pPrintMode == "fatality") { 
		cout << "\n  Number of Fatalities: " << pMaxHeap[0]->max;
		PrintFatality(pMaxHeap[0]->ptr2Storm->f);
		cout << endl;
	}

	swap(pMaxHeap[0], pMaxHeap[--pHeapSize]);
	MaxHeapify(pMaxHeap, 0, pHeapSize);
	return pHeapSize;
}




int FindBSTHeight(bst* pNode) {
	int left = 0, right = 0;


	if (pNode == NULL) { return 0; }
	else {
		left = FindBSTHeight(pNode->left);
		right = FindBSTHeight(pNode->right);
		if (left > right) { return left + 1; }
		else return right + 1;
	}
}




void FindEvent(annual_storms* pArrAnnual, hash_table_entry** pHashTable, int pEventID, int pModVal, int pPrintMode) {
	hash_table_entry* ptr2Hash;
	int counter = 0, hashIndex;
	storm_event* ptr2Storm;



	hashIndex = HashVal(pEventID, pModVal); // Gets the hash table index value of the 'event_id'.
	ptr2Hash = pHashTable[hashIndex]; // Sets 'ptr2Hash' to the address of the hash table at the index location.

	while (ptr2Hash != NULL && ptr2Hash->event_id != pEventID) { ptr2Hash = ptr2Hash->next; } // Cycles through the linked list until a matching 'event_id' is found.
	if (ptr2Hash != NULL) {
		while (ptr2Hash->year != pArrAnnual[counter].year) { counter++; } // Cycles through the 'pArrAnnual' until the year of the event at 'ptr2Hash' matches the year of the storm_event array.
		ptr2Storm = pArrAnnual[counter].events; // Sets 'ptr2Storm' to the address of the 'storm_event' array.

		PrintStormEvent(ptr2Storm, ptr2Hash->event_index, pPrintMode);
	}
	else { cout << "  Storm event \"" << pEventID << "\" does not exist." << endl; } // Default msg if the event cannot be located.
}




int FindHashTableSize(int pNumOfEvents) {
	int hashSize = (pNumOfEvents * 2) + 1;


	while (!TestForPrime(hashSize)) { hashSize++; } // Finds the first prime # that is > the # of events * 2.
	return hashSize;
}




int FindHeapSize(annual_storms* pArrAnnual, int pAnnualSize, string pYear) {
	int heapSize = 0;


	for (int i = 0; i < pAnnualSize; i++) {
		if (pYear == "all" || stoi(pYear) == pArrAnnual[i].year) { heapSize += pArrAnnual[i].length; }
	}
	return heapSize;
}




void HashData(hash_table_entry** pHashTable, int pHashSize, int pEventSum) {
	int* arrLenOfChains = new int[1](), maxLen = 0;



	for (int i = 0; i < pHashSize; i++) {
		hash_table_entry* ptr2Hash = pHashTable[i];
		int counter = 0;


		while (ptr2Hash != NULL) {
			if (ptr2Hash->event_id != 0) { counter++; }
			ptr2Hash = ptr2Hash->next;
		}

		if (counter > maxLen) {
			int* tempArr = new int[counter + 1]();


			for (int j = 0; j <= maxLen; j++) { tempArr[j] = arrLenOfChains[j]; }
			
			delete[] arrLenOfChains;
			arrLenOfChains = tempArr;
			maxLen = counter;
		}
		arrLenOfChains[counter] += 1;
	}
	
	cout << "\nHash Table Chain Lengths (Chain Length: # Of Chains)" << endl;
	for (int i = 0; i <= maxLen; i++) { cout << "  " <<  i << ": " << arrLenOfChains[i] << endl; }
	cout << "\nLoad Factor: " << ((double) pEventSum / pHashSize) * 100 << "%" << endl;
	delete[] arrLenOfChains;
}




int HashVal(int pEventID, int pModVal) { return pEventID % pModVal; } // Returns the remainder of 'pEventID' divided by 'pModVal'.




bst* InsertNode(bst* pNode, char* ptr2Char, int pEventID, string pType) {
	if (pNode == NULL) { return NewNode(ptr2Char, pEventID); }

	if (strcmp(ptr2Char, pNode->s) == 0) {
		if (pEventID < pNode->event_id) { pNode->left = InsertNode(pNode->left, ptr2Char, pEventID, pType); }
		else if (pEventID > pNode->event_id) { pNode->right = InsertNode(pNode->right, ptr2Char, pEventID, pType); }
	}
	else if (pType == "month_name" && Month2Int(ptr2Char) < Month2Int(pNode->s)) {
		pNode->left = InsertNode(pNode->left, ptr2Char, pEventID, pType); }
	else if (pType == "month_name" && Month2Int(ptr2Char) > Month2Int(pNode->s)) {
		pNode->right = InsertNode(pNode->right, ptr2Char, pEventID, pType); }
	else if (pType == "state" && strcmp(ptr2Char, pNode->s) < 0) {
		pNode->left = InsertNode(pNode->left, ptr2Char, pEventID, pType); }
	else if (pType == "state" && strcmp(ptr2Char, pNode->s) > 0) {
		pNode->right = InsertNode(pNode->right, ptr2Char, pEventID, pType); }

	return pNode;
}




void LoadFatalityValues(hash_table_entry** pHashTable, storm_event* pArrStorm, int pModVal, string pFileName) {
	bool proceed = false;
	ifstream inFile;
	string sLine;



	inFile.open(pFileName); // Opens the file.

	while (getline(inFile, sLine, '\n')) {
		fatality_event* tempFatality = new fatality_event();
		hash_table_entry* ptr2Hash;
		int counter = 0, hashIndex, stormIndex;
		string token;
		stringstream sstr(sLine);


		while (getline(sstr, token, ',') && proceed == true) {
			if (token != "") {
				switch (counter) {
					case 0: // Case for 'fatality_id'.
						tempFatality->fatality_id = stoi(token);
						break;

					case 1: // Case for 'event_id'.
						tempFatality->event_id = stoi(token);
						hashIndex = HashVal(tempFatality->event_id, pModVal);
						ptr2Hash = pHashTable[hashIndex];


						while (ptr2Hash->event_id != tempFatality->event_id) { ptr2Hash = ptr2Hash->next; } // Finds the matching 'event_id' and gives the index into the 'pArrStorm'.

						stormIndex = ptr2Hash->event_index;
						if (pArrStorm[stormIndex].f == NULL) { pArrStorm[stormIndex].f = tempFatality; } // Base case for starting the linked list of 'fatality_event'.
						else {
							tempFatality->next = pArrStorm[stormIndex].f;
							pArrStorm[stormIndex].f = tempFatality;
						}
						break;

					case 2: // Case for 'fatality_type'.
						tempFatality->fatality_type = (char)token[0];
						break;

					case 3: // Case for 'fatality_date'.
						strcpy(tempFatality->fatality_date, &token[0]);
						break;

					case 4: // Case for 'fatality_age'.
						tempFatality->fatality_age = stoi(token);
						break;

					case 5: // Case for 'fatality_sex'.
						tempFatality->fatality_sex = (char)token[0];
						break;

					case 6: // Case for 'fatality_location'.
						strcpy(tempFatality->fatality_location, &token[0]);
						break;
					}
			}
			counter++;
		}
		if (proceed == false) { delete tempFatality; } // Deletes the first 'fatality_event' object as it is unused.
		proceed = true; // Allows the method to begin storing values into the storm event array. Used to account for the header row.
	}
	inFile.close(); // Closes the file.
}




void LoadHashStormValues(hash_table_entry** pHashTable, storm_event* pArrStorm, int pModVal, string pFileName) {
	ifstream inFile;
	int stormIndex = -1; // Starts at -1 to account for the header row.
	string sLine;



	inFile.open(pFileName); // Opens the file.

	while (getline(inFile, sLine, '\n')) {
		hash_table_entry* tempHash = NULL;
		int counter = 0, hashIndex, multiplier;
		string token;
		stringstream sstr(sLine);


		while (getline(sstr, token, ',') && stormIndex > -1) {
			if (token != "") {
				switch (counter) {
				case 0: // Case for 'event_id'.
					pArrStorm[stormIndex].event_id = stoi(token); // Sets the 'event_id' of 'pArrStorm' to 'token' which equals the storm event ID in the file.
					pArrStorm[stormIndex].f = NULL; // Sets the fatality pointer to 'NULL'.

					tempHash = new hash_table_entry(); // Creates a new 'hash_table_entry' object.
					tempHash->event_id = stoi(token); // Sets 'tempHash->event_id' to the storm event ID.
					tempHash->event_index = stormIndex; // Sets 'tempHash->event_index' to the index of the storm event in the 'pArrStorm'.

					hashIndex = HashVal(stoi(token), pModVal); // Finds the index at which the current storm event belongs in the hash table.
					if (pHashTable[hashIndex] == NULL) { pHashTable[hashIndex] = tempHash; }
					else {
						tempHash->next = pHashTable[hashIndex]; // Sets 'tempHash->next' to the address of the indexed element in 'pHashTable'.
						pHashTable[hashIndex] = tempHash; // Sets the indexed element in 'pHashTable' to 'tempHash'.
					}
					break;

				case 1: // Case for 'state'.
					strcpy(pArrStorm[stormIndex].state, &token[0]);
					break;

				case 2: // Case for 'year'.
					pArrStorm[stormIndex].year = stoi(token);
					pHashTable[hashIndex]->year = stoi(token);
					break;

				case 3: // Case for 'month_name'.
					strcpy(pArrStorm[stormIndex].month_name, &token[0]);
					break;

				case 4: // Case for 'event_type'.
					strcpy(pArrStorm[stormIndex].event_type, &token[0]);
					break;

				case 5: // Case for 'cz_type'.
					pArrStorm[stormIndex].cz_type = (char)token[0];
					break;

				case 6: // Case for 'cz_name'.
					strcpy(pArrStorm[stormIndex].cz_name, &token[0]);
					break;

				case 7: // Case for 'injuries_direct'.
					pArrStorm[stormIndex].injuries_direct = stoi(token);
					break;

				case 8: // Case for 'injuries_indirect'.
					pArrStorm[stormIndex].injuries_indirect = stoi(token);
					break;

				case 9: // Case for 'deaths_direct'.
					pArrStorm[stormIndex].deaths_direct = stoi(token);
					break;

				case 10: // Case for 'deaths_indirect'.
					pArrStorm[stormIndex].deaths_indirect = stoi(token);
					break;

				case 11: // Case for 'damage_property'.
					if (token.substr(token.length() - 1, 1) == "K") { multiplier = pow(10, 3); }
					else if (token.substr(token.length() - 1, 1) == "M") { multiplier = pow(10, 6); }
					pArrStorm[stormIndex].damage_property = stod(token) * multiplier;
					break;

				case 12: // Case for 'damage_crops'.
					if (token.substr(token.length() - 1, 1) == "K") { multiplier = pow(10, 3); }
					else if (token.substr(token.length() - 1, 1) == "M") { multiplier = pow(10, 6); }
					pArrStorm[stormIndex].damage_crops = stod(token) * multiplier;
					break;

				case 13: // Case for 'tor_f_scale'.
					strcpy(pArrStorm[stormIndex].tor_f_scale, &token[0]);
					break;
				}
			}
			counter++; // Counter for loading the data members of the 'storm_event'.
		}
		stormIndex++; // Increments to the next element of 'pArrStorm'.
	}
	inFile.close(); // Closes the file.
}




void MaxHeapify(heap** pMaxHeap, int pos, int pSize) {
	int left = (2 * pos) + 1, right = (2 * pos) + 2, largest;



	if (left < pSize && pMaxHeap[left]->max > pMaxHeap[pos]->max) { largest = left; }
	else { largest = pos; }

	if (right < pSize && pMaxHeap[right]->max > pMaxHeap[largest]->max) { largest = right; }
	if (largest != pos) {
		swap(pMaxHeap[pos], pMaxHeap[largest]);
		MaxHeapify(pMaxHeap, largest, pSize);
	}
}




int Month2Int(string pMonth) {
	const int numOfMonths = 12;
	string months[numOfMonths] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };



	for (int i = 0; i < numOfMonths/2; i++) {
		if (pMonth == months[i]) { return i; }
		if (pMonth == months[numOfMonths - i - 1]) { return numOfMonths - i - 1; }
	}
	return -1;
}




bst* NewNode(char* ptr2Char, int pEventID) {
	bst* tempNode = new bst;
	tempNode->s = ptr2Char;
	tempNode->event_id = pEventID;
	tempNode->left = tempNode->right = NULL;
	return tempNode;
}




void PrintBST(annual_storms* pArrAnnual, hash_table_entry** pHashTable, int pModVal, bst* pBST, string pType) {
	int printMode = 0;
	


	if (pBST != NULL) {
		if (pType == "state") { printMode = 1; }
		else if (pType == "month_name") { printMode = 2; }

		PrintBST(pArrAnnual, pHashTable, pModVal, pBST->left, pType);
		FindEvent(pArrAnnual, pHashTable, pBST->event_id, pModVal, printMode);
		PrintBST(pArrAnnual, pHashTable, pModVal, pBST->right, pType); 
	}
}




void PrintFatality(fatality_event* ptr2Fatality) {
	while (ptr2Fatality != NULL) {
		cout
			<< "\n     Fatality ID: " << ptr2Fatality->fatality_id << endl
			<< "     Event ID: " << ptr2Fatality->event_id << endl
			<< "     Fatality Type: " << ptr2Fatality->fatality_type << endl
			<< "     Fatality Date: " << ptr2Fatality->fatality_date << endl
			<< "     Fatality Age: " << ptr2Fatality->fatality_age << endl
			<< "     Fatality Sex: " << ptr2Fatality->fatality_sex << endl
			<< "     Fatality Location: " << ptr2Fatality->fatality_location << endl;
		ptr2Fatality = ptr2Fatality->next;
	}
}




void PrintStormEvent(storm_event* pArrStorm, int pStormIndex, int pPrintMode) {
	cout << "  Event ID: " << pArrStorm[pStormIndex].event_id << endl;
	if (pPrintMode != 2) { cout << "  State: " << pArrStorm[pStormIndex].state << endl; }
	cout << "  Year: " << pArrStorm[pStormIndex].year << endl;
	if (pPrintMode != 1) { cout << "  Month: " << pArrStorm[pStormIndex].month_name << endl; }
	cout << "  Event Type: " << pArrStorm[pStormIndex].event_type << endl;
	cout << "  CZ Type: " << pArrStorm[pStormIndex].cz_type << endl;
	cout << "  CZ Name: " << pArrStorm[pStormIndex].cz_name << endl;
	if (pPrintMode != 0) { cout << endl; }
	if (pPrintMode == 0) { cout << "  Direct Injuries: " << pArrStorm[pStormIndex].injuries_direct << endl; }
	if (pPrintMode == 0) { cout << "  Indirect Injuries: " << pArrStorm[pStormIndex].injuries_indirect << endl; }
	if (pPrintMode == 0) { cout << "  Direct Deaths: " << pArrStorm[pStormIndex].deaths_direct << endl; }
	if (pPrintMode == 0) { cout << "  Indirect Deaths: " << pArrStorm[pStormIndex].deaths_indirect << endl; }
	if (pPrintMode == 0) { cout << "  Property Damage: $" << pArrStorm[pStormIndex].damage_property << endl; }
	if (pPrintMode == 0) { cout << "  Crop Damage: $" << pArrStorm[pStormIndex].damage_crops << endl; }
	if (pPrintMode == 0) { cout << "  F-scale: " << pArrStorm[pStormIndex].tor_f_scale << endl; }
	if (pPrintMode == 0) {
		cout << "  Fatalities: ";
		if (pArrStorm[pStormIndex].f == NULL) { cout << "\n     No fatalities." << endl; }
		else { PrintFatality(pArrStorm[pStormIndex].f); }
	}
}




void Query(annual_storms* pArrAnnual, int pAnnualSize, hash_table_entry** pHashTable, int pModVal) {
	string arrOptions[4] = { "find event ", "find max ", "find max fatality ", "range " }, numOfQueries;



	cout << "Please enter the number of queries that you would like to process: ";
	getline(cin, numOfQueries);

	for (int i = 0; i < stoi(numOfQueries); i++) {
		int counter = 0, eventID = 0, heapSize, num;
		string dmgType, high, line, low, str, type, year;


		cout << "\nEnter query " << i + 1 << ": ";
		getline(cin, line);

		if (string(line.substr(0, arrOptions[0].length())) == arrOptions[0]) { // Do 'find event'.
			stringstream str(line);

			
			while (getline(str, line, ' ')) {
				if (counter == 2) { eventID = stoi(line); };
				counter++;
			}
			FindEvent(pArrAnnual, pHashTable, eventID, pModVal, 0);
		}

		else if (string(line.substr(0, arrOptions[2].length())) == arrOptions[2]) { // do 'find max fatality'.
			stringstream str(line);
			

			while (getline(str, line, ' ')) {
				if (counter == 2) { type = line; }
				else if (counter == 3) { num = stoi(line); }
				else if (counter == 4) { year = line; }
				counter++;
			}

			if (num <= 50) {
				heapSize = FindHeapSize(pArrAnnual, pAnnualSize, year);
				heap** maxHeap = BuildMaxHeap(pArrAnnual, pAnnualSize, year, type, heapSize);

				for (int i = 0; i < num; i++) { heapSize = ExtractMax(maxHeap, heapSize, type); }
				delete[] maxHeap;
			}
			cout << "Max-Heap Total Nodes: " << heapSize << "\nMax-Heap Height: " << floor(log2(heapSize)) << "\nMax-Heap Left Subtree Height: "
				<< floor(log2(heapSize - 1)) - 1 << "\nMax-Heap Right Subtree Height: " << floor(log2(heapSize - 2)) - 1 << endl;
		}

		else if (string(line.substr(0, arrOptions[1].length())) == arrOptions[1]) { // do 'find max'.
			stringstream str(line);


			while (getline(str, line, ' ')) {
				if (counter == 2) { num = stoi(line); }
				else if (counter == 3) { year = line; }
				else if (counter == 4) { dmgType = line; }
				counter++;
			}
			
			if (num <= 50) {
				heapSize = FindHeapSize(pArrAnnual, pAnnualSize, year);
				heap** maxHeap = BuildMaxHeap(pArrAnnual, pAnnualSize, year, dmgType, heapSize);
				
				for (int i = 0; i < num; i++) { heapSize = ExtractMax(maxHeap, heapSize, dmgType); }
				delete[] maxHeap;
			}
			cout << "\nMax-Heap Total Nodes: " << heapSize << "\nMax-Heap Height: " << floor(log2(heapSize)) << "\nMax-Heap Left Subtree Height: "
				<< floor(log2(heapSize - 1)) - 1 << "\nMax-Heap Right Subtree Height: " << floor(log2(heapSize - 2)) - 1 << endl;
		}

		else if (string(line.substr(0, arrOptions[3].length())) == arrOptions[3]) { // do 'range'
			int total = 0, left = 0, right = 0;
			stringstream str(line);


			while (getline(str, line, ' ')) {
				if (counter == 1) { year = line; }
				else if (counter == 2) { type = line; }
				else if (counter == 3) { low = line; }
				else if (counter == 4) { high = line; }
				counter++;
			}
			bst* BST = BuildBST(pArrAnnual, pAnnualSize, year, type, low, high);
			
			if (BST == NULL) { cout << "  No storm events found for the given range." << endl << endl; }
			else { PrintBST(pArrAnnual, pHashTable, pModVal, BST, type); }
			
			cout << "BST Total Nodes: " << CountBSTNodes(BST);

			total = FindBSTHeight(BST);
			if (total > 0) {
				total--;
				left = FindBSTHeight(BST->left);
				right = FindBSTHeight(BST->right);
			}
			if (left > 0) { left--; }
			if (right > 0) { right--; }

			cout << "\nBST Root Height: " << total << "\nBST Left Subtree Height: " << left << "\nBST Right Subtree Height: " << right << endl;
		}

		else { cout << "  The query was unable to be processed." << endl; } // Default if the query does not match the criteria.
	}
}	



//Provided by Dr. Violet R. Syrotiuk @ Arizona State Univserity.
bool TestForPrime(int val) {
	int limit, factor = 2;


	limit = (long)(sqrtf((float)val) + 0.5f);
	while ((factor <= limit) && (val % factor)) { factor++; }

	return(factor > limit);
}
