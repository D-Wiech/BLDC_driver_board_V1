/*
 * STM32_Cordic.c
 *
 *  Created on: Aug 27, 2023
 *      Author: Daniel
 */

#include "main.h"
#include "STM32_Cordic.h"



float fast_Sinus(float Eingabe)// Sinusfunktion | Eingabe in Grad | -1 < Ausgabe < 1
{
	Eingabe = Eingabe / 180.f;//Umrechnung von 360Grad auf 1

	//Beachtung der Periodizität
	while(Eingabe > 1.)
	{
		Eingabe  = Eingabe - 2.f;
	}
	while(Eingabe < -1.)
	{
		Eingabe = Eingabe + 2.f;
	}

	//Umwandelung des Datenformats (Darstellung mit einer Festkommazahl)
	int32_t Cordic_Eingabe = (int32_t)((Eingabe * 2147483648.f));

	// Low Level code (Initalisierung der Cordic-Einheit)
	LL_CORDIC_Config(CORDIC, LL_CORDIC_FUNCTION_SINE,   /* cosine function */
	                           LL_CORDIC_PRECISION_6CYCLES, /* max precision for q1.31 cosine */
	                           LL_CORDIC_SCALE_0,           /* no scale */
	                           LL_CORDIC_NBWRITE_1,         /* One input data: angle. Second input data (modulus) is 1 after cordic reset */
	                           LL_CORDIC_NBREAD_1,          /* Two output data: cosine, then sine */
	                           LL_CORDIC_INSIZE_32BITS,     /* q1.31 format for input data */
	                           LL_CORDIC_OUTSIZE_32BITS);   /* q1.31 format for output data */

	 //Festkommazahl der Cordic-Einheit übergeben (Startet die Berechnung)
	  LL_CORDIC_WriteData(CORDIC, Cordic_Eingabe);

	  //Auslesen des Sinus Ergebnisses
	  int32_t Cordic_Ergebnis = (int32_t)LL_CORDIC_ReadData(CORDIC);

	  //Umrechnen von dem Festkommaergebnis zur Gleitkommazahl
	  float Ergebnis = (float)(Cordic_Ergebnis) * 0.4656612873077392578125E-9f;

	  return Ergebnis;
}

float fast_Cosinus(float Eingabe)// Cosinusfunktion | Eingabe in Grad | -1 < Ausgabe < 1
{
	//Umrechnung von 360Grad auf 1
	Eingabe = Eingabe / 180.f;

	//Beachtung der Periodizität
	while(Eingabe > 1.)
	{
		Eingabe  = Eingabe - 2.f;
	}
	while(Eingabe < -1.)
	{
		Eingabe = Eingabe + 2.f;
	}

	//Umwandelung des Datenformats (Darstellung mit einer Festkommazahl)
	int32_t Cordic_Eingabe = (int32_t)((Eingabe * 2147483648.f));

	// Low Level code (Initalisierung der Cordic-Einheit)
	LL_CORDIC_Config(CORDIC, LL_CORDIC_FUNCTION_COSINE,   /* cosine function */
	                           LL_CORDIC_PRECISION_6CYCLES, /* max precision for q1.31 cosine */
	                           LL_CORDIC_SCALE_0,           /* no scale */
	                           LL_CORDIC_NBWRITE_1,         /* One input data: angle. Second input data (modulus) is 1 after cordic reset */
	                           LL_CORDIC_NBREAD_1,          /* Two output data: cosine, then sine */
	                           LL_CORDIC_INSIZE_32BITS,     /* q1.31 format for input data */
	                           LL_CORDIC_OUTSIZE_32BITS);   /* q1.31 format for output data */

	 //Festkommazahl der Cordic-Einheit übergeben (Startet die Berechnung)
	  LL_CORDIC_WriteData(CORDIC, Cordic_Eingabe);

	  //Auslesen des Cosinus Ergebnisses (Erst nach erfolgreicher Berechnung wird das Lesen ausgeführt)
	  int32_t cosOutput = (int32_t)LL_CORDIC_ReadData(CORDIC);

	  //Umrechnen von dem Festkommaergebnis zur Gleitkommazahl
	  float Ergebnis = (float)(cosOutput) * 0.4656612873077392578125E-9f;

	  return Ergebnis;
}

float fast_atan2(float y, float x)//Atan2 funktion / X:Realteil, Y:Imaginärteil; Ausgabe: Winkel(grad) bezügleich der positiven Realachse
{
	//Beachtung des Eingabeberechs (Eingabebereich ist -1 bis 1)
	while(y > 1.f || y < -1.f)
	{
		y = y * 0.01f; // durch hundert teilen
		x = x * 0.01f;
	}
	while(x > 1.f || x < -1.f)
	{
		x = x * 0.01f;
		y = y * 0.01f;
	}

	//Umwandelung des Datenformats (Darstellung mit einer Festkommazahl)
	int32_t Cordic_Eingabe1_Y = (int32_t)((y * 2147483648.f));
	int32_t Cordic_Eingabe2_X = (int32_t)((x * 2147483648.f));

	// Low Level code (Initalisierung der Cordic-Einheit)
	LL_CORDIC_Config(CORDIC, LL_CORDIC_FUNCTION_PHASE,   /* atan2 function */
	                           LL_CORDIC_PRECISION_6CYCLES, /* max precision for q1.31 cosine */
	                           LL_CORDIC_SCALE_0,           /* no scale */
							   LL_CORDIC_NBWRITE_2,         /* Two input data: angle. */
	                           LL_CORDIC_NBREAD_1,          /* Two output data: cosine, then sine */
	                           LL_CORDIC_INSIZE_32BITS,     /* q1.31 format for input data */
	                           LL_CORDIC_OUTSIZE_32BITS);   /* q1.31 format for output data */

	 //Festkommazahl der Cordic-Einheit übergeben (Startet die Berechnung)
	  LL_CORDIC_WriteData(CORDIC, Cordic_Eingabe2_X);
	  LL_CORDIC_WriteData(CORDIC, Cordic_Eingabe1_Y);

	  //Auslesen des Atan2 Ergebnisses (Erst nach erfolgreicher Berechnung wird das Lesen ausgeführt)
	  int32_t Cordic_Winkel = (int32_t)LL_CORDIC_ReadData(CORDIC);

	  //Umrechnen von dem Festkommaergebnis zur Gleitkommazahl
	  float Ergebnis = (float)(Cordic_Winkel) * 0.4656612873077392578125E-9f;

	  return Ergebnis * 180;
}

