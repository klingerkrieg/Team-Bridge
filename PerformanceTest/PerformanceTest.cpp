// PerformanceTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <time.h>
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <conio.h>

using namespace std;


//Quantidade de repetições
const int repMax = 300;


// fonte: https://www.vivaolinux.com.br/script/Estatistica-Moda-e-mediana-de-um-vetor
/* Sub-rotina para ordenar elementos de um
vetor de forma crescente */
void ordenaCrescente(double vet[], int faixa) {
	register int i, j; // indexadores.
	double aux;  // variavel auxiliar.

	for ( i = 0; i < (faixa - 1); i++ )
		for ( j = i + 1; j < faixa; j++ )
			if ( vet[i] > vet[j] ) {
				aux = vet[i];
				vet[i] = vet[j];
				vet[j] = aux;
			}
}


//fonte: https://www.vivaolinux.com.br/script/Estatistica-Moda-e-mediana-de-um-vetor
/* Funcao que retorna o valor da mediana de um
vetor de numeros de ponto flutuante */

double mediana(double vet[], int faixa) {
	double m1, m2;

	ordenaCrescente(vet, faixa); // Ordenando conjunto numerico.

	switch ( faixa % 2 ) // Seletor para calculo da mediana.
	{
		case 0: // Faixa de valores (qtd de elem do vetor) e PAR.
			m1 = vet[faixa / 2 - 1];
			m2 = vet[faixa / 2];
			m1 += m2;
			return  (m1 / 2);

		case 1: // Faixa de valores do vetor e IMPAR.
			m1 = vet[(faixa - 1) / 2];
			return m1;
	}
}

double desvioPadrao(double vet[], int faixa, double media) {
	double variancia = 0;

	for ( int i = 0; i < repMax; i++ ) {
		variancia += pow(media - vet[i],2);
	}

	variancia = variancia / (faixa - 1);
	return sqrt(variancia);
}


//fonte: https://stackoverflow.com/questions/10905892/equivalent-of-gettimeday-for-windows
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



int main() {
	struct timeval tp;
	std::cout.precision(14);
	double ant = 0;
	double count = 1;
	double med = 0;
	int i = 0;
	double medianaVec[repMax];
	double min = 100;
	double max = 0;
	double atual;
	double desvioP;
	int countDentroDesvioP = 0;


	cout << ("Inicie o teste:\n");
	while ( i-3 < repMax ) {
		char c = _getch();
		gettimeofday(&tp, NULL);
		
		double now = (double)tp.tv_sec + ((double)tp.tv_usec / 1000000.0);

		if ( i > 2 ) {
			atual = (now - ant);
			medianaVec[i-3] = atual;
			med += atual;
			printf("Tempo: %.4f\t", atual);
			printf("Media: %.4f\n", (med / count));
			count++;

			if ( atual > max ) {
				max = atual;
			} else 
				if ( atual < min ) {
				min = atual;
			}
		}
		
		ant = (double)tp.tv_sec + ((double)tp.tv_usec / 1000000.0);
		i++;
	}



	if ( i - 3 == repMax ) {
		double media = (med / count);
		printf("Mediana: %.4f\n", mediana(medianaVec, repMax));
		desvioP = desvioPadrao(medianaVec, repMax, media);
		printf("Desvio padrao: %.4f\n", desvioP);
		for ( i = 0; i < repMax; i++ ) {
			if ( medianaVec[i] > media - desvioP && medianaVec[i] < media + desvioP ) {
				countDentroDesvioP++;
			}
		}
		cout << countDentroDesvioP * 100 / repMax << "% das amostras dentro do desvio padrao.\n";


		printf("Min: %.4f\n", min);
		printf("Max: %.4f\n", max);
		Sleep(10000);
	}

    return 0;
}

