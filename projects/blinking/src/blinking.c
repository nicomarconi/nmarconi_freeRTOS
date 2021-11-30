/* Copyright 2017, Esteban Volentini - Facet UNT, Fi UNER
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

/** @file blibnking.c
 **
 ** @brief Ejemplo de un led parpadeando
 **
 ** | RV | YYYY.MM.DD | Autor       | Descripción de los cambios              |
 ** |----|------------|-------------|-----------------------------------------|
 ** |  1 | 2021.10.29 | evolentini  | Version inicial del archivo             |
 ** 
 ** @defgroup ejemplos Proyectos de ejemplo
 ** @brief Proyectos de ejemplo de la Especialización en Sistemas Embebidos
 ** @{ 
 */

/* === Inclusiones de cabeceras ============================================ */

#include <string.h>
#include <stdint.h>
#include "led.h"
#include "switch.h"
#include "chip.h"

/* === Definicion y Macros ================================================= */

/** Valor de la cuenta para la función de espera */
#define COUNT_DELAY 5000000

/* === Declaraciones de tipos de datos internos ============================ */

/* === Declaraciones de funciones internas ================================= */

/** @brief Función para generar demoras
 **
 ** Función basica que genera una demora para permitir el parpadeo de los leds
 */
void Delay(void);

/* === Definiciones de variables internas ================================== */

/* === Definiciones de variables externas ================================== */

/* === Definiciones de funciones internas ================================== */

void Delay(void) {
    uint32_t i;

    for(i = COUNT_DELAY; i != 0; i--) {
        __asm__("nop");
    }
}

/* === Definiciones de funciones externas ================================== */
int main(void) {
    /* Configuración de los dispositivos de entrada/salida */
    Init_Leds();
    Init_Switches();

    while (1) {
        Led_Toggle(YELLOW_LED);
        Delay();
    }

    return 0;
}

/* === Ciere de documentacion ============================================== */

/** @} Final de la definición del modulo para doxygen */
