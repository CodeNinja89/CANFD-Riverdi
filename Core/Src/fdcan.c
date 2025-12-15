/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.c
  * @brief   This file provides code for the configuration
  *          of the FDCAN instances.
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
/* Includes ------------------------------------------------------------------*/
#include "fdcan.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
#include <string.h>
#include "cmsis_os2.h"

#define	CAN_MAX_ACK_ERROR		3	/* maximum number of no ack errors */
#define CAN_MAX_OTH_ERROR		10	/* maximum number of other CANbus errors */
#define CAN_MAX_STF_ERROR		12	/* maximum number of STF errors */
#define CAN_HANDLING_DEBUG 		0

FDCAN_TxHeaderTypeDef TxHeader;

__IO uint32_t Notification_flag = 0;
__IO uint32_t Transmission_flag = 0;


/* 		Global variables				*/
FDCAN_RxHeaderTypeDef 	RxHeader;
FDCAN_RxHeaderTypeDef   RxHeader_batt;
uint8_t RxData[8];
uint32_t tx_timer_start;
uint32_t tx_timer;
volatile uint32_t can_activity_stat;
uint8_t 	rx_data_batt[8];
uint8_t 	rx_data_app[8];
uint32_t 	can_rx_write_ptr_batt;
uint32_t 	can_rx_read_ptr_batt;
uint32_t 	can_rx_write_ptr_app;
uint32_t 	can_rx_read_ptr_app;
uint32_t    tx_mailbox;
uint32_t 	can_fifo0_ov;
uint32_t 	can_fifo1_ov;
uint32_t 	can_rx_error_overrun_batt;
uint32_t 	can_rx_error_overrun_app;
uint32_t 	can_ack_error_count;
uint32_t 	can_oth_error_count;
uint32_t 	can_stf_error_count;
uint32_t 	can_err;
uint32_t 	can_err_inst;
can_rx_struct can_rx_buffer_batt[CAN_BUF_SIZE];

disp_data_struct disp_data;


/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;

/* FDCAN1 init function */
void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */
	FDCAN_FilterTypeDef sFilterConfig; // the required filter ID can be found in the struct
  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = ENABLE;
  hfdcan1.Init.NominalPrescaler = 8;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 8;
  hfdcan1.Init.NominalTimeSeg2 = 1;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 1;
  hfdcan1.Init.DataTimeSeg2 = 1;
  hfdcan1.Init.MessageRAMOffset = 0;
  hfdcan1.Init.StdFiltersNbr = 1;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.RxFifo0ElmtsNbr = 1;
  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxFifo1ElmtsNbr = 1;
  hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxBuffersNbr = 10;
  hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.TxEventsNbr = 0;
  hfdcan1.Init.TxBuffersNbr = 0;
  hfdcan1.Init.TxFifoQueueElmtsNbr = 1;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */
  /* Configure Rx filter */
  // read messages from specified IDs
  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0; // IMP
  sFilterConfig.FilterType = FDCAN_FILTER_MASK; // IMP
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // IMP
  sFilterConfig.FilterID1 = 0x321; // the specified IDs
  sFilterConfig.FilterID2 = 0x7FF;
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }

  printf("CAN bus config filter succ!\r\n");
