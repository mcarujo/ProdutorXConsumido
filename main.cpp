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
#include <vector>
#include <cmath>
#include "adc.h"

#define UNIT_MS 1000
#define UNIT_SEC 1000000

using namespace std;

vector<int> buffer;//Declarando Buffer
/****************** Variaveis Globais ***********************/
pthread_mutex_t work_mutex;

BlackLib::BlackGPIO entrada(BlackLib::GPIO_68, BlackLib::input, BlackLib::SecureMode);
// Display
Display display(BlackLib::GPIO_65, BlackLib::GPIO_45,
					BlackLib::GPIO_69, BlackLib::GPIO_60, BlackLib::GPIO_27, BlackLib::GPIO_66,
					BlackLib::GPIO_49, BlackLib::GPIO_115,
					BlackLib::GPIO_20, BlackLib::GPIO_47,
					BlackLib::GPIO_48, BlackLib::GPIO_46);
					

void *thread_consumidor(void *arg);
void *thread_produtor(void *arg);
void controle(bool action);

/****************** FIM de Globais ***********************/

int main()
{	
	buffer.reserve(7);
	buffer.clear();
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

		while(entrada.getValue() == "1");
		pthread_cancel(consumdior);
		pthread_cancel(produtor);
		display.showNumber(0);
		pthread_mutex_unlock(&work_mutex);
		buffer.clear();//Resetando Buffer 
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
		
		while(buffer.size()==7);//Espera
		controle(true);//Produz Buffer
		//printf("%d\n", buffer);
		
		
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
		while(!buffer.empty());//Espera Buffer
		controle(false);//Consume Buffer
		//printf("%d\n", buffer);
		usleep(100 * UNIT_MS);
		//pthread_mutex_unlock(&work_mutex); // saindo de região critica
		
		
	}

}

void controle(bool action)
{
	if(action)
	{
		pthread_mutex_lock(&work_mutex); // Entrando na região crítica
		buffer.push_back(1);
		display.showNumber(buffer.size());
		pthread_mutex_unlock(&work_mutex); // saindo de região critica

	}
	else
	{
		pthread_mutex_lock(&work_mutex); // Entrando na região crítica
		buffer.pop_back();
		display.showNumber(buffer.size());
		pthread_mutex_unlock(&work_mutex); // saindo de região critica

	}


}
