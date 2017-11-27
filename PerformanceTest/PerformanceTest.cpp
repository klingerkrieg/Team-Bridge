// PerformanceTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <time.h>
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <conio.h>

using namespace std;

int gettimeofday(struct timeval * tp, struct timezone * tzp) {
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
	// until 00:00:00 January 1, 1970 
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}



int main(){
	struct timeval tp;
	std::cout.precision(14);
	double ant = 0;
	double count = 0;
	double med = 0;
	int i = 0;

	cout << ("Inicie o teste:\n");
	while ( true ) {
		char c = _getch();
		gettimeofday(&tp, NULL);
		
		double now = (double)tp.tv_sec + ((double)tp.tv_usec / 1000000.0);

		if ( i > 2 ) {
			med += now - ant;
			printf("Tempo: %.4f\t", (med / count));
			printf("Media: %.4f\n", (med / count));
			count++;
		}
		
		ant = (double)tp.tv_sec + ((double)tp.tv_usec / 1000000.0);
		i++;
	}

    return 0;
}