//  /* Configure global filter to reject all non-matching frames */
//  HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
//
//  printf("CAN bus global config filter succ!\r\n");

  /* Configure Rx FIFO 0 watermark to 2 */
  HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 2);

  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }

  printf("CAN bus started succ!\r\n");
    /* Activate Rx FIFO 0 watermark notification */
  HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

  /* Prepare Tx Header */
  TxHeader.Identifier = 0x111;
  TxHeader.IdType = FDCAN_STANDARD_ID;
  TxHeader.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader.DataLength = FDCAN_DLC_BYTES_8;
  TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader.BitRateSwitch = FDCAN_BRS_ON;
  TxHeader.FDFormat = FDCAN_FD_CAN;
  TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  TxHeader.MessageMarker = 0;
  /* USER CODE END FDCAN1_Init 2 */

}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspInit 0 */

  /* USER CODE END FDCAN1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* FDCAN1 clock enable */
    __HAL_RCC_FDCAN_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PB9     ------> FDCAN1_TX
    PA11     ------> FDCAN1_RX
    */
    GPIO_InitStruct.Pin = FDCAN1_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(FDCAN1_TX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = FDCAN1_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(FDCAN1_RX_GPIO_Port, &GPIO_InitStruct);

    /* FDCAN1 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
    HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
  /* USER CODE BEGIN FDCAN1_MspInit 1 */

  /* USER CODE END FDCAN1_MspInit 1 */
  }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

  /* USER CODE END FDCAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FDCAN_CLK_DISABLE();

    /**FDCAN1 GPIO Configuration
    PB9     ------> FDCAN1_TX
    PA11     ------> FDCAN1_RX
    */
    HAL_GPIO_DeInit(FDCAN1_TX_GPIO_Port, FDCAN1_TX_Pin);

    HAL_GPIO_DeInit(FDCAN1_RX_GPIO_Port, FDCAN1_RX_Pin);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
    HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
  /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

  /* USER CODE END FDCAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void can_init()
{
	can_handling_init();

	tx_timer_start 	= 0;
	tx_timer 		= 0;
}

bool can_receive_batt(can_rx_struct *p_rx_struct)
{
	if(can_rx_error_overrun_batt != 0) /* Check if there was a FIFO overrun */
	{
		printf("CAN OVERRUN PACK SIDE!\r\n");
		can_rx_error_overrun_batt = 0;
	}

	if(can_rx_read_ptr_batt != can_rx_write_ptr_batt) /* Check if there is new data*/
	{
		uint32_t can_rx_read_temp =  can_rx_read_ptr_batt;
		/* start of Critical section - disable interrupts */
		__disable_irq();
		can_rx_read_ptr_batt++;
		if (can_rx_read_ptr_batt >= CAN_BUF_SIZE)
		{
			can_rx_read_ptr_batt = 0;
		}
		(void) memcpy((void*)p_rx_struct, (const void*)(&can_rx_buffer_batt[can_rx_read_temp]), sizeof(can_rx_struct));
		/* end of Critical section - enable interrupts*/
		__enable_irq();
		return true;
	}
	else
	{
		return false;
	}
}

void can_receive_batt_proc(void* arg)
{
	can_init();

	while(1)
	{
		HAL_GPIO_TogglePin(USR_LED_GPIO_Port, USR_LED_Pin);
		can_rx_struct can_rx_frame;
		bool ret_val;
		ret_val = can_receive_batt(&can_rx_frame);

//		printf("ret_val = %d\r\n", ret_val);
//		printf("%d %d %d %d %d %d 0x%-8x \r\n", disp_data.batt_volt, disp_data.min_cell_volt, disp_data.max_cell_volt, (int)disp_data.batt_curr,  disp_data.min_cell_temp, disp_data.max_cell_temp, disp_data.batt_err_code);

		while(ret_val == true)
		{
			can_msg_proc(&can_rx_frame);
			ret_val = can_receive_batt(&can_rx_frame);
		}
		osDelay(50);
	}
}

void can_handling_init()
{
	can_rx_write_ptr_batt	= 0;
	can_rx_read_ptr_batt	= 0;

	can_rx_write_ptr_app 	= 0;
	can_rx_read_ptr_app 	= 0;

	can_fifo0_ov 			= 0;
	can_fifo1_ov 			= 0;

	can_rx_error_overrun_batt	= 0;
	can_rx_error_overrun_app	= 0;

	can_ack_error_count 	= 0;
	can_oth_error_count 	= 0;
	can_stf_error_count 	= 0;
	can_err 				= 0;
	can_err_inst 			= 0;

	can_activity_stat 		= 0;
}

void can_set_activity_stat(bool val)
{
	if(val == false)
	{
		can_activity_stat = 0;
	}
	else
	{
		can_activity_stat++;
	}
}

uint32_t can_get_activity_stat(void)
{
	return can_activity_stat;
}

