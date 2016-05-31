// The class is designed to storing signal
#include "Signal.h"
#include <iostream>
#include <fstream>
#include <iterator>
using namespace std;

Signal::Signal()
{

}

Signal::Signal(int sig_len)
{
	data.reserve(sig_len);
	length = sig_len;
}

Signal::~Signal()
{
	data.clear();
}

void Signal::ReadInSignal(const char* fname)
{
	// from stackoverflow, read in files
	std::ifstream f(fname);
	char c;
	int line_count = 0;
	while (f.get(c))
	    if (c == '\n')
	        ++line_count;

	// create vector for storing signal
	data.reserve(line_count);
	// set up input file
	ifstream in(fname,ios::in);
	float tmp_dpoint;
	length = 0;
	printf("\nStart to read in signal\n");
	while (in >> tmp_dpoint) {
		//add data point to vector
		data[length] = tmp_dpoint;
		length++;
	}
	printf("Signal length: %d\n", length);
	in.close();
}

// print first and last few elements for diagnostic purpose
void Signal::PrintSignal()
{
	if (length>6){
		printf("%f,%f,%f,...,%f,%f,%f\n",data[0], data[1],data[2], 
			data[length-3], data[length-2],data[length-1]);
	}
	else{
		for (int i=0; i<length;i++){
			printf("%f,",data[i]);
		}
		printf("\n");
	}
}

// return signal length
int Signal::GetLength()
{
	return length;
}

// get pointer to first element
float* Signal::GetSigPtr(){
	return data.data();
}