float fast_sqrt(float x)//Wurzel funktion / Eingabe: X; Ausgabe: Wurzel von X
{
	//Signifikant langsamer als sqrtf aus math.h
	int n = 0;
	if(x < 0.027f)
	{
		return NAN;
	}
	//Beachtung des Eingabeberechs (Eingabebereich ist -1 bis 1)
	while(x > 0.703f)
	{
		x = x * 0.25f; // Mal 4^(-n)
		n++;
	}

	//Umwandelung des Datenformats (Darstellung mit einer Festkommazahl)
	int32_t Cordic_Eingabe1 = (int32_t)((x * 2147483648.f));

	// Low Level code (Initalisierung der Cordic-Einheit)
	LL_CORDIC_Config(CORDIC, LL_CORDIC_FUNCTION_SQUAREROOT,   /* atan2 function */
							   LL_CORDIC_PRECISION_6CYCLES , /* max precision for q1.31 cosine */
	                           LL_CORDIC_SCALE_0,           /* scale of 0 */
							   LL_CORDIC_NBWRITE_1,         /* Two input data: angle. */
	                           LL_CORDIC_NBREAD_1,          /* Two output data: cosine, then sine */
	                           LL_CORDIC_INSIZE_32BITS,     /* q1.31 format for input data */
	                           LL_CORDIC_OUTSIZE_32BITS);   /* q1.31 format for output data */

	 //Festkommazahl der Cordic-Einheit übergeben (Startet die Berechnung)
	  LL_CORDIC_WriteData(CORDIC, Cordic_Eingabe1);

	  //Auslesen des Wurzel Ergebnisses (Erst nach erfolgreicher Berechnung wird das Lesen ausgeführt)
	  int32_t Cordic_Wurzel = (int32_t)LL_CORDIC_ReadData(CORDIC);

	  //Umrechnen von dem Festkommaergebnis zur Gleitkommazahl
	  float Ergebnis = (float)(Cordic_Wurzel) * 0.4656612873077392578125E-9f;

	  //Beachtung des Eingabeberechs (Eingabebereich ist -1 bis 1)
	  if(n % 2 == 1)
	  {
		  n = n / 2;
		  while(n > 0)
		  {
			  Ergebnis = Ergebnis * 4.f; // Mal 4^(n)
			  n--;
		  }
		  Ergebnis = Ergebnis * 2.f; // Mal 4^(1/2)
	  }
	  else
	  {
		  n =  n / 2;
		  while(n > 0)
		  {
			  Ergebnis = Ergebnis * 4.f; // Mal 4^(n)
			  n--;
		  }
	  }

	  return Ergebnis;
}

void fast_Sinus_Cosinus(float Eingabe, float* Sinus_out, float* Cosinus_out)
{
	//Nimmt einen Winkel in Grad ein und gibt die Cosinus sowie Sinus werte wieder zurück
	Eingabe = Eingabe / 180.f;//Umrechnung von 360Grad auf 1

	//Beachtung der Periodizität
	while(Eingabe > 1.)
	{
		Eingabe  = Eingabe - 2.f;
	}
	while(Eingabe < -1.)
	{
		Eingabe = Eingabe + 2.f;
	}

	//Umwandelung des Datenformats (Darstellung mit einer Festkommazahl)
	int32_t Cordic_Eingabe = (int32_t)((Eingabe * 2147483648.f));

	// Low Level code (Initalisierung der Cordic-Einheit)
	LL_CORDIC_Config(CORDIC, LL_CORDIC_FUNCTION_COSINE,   /* cosine function */
	                           LL_CORDIC_PRECISION_6CYCLES, /* max precision for q1.31 cosine */
	                           LL_CORDIC_SCALE_0,           /* no scale */
	                           LL_CORDIC_NBWRITE_1,         /* One input data: angle. Second input data (modulus) is 1 after cordic reset */
	                           LL_CORDIC_NBREAD_2,          /* Two output data: cosine, then sine */
	                           LL_CORDIC_INSIZE_32BITS,     /* q1.31 format for input data */
	                           LL_CORDIC_OUTSIZE_32BITS);   /* q1.31 format for output data */

	 //Festkommazahl der Cordic-Einheit übergeben (Startet die Berechnung)
	  LL_CORDIC_WriteData(CORDIC, Cordic_Eingabe);

	  //Auslesen des Ergebnisses
	  int32_t COSINE_Ergebnis = (int32_t)LL_CORDIC_ReadData(CORDIC);
	  int32_t SINE_Ergebnis = (int32_t)LL_CORDIC_ReadData(CORDIC);

	  //Umrechnen von dem Festkommaergebnis zur Gleitkommazahl
	  *Cosinus_out = (float)(COSINE_Ergebnis) * 0.4656612873077392578125E-9f;
	  *Sinus_out = (float)(SINE_Ergebnis) * 0.4656612873077392578125E-9f;
}