void can_msg_proc(can_rx_struct *can_rx_data)
{
	uint32_t can_id = 0;
	int32_t curr = 0;
	can_id = can_rx_data->RxHeader.Identifier;

	if(can_id == TEST_MSG_ID)
	{

		disp_data.mot_rpm =	(uint32_t)(can_rx_data->rx_data[1] << 8) 	+
								(uint32_t)(can_rx_data->rx_data[0]);
	}
//	else if(can_id == BCB_MSG_ID_1)
//	{
//		disp_data.min_cell_volt = 	(uint32_t)(can_rx_data->rx_data[1] << 8) 	+
//									(uint32_t)(can_rx_data->rx_data[0]);
//
//		disp_data.max_cell_volt = 	(uint32_t)(can_rx_data->rx_data[3] << 8) 	+
//									(uint32_t)(can_rx_data->rx_data[2]);
//
//		disp_data.min_cell_temp =   (int8_t)(can_rx_data->rx_data[4]);
//		disp_data.max_cell_temp =   (int8_t)(can_rx_data->rx_data[5]);
//
//	}
//	else if(can_id == BCB_MSG_ID_2)
//	{
//		disp_data.batt_err_code = 	(uint32_t)(can_rx_data->rx_data[3] << 24) 	+
//									(uint32_t)(can_rx_data->rx_data[2] << 16) 	+
//									(uint32_t)(can_rx_data->rx_data[1] << 8) 	+
//									(uint32_t)can_rx_data->rx_data[0];
//	}
//	else if(can_id == SEV_MSD_ID_0)
//	{
//
//	}
//	else if(can_id == SEV_MSD_ID_1)
//	{
//		disp_data.sevcon_err_code = 	(uint32_t)(can_rx_data->rx_data[7] << 8) 	+
//										(uint32_t)(can_rx_data->rx_data[6]);
//	}
//	else if(can_id == SEV_MSD_ID_2)
//	{
//
//	}
//	else if(can_id == SEV_MSD_ID_3)
//	{
//		disp_data.mot_temp = 	(uint32_t)(can_rx_data->rx_data[1] << 8) 	+
//								(uint32_t)(can_rx_data->rx_data[0]);
//
//		disp_data.sevcon_temp	= (int32_t)can_rx_data->rx_data[2];
//
//		disp_data.mot_torque = (int32_t)(can_rx_data->rx_data[6] << 8) 	+
//								(int32_t)(can_rx_data->rx_data[5]);
//	}
//	else if(can_id == SEV_MSD_ID_4)
//	{
//		disp_data.mot_rpm = (int32_t)(can_rx_data->rx_data[7] << 24) 	+
//							(int32_t)(can_rx_data->rx_data[6] << 16) 	+
//							(int32_t)(can_rx_data->rx_data[5] << 8) 	+
//							(int32_t)can_rx_data->rx_data[4];
//	}
}


void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	can_set_activity_stat(true);

	if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
	{
		/* Retreive Rx messages from RX FIFO0 */
		uint8_t can_err = HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader_batt, rx_data_batt);
		if (can_err != HAL_OK)
		{
		/* Reception Error */
//		Error_Handler();
		}

		if(RxHeader_batt.IdType == FDCAN_STANDARD_ID)
		{
			uint32_t i = can_rx_write_ptr_batt + 1;

			if(i >= CAN_BUF_SIZE)
			{
				i = 0;
			}

			if(i != can_rx_read_ptr_batt)
			{
				/* Copy header */
				can_rx_buffer_batt[can_rx_write_ptr_batt].RxHeader = RxHeader_batt;
				/* Copy data */
				for(uint8_t counter = 0; counter < CAN_MES_SIZE; counter++)
				{
					can_rx_buffer_batt[can_rx_write_ptr_batt].rx_data[counter] = rx_data_batt[counter];
				}
	//			printf("CAN bus PACK - new message received from 0x%x with length %ld bytes!"
	//					"\r\n",
	//					can_rx_buffer_batt[can_rx_write_ptr_batt].RxHeader.StdId,
	//					can_rx_buffer_batt[can_rx_write_ptr_batt].RxHeader.DLC);
	//			for (uint8_t i = 0; i < 8; i++)
	//			{
	//				printf("%x\t", can_rx_buffer_batt[can_rx_write_ptr_batt].rx_data[i]);
	//			}
	//			printf("\r\n");

				/* Increment write_ptr to next location */
				can_rx_write_ptr_batt++;
				/* If last location in the buffer, then go back to 0 */
				if(can_rx_write_ptr_batt >= CAN_BUF_SIZE)
				{
					can_rx_write_ptr_batt = 0;
				}
			}
			else
			{
				can_rx_error_overrun_batt++;
			}
		}


		if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
		{
		  /* Notification Error */
		  Error_Handler();
		}
	}
}

void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan)
{
	can_err = hfdcan->ErrorCode;
	printf("CAN bus error Call back!\r\n");
	hfdcan->ErrorCode = HAL_FDCAN_ERROR_NONE;
}

/* USER CODE END 1 */
