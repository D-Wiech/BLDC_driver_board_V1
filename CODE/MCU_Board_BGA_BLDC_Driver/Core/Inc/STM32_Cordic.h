/*
 * STM32_Cordic.h
 *
 *  Created on: Aug 27, 2023
 *      Author: Daniel
 */

#ifndef INC_STM32_CORDIC_H_
#define INC_STM32_CORDIC_H_

float fast_Sinus(float Eingabe);
float fast_Cosinus(float Eingabe);
float fast_atan2(float y, float x);
float fast_sqrt(float x);

void fast_Sinus_Cosinus(float Eingabe, float* Sinus_out, float* Cosinus_out);

#endif /* INC_STM32_CORDIC_H_ */
