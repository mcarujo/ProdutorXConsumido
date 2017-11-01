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
#include <mutex> 
#include <stdio.h>
#include <string.h>
#include <pthread.h> // biblioteca pthread
#include <semaphore.h>
#include <cmath>
#include "adc.h"

#define UNIT_MS 1000
#define UNIT_SEC 1000000

using namespace std;

/****************** Variaveis Globais ***********************/
pthread_mutex_t work_mutex;
int numero = 0;

BlackLib::BlackGPIO entrada(BlackLib::GPIO_68, BlackLib::input, BlackLib::SecureMode);
// Display
Display display(BlackLib::GPIO_65, BlackLib::GPIO_45,
					BlackLib::GPIO_69, BlackLib::GPIO_60, BlackLib::GPIO_27, BlackLib::GPIO_66,
					BlackLib::GPIO_49, BlackLib::GPIO_115,
					BlackLib::GPIO_20, BlackLib::GPIO_47,
					BlackLib::GPIO_48, BlackLib::GPIO_46);
					

void *thread_consumidor(void *arg);
void *thread_produtor(void *arg);
int controle(int numero,bool action);

/****************** FIM de Globais ***********************/

int main()
{
	int res, valor=0;
	pthread_t consumdior, produtor;
	void * statusFinalizacao;
	
	
	while(1){
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

		/*
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
		*/
		while(entrada.getValue() == "1");
		pthread_cancel(consumdior);
		pthread_cancel(produtor);
		display.showNumber(0);
		pthread_mutex_unlock(&work_mutex);
		numero = 0;
		sleep(1);
	}
}

void *thread_produtor(void *valor)
{
	//BlackLib::BlackGPIO entrada_produtor(BlackLib::GPIO_68, BlackLib::input, BlackLib::SecureMode);
	int prevType;
   	 pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &prevType);
	float perc1;
	ADC pot1(AIN0); // pot1 = consumidor
	
	while (1)
	{
	
		perc1 = pot1.getPercentValue();	
		
		//pthread_mutex_lock(&work_mutex); // Entrando na região crítica
		
		//printf("Produtor com perc = %d\n",(int) floor(perc1));
		usleep(perc1 * 100 * UNIT_MS); // Espera muito ou pouco
		
		pthread_mutex_lock(&work_mutex); // Entrando na região crítica
		numero = controle(numero, true);
		display.showNumber(numero);
		//printf("%d\n", numero);
		
		pthread_mutex_unlock(&work_mutex); // saindo de região critica
		usleep(100 * UNIT_MS);
		//pthread_mutex_unlock(&work_mutex); // saindo de região critica
	}

}

void *thread_consumidor(void *valor)
{
	int prevType;
   	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &prevType);
	float perc2;
	ADC pot2(AIN1); // pot2 = produtor
	
	//BlackLib::BlackGPIO entrada_consumidor(BlackLib::GPIO_68, BlackLib::input, BlackLib::SecureMode);
	while (1)
	{
		perc2 = pot2.getPercentValue();
		
		//printf("Consumidor com perc = %d\n",(int) floor(perc2));
		usleep(perc2  * 100 *  UNIT_MS); // Espera muito ou pouco
		
		pthread_mutex_lock(&work_mutex); // Entrando na região crítica
		numero = controle(numero, false);
		display.showNumber(numero);
		//printf("%d\n", numero);
		pthread_mutex_unlock(&work_mutex); // saindo de região critica
		
		usleep(100 * UNIT_MS);
		//pthread_mutex_unlock(&work_mutex); // saindo de região critica
		
		
	}

}

int controle(int numero, bool action)
{
	if (action) // Produtor = true
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
