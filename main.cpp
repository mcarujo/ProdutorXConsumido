#include <iostream>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
//#include "BlackTime/BlackTime.h"
//#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
//#include "display.h"
#include <mutex> 
#include <stdio.h>
#include <string.h>
#include <pthread.h> // biblioteca pthread

#define UNIT_MS 1000
#define UNIT_SEC 1000000

using namespace std;

/****************** Variaveis Globais ***********************/
std::mutex mtx;
int numero;
// Display
/*Display display(BlackLib::GPIO_65, BlackLib::GPIO_45,
					BlackLib::GPIO_69, BlackLib::GPIO_60, BlackLib::GPIO_27, BlackLib::GPIO_66,
					BlackLib::GPIO_49, BlackLib::GPIO_115,
					BlackLib::GPIO_20, BlackLib::GPIO_47,
					BlackLib::GPIO_48, BlackLib::GPIO_46);
		*/
void *thread_consumidor(void *arg);
void *thread_produtor(void *arg);
int controle(int numero);

/****************** FIM de Globais ***********************/

int main()
{
	int res, valor=0;
	pthread_t consumdior, produtor;
	void * statusFinalizacao;
	
	//display.showNumber(0000);

	printf("Programa principal criando thread consumidor...\n");
	res = pthread_create(&consumdior, NULL, thread_consumidor, (void *)&valor);
	if (res != 0)
	{
		perror("A Craição da Thread consumidor falhou");
		exit(EXIT_FAILURE);
	}


	printf("Programa principal criando thread produtor...\n");
	res = pthread_create(&produtor, NULL, thread_produtor, (void *)&valor);
	if (res != 0)
	{
		perror("A Craição da Thread produtor falhou");
		exit(EXIT_FAILURE);
	}


	printf("Programa principal esperando pelo término das threads...\n");
    res = pthread_join(consumdior, &statusFinalizacao);
    if (res != 0) {
        perror("O thread_join falhou");
        exit(EXIT_FAILURE);
    }
	res = pthread_join(produtor, &statusFinalizacao);
    if (res != 0) {
        perror("O thread_join falhou");
        exit(EXIT_FAILURE);
    }
   
    exit(EXIT_SUCCESS);
}

void *thread_produtor(void *valor)
{
	BlackLib::BlackGPIO entrada_produtor(BlackLib::GPIO_68, BlackLib::input, BlackLib::SecureMode);
	int numero=0,buffer=0;
	while (1)
	{
		//while(entrada_produtor.getValue() == "1");
		//sleep(entrada_produtor);

		mtx.lock();
		numero = controle(numero,true);
		//display.showNUmber(numero);
		mtx.unlock();
	}

}

void *thread_consumidor(void *valor)
{
	
	BlackLib::BlackGPIO entrada_consumidor(BlackLib::GPIO_68, BlackLib::input, BlackLib::SecureMode);
	while (1)
	{
		
		
		//while(entrada_consumidor.getValue() == "1");
		//sleep(entrada_consumidor);

		mtx.lock();
		numero = controle(numero,false);
		//display.showNUmber(numero);
		mtx.unlock();
	}

}


int controle(int numero, bool action)
{
	if (action)
	{
		numero += 1;
	}
	else
	{
		numero -= 1;
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