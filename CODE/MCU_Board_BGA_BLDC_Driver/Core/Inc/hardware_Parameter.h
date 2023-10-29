/*
 * hardware_Parameter.h
 *
 *  Created on: Sep 8, 2023
 *      Author: Daniel
 */

#ifndef INC_HARDWARE_PARAMETER_H_
#define INC_HARDWARE_PARAMETER_H_

//Contains the Hardware parameter and also the tuning values

	//I_D Parameters
	const float PID_KP_I_D = 1;
	const float PID_KI_I_D = 0;
	const float PID_KD_I_D = 0;

	const float PID_TAU_I_D = 0.00008f;//12.5kHz Update rate

	const float PID_LIM_MIN_INT_I_D = 5;
	const float PID_LIM_MAX_INT_I_D = 5;

	//I_Q Parameters
	const float PID_KP_I_Q = 1;
	const float PID_KI_I_Q = 0;
	const float PID_KD_I_Q = 0;

	const float PID_TAU_I_Q = 0.00008f;//12.5kHz Update rate

	const float PID_LIM_MIN_INT_I_Q = 5;
	const float PID_LIM_MAX_INT_I_Q = 5;

	const float SAMPLE_TIME_S = 0;

	//Motor Parameters
	typedef struct {

		/* Controller gains */
		float Pole_pair_count;

	} Motor_Parameter;

	//Controller Hardware Parameters
	typedef struct {

		float I_U_offset;
		float I_V_offset;
		float I_W_offset;

	} Controller_Hardware_Parameter;

	const float Pole_pair_count = 2.f;

	const float angle_sens_offset = 72.5f;

#endif /* INC_HARDWARE_PARAMETER_H_ */
