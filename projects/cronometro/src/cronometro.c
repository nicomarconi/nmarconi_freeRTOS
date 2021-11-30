/*Copyright 2021, Nicolás Marconi -FIUNER
 * Copyright 2017, Esteban Volentini - Facet UNT, Fi UNER
 * Copyright 2014, 2015 Mariano Cerdeiro
 * Copyright 2014, Pablo Ridolfi
 * Copyright 2014, Juan Cecconi
 * Copyright 2014, Gustavo Muro
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** @file cronometro.c
 **
 ** @brief La app implementa un cronometro con las siguientes caracteristicas
 **    - El botón TEC1 debe iniciar y detener la cuenta.
 **    - El botón TEC2 debe volver la cuenta a cero solo si esta detenido.
 **    - Mientras la cuenta esta corriendo deben parpadear el led RGB en verde
 **    - Mientras la cuenta esta detenida el led RGB debe permanecer en rojo
 **    - Si el cronometro esta funcionando y la cuenta se muestra en la
 **      pantalla la tecla TEC3 debe congelar el tiempo parcial y mantener la
 **      cuenta activa.
 **    - Si se esta mostrando un tiempo parcial la tecla TEC3 debe volver a
 **      mostrar la cuenta actual de cronometro.
 **
 ** Ejemplo de un led parpadeando utilizando la capa de abstraccion de
 ** hardware y con sistema operativo FreeRTOS.
 **
 ** | RV | YYYY.MM.DD | Autor       | Descripción de los cambios              |
 ** |----|------------|-------------|-----------------------------------------|
 ** |  3 | 2021.11.29 |Nico Marconi | Nueva versión NM
 ** |  2 | 2017.10.16 | evolentini  | Correción en el formato del archivo     |
 ** |  1 | 2017.09.21 | evolentini  | Version inicial del archivo             |
 **
 ** @defgroup ejemplos Proyectos de ejemplo
 ** @brief Proyectos de ejemplo de la Especialización en Sistemas Embebidos
 ** @{
 */

/* === Inclusiones de cabeceras ============================================ */
#include "FreeRTOS.h"
#include "task.h" //acceder a funciones de tareas
#include "soc.h" //systick init
#include "led.h"
#include "ili9341.h"
#include <string.h>
#include <stdio.h>
#include "board.h"
#include "switch.h"

//#include "fonts.h" //incluida dentro de ii9341.h

/* === Definicion y Macros ================================================= */

#define SPI_1 1  /*!< EDU-CIAA SPI port */
#define GPIO_0 0 /*!< EDU-CIAA GPIO0 port */
#define GPIO_1 1 /*!< EDU-CIAA GPIO1 port */
#define GPIO_2 2 /*!< EDU-CIAA GPIO2 port */
#define GPIO_3 3 /*!< EDU-CIAA GPIO3 port */


/* === Declaraciones de tipos de datos internos ============================ */

typedef struct  {
	uint8_t hora;
	uint8_t min;
	uint8_t seg;
}param_crono;

uint32_t segundos = 0;
uint8_t dseg = 0; //decimas de segundo
uint8_t estado = 0; //0=detenido 1=corriendo 2=parcial


/* === Declaraciones de funciones internas ================================= */


void Blinking(void *parametros);  //parpadeo de led mientras esta corriendo
void teclaPresionada(void *parametros); //actua segun la tecla, incluye antirebote
void Cronometro(void *parametros); //lleva el tiempo transcurrido
void refrescoPantalla(void *parametros);//mostrar cambios en TFT


/* === Definiciones de variables internas ================================== */

/* === Definiciones de variables externas ================================== */

/* === Definiciones de funciones internas ================================== */

void Blinking(void *argumentos)
{
   while (1)
   { /* Si estado es 1 entonces el led verde parpadea, si el estado es otro se apaga*/
      if (estado==1)  Led_Toggle(RGB_G_LED);
      else Led_Off(RGB_G_LED);
      vTaskDelay(500 / portTICK_PERIOD_MS);
   }
}

void Cronometro(void *argumentos){
   param_crono * parametros = argumentos;
	TickType_t lasttick = xTaskGetTickCount();
	//uint32_t segundos = 0;
	while(1)
	{
		if (dseg==9) segundos++; {
		 if(segundos==86400)segundos = 0;
		dseg %= 10;
		dseg++;
		parametros->hora = segundos/3600;
		parametros->min = (segundos)/60;
		parametros->seg = segundos -(parametros->hora*3600) - ( parametros->min*60);
		if (parametros->hora == 24)parametros->hora=0;
		if (parametros->min == 60)parametros->min = 0;
		if (parametros->seg == 60)parametros->seg=0;
	}
		// cuenta 0,01 segundos cada vez, por tanto cada vez que llegue a 0,09, se cumple 1 segund.
		vTaskDelayUntil(&lasttick, 100 / portTICK_RATE_MS);// evita un retraso continuio, haciendo que se despierte 100 ms
		                                                   //  después de 100 ms fijos
	}
}


