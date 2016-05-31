#ifndef SIGNAL_H
#define SIGNAL_H

#include <vector>
#include <complex>

using namespace std;
class Signal
{
private:
	int length;
	vector<float> data;

public:
	Signal();
	Signal(int);
	~Signal();
	void ReadInSignal(const char*);
	void PrintSignal();
	int GetLength();
	float* GetSigPtr();
};
#endif