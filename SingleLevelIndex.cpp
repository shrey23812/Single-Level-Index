/******************************************************************************
 * C++ program to create a single level index.
 *
 * This program is written as the submission of home work.
 *
 * The purpose of this program is to create a single level index and understand
 * how these things work at the lower level.
 *
 * It takes 4 command line arguments:
 *
 * 1. -c or -l:
 *      -c = create an index
 *      -l = list the records in order of the indexes
 *
 * 2. Name of the data file:
 *      This is supposed to be a text file.
 *
 * 3. Name of the index file:
 *      This is supposed to be a binary file.
 *
 * 4. A number between 1 and 24 (both inclusive):
 *      This is the length of the key.
 *
 * Written by Shrey Shah (sxs190184) at The University of Texas at Dallas
 * starting November 7, 2019
 ******************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <cstring>

using namespace std;

union charLong
{
long long lpart;
char cpart[8];
};
charLong cl;

//Index Structure to keep key and pointer.
struct Index
{
	char key[1024];
	long long pointer;
};

// Function to create index
void createIndex(string dataFile, string indexFile, int keyLength){
	ifstream dtfile;
	dtfile.open(dataFile, ifstream::binary);
	ofstream idxfile;
	idxfile.open(indexFile, ios::binary);
	string line;
	int i,j;
	long long pointer;
	Index temp;
 
	dtfile.seekg(0);
	int recordCount = 0;
	while(getline(dtfile,line)){
		recordCount++;
	}


	dtfile.clear();

 	vector <Index> idxArray;

 	//Go to beginning of file
	dtfile.seekg(0,ios::beg);

	//Add index to vector
	for(int i = 0; i<recordCount; i++){
		pointer = dtfile.tellg();
		getline(dtfile,line);
		Index in;
		for(j=0;j<keyLength;j++){
            in.key[j] = line[j];
          }
          in.key[j]='\0';
		in.pointer = pointer;
		idxArray.push_back(in);
	}

	//Sorting of vector
	for (i=0;i<idxArray.size()-1;i++) {
          for(j=0;j<idxArray.size()-1-i;j++){
            if(strcmp(idxArray[j].key,idxArray[j+1].key)>0){
                temp = idxArray[j];
                idxArray[j] = idxArray[j+1];
                idxArray[j+1] = temp;
            }
          }
      }
	vector<Index>::iterator itv;

	int bufptr;
	char buffer[1024];

	//Write indexes to file
	for(itv=idxArray.begin(); itv!=idxArray.end(); ++itv){
		bufptr = 0;
        cl.lpart = (*itv).pointer;
        memset(&buffer[bufptr], 0, 1024);
        memcpy_s(&buffer[bufptr], keyLength, (*itv).key, keyLength);
        bufptr+=keyLength;
        memcpy_s(&buffer[bufptr], 8, cl.cpart, 8);
        idxfile.write(buffer,keyLength+8);
	}
	cout<<"Index file created!";

	//close files
	dtfile.close();
	idxfile.close();
}

//Function to list the data records to console by reading from index
void listIndex(string dataFile, string indexFile, int keyLength){
	ifstream dtfile;
	dtfile.open(dataFile, ifstream::binary);
	ifstream idxfile;
	idxfile.open(indexFile, ios::binary | ios::ate);
	
	int size,recordCount;
	long long pointer;
	string record;
	char *line = new char[keyLength+1024];

	size = idxfile.tellg();
    idxfile.seekg(0, ios::beg);
    recordCount = size/(keyLength+8);
	
	dtfile.seekg(0);
	idxfile.seekg(0);

	int buffersize = keyLength+8;
	char buffer[1024];


	for(int i = 0; i < recordCount; i++ ){
		idxfile.read(buffer, buffersize);
		memcpy_s(cl.cpart, 8, (buffer+keyLength), 8);
		pointer = cl.lpart;
		dtfile.seekg(pointer,ios::beg);
		getline(dtfile,record);
		// dtfile.getline(record,1028);
		cout<<record<<endl;
	}

	//close files
	dtfile.close();
	idxfile.close();
}


int main(int argc, char const *argv[])
{
	//Should have 5 arguments
	if(argc != 5) {
		cout<<"Not enough command line arguments";
		exit(EXIT_FAILURE);
	}

	string dataFile = argv[2];
	string indexFile = argv[3];
	int keyLength = atoi(argv[4]);

	//key length should be between 1 and 24
	if(keyLength<1 || keyLength>24){
		cout<<"Key length is between 1 and 24";
		exit(EXIT_FAILURE);
	}

	if(strcmp("-c",argv[1]) == 0){
		createIndex(dataFile, indexFile, keyLength);
	}
	else if(strcmp("-l",argv[1]) == 0){
		listIndex(dataFile, indexFile, keyLength);
	}
	else {
		cout<<"First parameter should be -c or -l";
		exit(EXIT_FAILURE);
	}
	return 0;
}
