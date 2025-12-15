/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.h
  * @brief   This file contains all the function prototypes for
  *          the fdcan.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FDCAN_H__
#define __FDCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"

/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

extern FDCAN_HandleTypeDef hfdcan1;

/* USER CODE BEGIN Private defines */
#define CAN_TX_TIMEOUT		10
#define CAN_BUF_SIZE		125		/* Battery side buffer [num messages] [RAM] */
#define CAN_MES_SIZE		8		/* 8 bytes CAN message payload */

#define TEST_MSG_ID 		0x183

typedef struct
{
	FDCAN_RxHeaderTypeDef   RxHeader;
	uint8_t					rx_data[CAN_MES_SIZE];
} can_rx_struct;

typedef struct
{
	int32_t 	mot_rpm;
//	int16_t 	mot_temp;
//	uint32_t 	batt_volt;
//	int16_t 	batt_curr;
//	int8_t 		min_cell_temp;
//	int8_t 		max_cell_temp;
//	uint32_t 	batt_err_code;
//	uint16_t 	sevcon_err_code;
//	int16_t 	sevcon_temp;
//	uint16_t 	min_cell_volt;
//	uint16_t 	max_cell_volt;
//	int16_t		mot_torque;
//	int16_t 	mot_power;
}disp_data_struct;

extern disp_data_struct disp_data;


/* USER CODE END Private defines */

void MX_FDCAN1_Init(void);

/* USER CODE BEGIN Prototypes */

/**
 * @brief Initialize CAN handling variables
 */
void can_handling_init(void);

/**
 * @brief Set CAN bus activity
 * @param val: True if a message was received on the CAN bus, otherwise false
 */
void can_set_activity_stat(bool val);

/**
 * @brief Get CAN bus activity status
 * @return uint32_t number of CAN messages
 */
uint32_t can_get_activity_stat(void);

/**
 * @brief Initialize CAN module 1, 2 and can handling
 */
void can_init(void);

/**
 * @brief Copy CAN bus pack messages from CAN buffer to local buffer
 * @param can_rx_struct
 * @return true if messages available, otherwise false
 */
bool can_receive_batt(can_rx_struct *p_rx_struct);

/**
 * @brief Process, sort CAN bus pack messages
 */
void can_receive_batt_proc(void* arg);


void can_msg_proc(can_rx_struct *can_rx_data);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __FDCAN_H__ */

