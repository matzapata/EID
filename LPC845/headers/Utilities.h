#ifndef UTILITIES_H_
#define UTILITIES_H_

/***********************************************************************************************************************************
 *** CONSTANTES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PUBLICAS
 **********************************************************************************************************************************/

/* void AlertPendingConfirmation(void);
 * Enciende led azul para indicar que hay confirmacion de comunicacion
 * pendiente.
 */
void AlertPending(void);

void AlertOK(void);
void AlertFAIL(void);
void AlertIdleState(void);

#endif /* UTILITIES_H_ */