void refrescoPantalla(void *argumentos){
	param_crono *valores_crono = argumentos;
	TickType_t lasttick = xTaskGetTickCount();

	char stpoCompleto[10]; //string

	while(1){ //0=detenido 1=corriendo 2=parcial
		if (estado==1)	{

			sprintf(stpoCompleto,"%02d:%02d:%02d,%02d",valores_crono->hora,valores_crono->min,valores_crono->seg,dseg);
			ILI9341DrawString(5, 25, stpoCompleto, &font_16x26, ILI9341_BLACK, ILI9341_WHITE);
			}
		vTaskDelayUntil(&lasttick, 100 / portTICK_RATE_MS);

	}

}

void teclaPresionada(void *argumentos){

	uint8_t teclaActual=0; //estado de la tecla
	uint8_t teclaAnterior=0; //ultimo estado de la tecla
	while(1){
		teclaActual = Read_Switches();
		if (teclaActual != teclaAnterior){//Antirrebote

			switch(teclaActual){
				case 1:
						if (estado==0){//TECLA 1
							estado=1;
							Led_Off(RGB_R_LED);
							}
						else if (estado==1) {
							estado=0;
							Led_On(RGB_R_LED);
							}

						break;

				case 2: if (estado==0) {//TECLA 2
					    segundos = 0;
					    dseg = 0;
						ILI9341DrawString(5, 25, "00:00:00,00", &font_16x26, ILI9341_BLACK, ILI9341_WHITE);
						Led_Off(RGB_R_LED);
						}
						break;
				case 4: { //TECLA 3
						if (estado==1) estado=2;
						else if (estado==2) estado=1;
						break;
						}

			}
			teclaAnterior = teclaActual;
		}
		vTaskDelay(70 / portTICK_PERIOD_MS);
	}
}

/* === Definiciones de funciones externas ================================== */

/** @brief Función principal del programa
 **
 ** @returns 0 La función nunca debería termina
 **
 ** @remarks En un sistema embebido la función main() nunca debe terminar.
 **          El valor de retorno 0 es para evitar un error en el compilador.
 **
 ** |  	ILI9341	  	|   EDU-CIAA 	|
 ** |:-------------:|:--------------|
 ** | 	SDO/MISO 	|	SPI_MISO	|
 ** | 	LED		 	| 	3V3			|
 ** | 	SCK		 	| 	SPI_SCK		|
 ** | 	SDI/MOSI 	| 	SPI_MOSI	|
 ** | 	DC/RS	    | 	GPIO2		|
 ** | 	RESET	    | 	GPIO3		|
 ** | 	CS		    | 	GPIO0		|
 ** | 	GND		 	| 	GND			|
 ** | 	VCC		 	| 	3V3			|
 **  Lo siguiente son DEFINE de board.h
 ** #define LED_RED                     0
 ** #define LED_GREEN                   1
 ** #define LED_BLUE                    2
 ** #define LED_1                       3
 ** #define LED_2                       4
 ** #define LED_3                       5
 **
 ** #define BOARD_TEC_1                 0
 ** #define BOARD_TEC_2                 1
 ** #define BOARD_TEC_3                 2
 ** #define BOARD_TEC_4                 3
 **
 ** #define BOARD_GPIO_0                0
 ** #define BOARD_GPIO_1                1
 ** #define BOARD_GPIO_2                2
 ** #define BOARD_GPIO_3                3
 ** #define BOARD_GPIO_4                4
 ** #define BOARD_GPIO_5                5
 ** #define BOARD_GPIO_6                6
 ** #define BOARD_GPIO_7                7
 ** #define BOARD_GPIO_8                8
 */
int main(void)
{
   static param_crono *param;
   /* Inicializaciones y configuraciones de dispositivos */
   Board_Init();
   Init_Leds;

   ILI9341Init(SPI_1, GPIO_0, GPIO_2, GPIO_3);
   ILI9341Rotate(ILI9341_Portrait_2);
   ILI9341DrawString(5, 25, "00:00:00,00", &font_16x26, ILI9341_BLACK, ILI9341_WHITE);

   /* Creación de las tareas */
   xTaskCreate(Blinking, "Blinking", configMINIMAL_STACK_SIZE, &param, tskIDLE_PRIORITY + 1, NULL);
   xTaskCreate(Cronometro, "Cronometro", configMINIMAL_STACK_SIZE,  &param, tskIDLE_PRIORITY + 1, NULL);
   xTaskCreate(refrescoPantalla, "refrescoPantalla", 128,  &param, tskIDLE_PRIORITY + 1, NULL);
   xTaskCreate(teclaPresionada, "teclaPresionada", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

   /* Arranque del sistema operativo */
   vTaskStartScheduler();

   /* vTaskStartScheduler solo retorna si se detiene el sistema operativo */
   while(1);

   /* El valor de retorno es solo para evitar errores en el compilador*/

   return 0;
}
/* === Ciere de documentacion ============================================== */
/** @} Final de la definición del modulo para doxygen */
