#include <iostream>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include "BlackTime/BlackTime.h"
#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "display.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h> // biblioteca pthread

#define UNIT_MS 1000
#define UNIT_SEC 1000000

using namespace std;

/****************** Variaveis Globais ***********************/
	// GPIO
	BlackLib::BlackGPIO saida(BlackLib::GPIO_67, BlackLib::output, BlackLib::SecureMode);
	BlackLib::BlackGPIO entrada(BlackLib::GPIO_68, BlackLib::input, BlackLib::SecureMode);

	// Variáveis relacioandas ao tempo
	int numero;
	float velocidade1,velocidade2;
	// Display
	Display display(BlackLib::GPIO_65, BlackLib::GPIO_45,
					BlackLib::GPIO_69, BlackLib::GPIO_60, BlackLib::GPIO_27, BlackLib::GPIO_66,
					BlackLib::GPIO_49, BlackLib::GPIO_115,
					BlackLib::GPIO_20, BlackLib::GPIO_47,
					BlackLib::GPIO_48, BlackLib::GPIO_46);

	void *thread_function(void *arg);
	int controle(int numero);

/****************** FIM de Globais ***********************/

int main()
{
	int res;
	pthread_t consumdior, produtor;
	void *thread_result;
	string valor;

	printf("MAIN()--> Criar thread...\n");
	valor = "consumidor";
	res = pthread_create(&thread1, NULL, thread_function, (void *)&valor);
	if (res != 0)
	{
		perror("A Craição da Thread falhou");
		exit(EXIT_FAILURE);
	}
	valor = "produtor";
	res = pthread_create(&thread1, NULL, thread_function, (void *)&valor);
	if (res != 0)
	{
		perror("A Craição da Thread falhou");
		exit(EXIT_FAILURE);
	}
	return 0;
}

void *thread_function(void *valor)
{
	if (valor == "consumidor")
	{
		while (1)
		{
		
		}
	}
	else if (valor == "produtor")
	{
		while (1)
		{
		}
	}
	else
	{
		cout << "\nDeu erro amigão!\n";
	}
}

int controle(int numero, bool action)
{
	if (action)
	{
		numero+= 1;
	}
	else
	{
		numero-= 1;
	}
	if (numero > 7)
	{
		numero = 7;
	}
	if (numero < 0)
	{
		numero = 0;
	}

	return numero;
}