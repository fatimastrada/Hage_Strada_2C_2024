/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 18/10/2024 | Document creation		                         |
 * | 22/10/2024 | distance_task									 |
 * | 22/10/2024 | weight_task									 |
 * | 22/10/2024 | Distance_task									 |
 * | 22/10/2024 | leds related functions						 |
 * | 22/10/2024 | disinfect function							 |
 *
 * @author Hage Boris (boris.hage@ingenieria.uner.edu.ar)
 * @author Strada Fatima (fatima.strada@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "switch.h"		// Teclas
#include "gpio_mcu.h"	// GPIO
#include "hx711.h" 		// Balanza
#include "servo_sg90.h"	// Servo
#include "hc_sr04.h" 	// Ultrasonido
#include "buzzer.h"		// Buzzer
/*==================[macros and definitions]=================================*/
#define READING_DISTANCE_DELAY 100	// Tiempo en ms para medir distancia
#define DELAY_TO_CLOSE 3000			// Tiempo en ms para cerrar la tapa
#define DISTANCE_TO_OPEN 15			// Distancia en cm a la que se abre la tapa
#define OPENED_SERVO_ANGLE 90		// Angulo en grados de la tapa abierta
#define CLOSED_SERVO_ANGLE -90		// Angulo en grados de la tapa cerrada
#define MAX_WEIGHT 300				// Peso maximo en gramos
#define READING_WEIGHT_DELAY 300	// Tiempo en ms para medir el peso
#define SCALE_GAIN 100				// Ganancia de la balanza
#define DELAY_TO_DESINFECT 5000		// Tiempo para desinfectar luego de vaciar
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/** @fn disinfect
 * @brief Se encarga de enviar el pulso que activa la desinfeccion
 */
void disinfect()
{
	GPIOOn(GPIO_10);	// *Hay que definir cual es el gpio a utilizar
	vTaskDelay(10);
	GPIOOff(GPIO_10);
}

/** @fn green_light
 * @brief Apaga el led rojo y enciende el led verde
 */
void green_light()
{
	GPIOOff(GPIO_12);	// Apagar el gpio que enciende el led rojo
	GPIOOn(GPIO_11);	// Encender el gpio que enciende el led verde
}

/** @fn red_light
 * @brief Apaga el led verde y enciende el led rojo
 */
void red_light()
{
	GPIOOff(GPIO_11);	// Apagar el gpio que enciende el led verde
	GPIOOn(GPIO_12);	// Encender el gpio que enciende el led rojo
}

/** @fn blink_leds
 * @brief Hace titilar los leds verde y rojo
 */
void blink_leds()
{
	GPIOToggle(GPIO_11);	// Cambia de estado el gpio que enciende el led verde
	GPIOToggle(GPIO_12); 	// Cambia de estado el gpio que enciende el led rojo
}

/** @fn distance_task
 * @brief Tarea que se encarga de leer la distancia y llamar a las funciones de abrir y cerrar la tapa
 * @param pvParameter Puntero utilizado por el sistema operativo para administrar las tareas
 */
void distance_task(void *pvParameter)
{	
	uint8_t counter=0;
	while(true){	// Se crea un bucle que siempre corre cuando el sistema operativo lo asigna
		if(HcSr04ReadDistanceInCentimeters()<=DISTANCE_TO_OPEN){
			counter=0;			// Contador para el delay de cerrar la tapa
			//ServoMove(SERVO_0,OPENED_SERVO_ANGLE);
		}
		else{
			if(counter<(DELAY_TO_CLOSE/READING_DISTANCE_DELAY)){
				counter++;
			}
			else{
				//ServoMove(SERVO_0,CLOSED_SERVO_ANGLE);
			}
		}
		vTaskDelay(READING_DISTANCE_DELAY);
	}
}

// /** @fn weight_task
//  * @brief Tarea que se encarga de medir el peso y llamar a las funciones del buzzer, los leds y la desinfeccion
//  * @param pvParameter Puntero utilizado por el sistema operativo para administrar las tareas
//  */
// void weight_task(void *pvParameter)
// {
// 	bool trash_full=false;
// 	bool blinking=false;
// 	uint8_t counter=0;
// 	while(true){	//Se crea un bucle que siempre corre cuando el sistema operativo lo asigna
// 		if(HX711_read()>=MAX_WEIGHT){
// 			// Sonar buzzer
// 			red_light();
// 			trash_full=true;	// Para que no se esté desinfectando continuamente si no se llenó previamente
// 			blinking=false;		// Para que no esté titilando 
// 			counter=0;
// 		}
// 		else{
// 			if(trash_full){		// Si previamente fue llenado se procede a desinfectar
// 				if(counter<(DELAY_TO_DESINFECT/READING_WEIGHT_DELAY)){
// 					counter++;
// 					if(!blinking){
// 						GPIOOn(GPIO_11);	// Encender led verde para luego titilar
// 						GPIOOff(GPIO_12);	// Encender led rojo para luego titilar
// 						blinking=true;		// Para que a partir de ahora titile
// 					}
// 					else{
// 						blink_leds();		// Titilar los leds
// 					}
// 				}
// 				else{					
// 					disinfect();	// Desinfectar
// 					green_light();	// Enciende el led verde y apaga el rojo
// 					trash_full=false;	// Falso para que no entre a hacer la desinfeccion mientras se mantenga sin llenar
// 					blinking=false;		// Falso para que ya no titile
// 				}
// 			}
// 		}
// 		vTaskDelay(READING_WEIGHT_DELAY);
// 	}
// }

/** @fn initialize_all
 * @brief Inicializa todos los perifericos utilizados
 */
void initialize_all()
{
	HcSr04Init(GPIO_3,GPIO_2);				// Inicializa el sensor ultrasonido
	//GPIOInit(El gpio de la desinfeccion)
	//GPIOInit(El gpio que enciende led verde)
	//GPIOInit(El gpio que enciende el led rojo)
	//ServoInit(SERVO_0,GPIO_8);				// Inicializa el servo *Chequear cual gpio hay que usar
	//ServoMove(SERVO_0,CLOSED_SERVO_ANGLE);	// Inicia el servo cerrado antes de empezar las tareas
	//HX711_Init(SCALE_GAIN,GPIO_14,GPIO_15); 	// Inicializa la balanza *Chequear cual gpio hay que usar
	//HX711_tare(10);							// Tarar la balanza (no se si es necesario hacer esto)
}

/*==================[external functions definition]==========================*/
void app_main(void){
	initialize_all();
	xTaskCreate(&distance_task,"Distance",512,NULL,5,NULL);	// Crea la tarea de leer distancia y la empieza a correr
	//xTaskCreate(&weight_task,"Weight",512,NULL,5,NULL); 	// Crea la tarea de medir el peso y la empieza a correr
}
/*==================[end of file]============================================*/