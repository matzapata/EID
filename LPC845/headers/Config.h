#ifndef CONFIG_H_
#define CONFIG_H_

/********************************************************************************************
 * INCLUDES
 ********************************************************************************************/
#include "Timer.h"
#include "SerialPort.h"


/********************************************************************************************
 * DEFINES
 ********************************************************************************************/

// Serial Port
#define COM_PORT SERIAL_PORT_0
#define BAUD_RATE 115200

// Timer
#define TIMER_MDEMAIN TIMER_1
#define TIMER_TRAMA TIMER_2
#define KEYBOARD_TIMER TIMER_3
#define TIMER_ALERT TIMER_4

// Teclado
#define CONFIRMATION_KEY 1

// Constantes uso
#define CONFIRMATION_TIMEOUT 5000
#define SESSION_TIMEOUT 3600*1000*15 //15min
#define ALERT_TIMEOUT 3000

// Definiciones exclusivas al usuario.
#define PUBLIC_KEY_MODULUS 1888261861
#define PUBLIC_KEY_EXPONENT 131073
#define PRIVATE_KEY_MODULUS 1888261861
#define PRIVATE_KEY_EXPONENT 1444841537
#define USERNAME "Matias Zapata" // Maximo tamaño 16 bytes
#define DEFAULT_HASH "f6fdffe48c908deb0f4c3bd36c032e72"

#endif /* CONFIG_H_ */
