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

static volatile uint32_t g_value = 0;
static volatile uint8_t g_hasNew = 0;

void CANValue_SetFromISR(uint32_t v) {
	g_value = v;
	g_hasNew = 1;
}

uint8_t CANValue_Get(uint32_t *out) {
	if(!g_hasNew) {
		return 0;
	} else {
		*out = g_value;
		g_hasNew = 0;
		return 1;
	}
}

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
	  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL; // FDCAN_MODE_INTERNAL_LOOPBACK; // FDCAN_MODE_NORMAL;
	  hfdcan1.Init.AutoRetransmission = DISABLE;
	  hfdcan1.Init.TransmitPause = DISABLE;
	  hfdcan1.Init.ProtocolException = DISABLE;
	  hfdcan1.Init.NominalPrescaler = 10;
	  hfdcan1.Init.NominalSyncJumpWidth = 1;
	  hfdcan1.Init.NominalTimeSeg1 = 13;
	  hfdcan1.Init.NominalTimeSeg2 = 2;
	  hfdcan1.Init.DataPrescaler = 1;
	  hfdcan1.Init.DataSyncJumpWidth = 1;
	  hfdcan1.Init.DataTimeSeg1 = 1;
	  hfdcan1.Init.DataTimeSeg2 = 1;
	  hfdcan1.Init.MessageRAMOffset = 0;
	  hfdcan1.Init.StdFiltersNbr = 1;
	  hfdcan1.Init.ExtFiltersNbr = 0;
	  hfdcan1.Init.RxFifo0ElmtsNbr = 8;
	  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
	  hfdcan1.Init.RxFifo1ElmtsNbr = 0;
	  hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
	  hfdcan1.Init.RxBuffersNbr = 1;
	  hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
	  hfdcan1.Init.TxEventsNbr = 0;
	  hfdcan1.Init.TxBuffersNbr = 1;
	  hfdcan1.Init.TxFifoQueueElmtsNbr = 1;
	  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
	  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
	  {
		Error_Handler();
	  }
	  HAL_GPIO_TogglePin(USR_LED_1_GPIO_Port, USR_LED_1_Pin);
	  /* USER CODE BEGIN FDCAN1_Init 2 */
	  FDCAN_FilterTypeDef sFilter = {0};

	  // Route standard ID 0x123 to FIFO0
	  sFilter.IdType = FDCAN_STANDARD_ID;
	  sFilter.FilterIndex = 0;
	  sFilter.FilterType = FDCAN_FILTER_MASK;
	  sFilter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	  sFilter.FilterID1 = 0x111;   // ID
	  sFilter.FilterID2 = 0x000;   // mask: all bits must match

	  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilter) != HAL_OK)
	  {
		Error_Handler();
	  }
	  HAL_FDCAN_ConfigGlobalFilter(
	      &hfdcan1,
	      FDCAN_ACCEPT_IN_RX_FIFO0,  // Non-matching standard IDs
	      FDCAN_ACCEPT_IN_RX_FIFO0,  // Non-matching extended IDs
	      FDCAN_REJECT_REMOTE,       // Standard remote
	      FDCAN_REJECT_REMOTE        // Extended remote
	  );
	  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
	  {
		Error_Handler();
	  }
	  HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	  FDCAN_TxHeaderTypeDef txh = {0};
	  uint8_t txd[8] = {0};

	  txh.Identifier = 0x123;
	  txh.IdType = FDCAN_STANDARD_ID;
	  txh.TxFrameType = FDCAN_DATA_FRAME;
	  txh.DataLength = FDCAN_DLC_BYTES_8;
	  txh.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	  txh.BitRateSwitch = FDCAN_BRS_OFF;
	  txh.FDFormat = FDCAN_CLASSIC_CAN;
	  txh.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	  txh.MessageMarker = 0;

	  uint32_t value = 424242; // whatever you want to see
	  txd[0] = (uint8_t)(value);
	  txd[1] = (uint8_t)(value >> 8);
	  txd[2] = (uint8_t)(value >> 16);
	  txd[3] = (uint8_t)(value >> 24);
	   //rest bytes can be 0

	  HAL_FDCAN_ActivateNotification(&hfdcan1,
	      FDCAN_IT_TX_COMPLETE | FDCAN_IT_BUS_OFF | FDCAN_IT_ERROR_PASSIVE | FDCAN_IT_ERROR_WARNING,
	      0);

	  if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txh, txd) != HAL_OK)
	  {
		Error_Handler();
	  }
  /* USER CODE END FDCAN1_Init 2 */

}

static uint32_t HAL_RCC_FDCAN_CLK_ENABLED=0;

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspInit 0 */

  /* USER CODE END FDCAN1_MspInit 0 */
    /* FDCAN1 clock enable */
    HAL_RCC_FDCAN_CLK_ENABLED++;
    if(HAL_RCC_FDCAN_CLK_ENABLED==1){
      __HAL_RCC_FDCAN_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PB9     ------> FDCAN1_TX
    PA11     ------> FDCAN1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = FDCAN1_STB_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(FDCAN1_STB_GPIO_Port, FDCAN1_STB_Pin, GPIO_PIN_RESET);

    /* FDCAN1 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
    HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
  /* USER CODE BEGIN FDCAN1_MspInit 1 */

  /* USER CODE END FDCAN1_MspInit 1 */
  }
  else if(fdcanHandle->Instance==FDCAN2)
  {
  /* USER CODE BEGIN FDCAN2_MspInit 0 */

  /* USER CODE END FDCAN2_MspInit 0 */
    /* FDCAN2 clock enable */
    HAL_RCC_FDCAN_CLK_ENABLED++;
    if(HAL_RCC_FDCAN_CLK_ENABLED==1){
      __HAL_RCC_FDCAN_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**FDCAN2 GPIO Configuration
    PB5     ------> FDCAN2_RX
    PB13     ------> FDCAN2_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* FDCAN2 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
    HAL_NVIC_SetPriority(FDCAN2_IT1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(FDCAN2_IT1_IRQn);
  /* USER CODE BEGIN FDCAN2_MspInit 1 */

  /* USER CODE END FDCAN2_MspInit 1 */
  }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

  /* USER CODE END FDCAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_FDCAN_CLK_ENABLED--;
    if(HAL_RCC_FDCAN_CLK_ENABLED==0){
      __HAL_RCC_FDCAN_CLK_DISABLE();
    }

    /**FDCAN1 GPIO Configuration
    PB9     ------> FDCAN1_TX
    PA11     ------> FDCAN1_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
    HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
  /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

  /* USER CODE END FDCAN1_MspDeInit 1 */
  }
  else if(fdcanHandle->Instance==FDCAN2)
  {
  /* USER CODE BEGIN FDCAN2_MspDeInit 0 */

  /* USER CODE END FDCAN2_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_FDCAN_CLK_ENABLED--;
    if(HAL_RCC_FDCAN_CLK_ENABLED==0){
      __HAL_RCC_FDCAN_CLK_DISABLE();
    }

    /**FDCAN2 GPIO Configuration
    PB5     ------> FDCAN2_RX
    PB13     ------> FDCAN2_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5|GPIO_PIN_13);

    /* FDCAN2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);
    HAL_NVIC_DisableIRQ(FDCAN2_IT1_IRQn);
  /* USER CODE BEGIN FDCAN2_MspDeInit 1 */

  /* USER CODE END FDCAN2_MspDeInit 1 */
  }
}


/* USER CODE BEGIN 1 */

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
	if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) == 0) {
		return;
	}

	FDCAN_RxHeaderTypeDef header;
	uint8_t data[64];

	if(HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &header, data) != HAL_OK) {
		return;
	}

	//if(header.IdType == FDCAN_STANDARD_ID && header.Identifier == 0x111) {
		uint32_t v = (uint32_t) data[0] |
				((uint32_t) data[1] << 8) |
				((uint32_t) data[2] << 16) |
				((uint32_t) data[3] << 24);
		CANValue_SetFromISR(v);
		HAL_GPIO_TogglePin(USR_LED_1_GPIO_Port, USR_LED_1_Pin);
	//}
}

void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
    HAL_GPIO_TogglePin(USR_LED_1_GPIO_Port, USR_LED_1_Pin); // TX complete event
}


void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan)
{
//	can_err = hfdcan->ErrorCode;
//	// printf("CAN bus error Call back!\r\n");
//	hfdcan->ErrorCode = HAL_FDCAN_ERROR_NONE;
}

/* USER CODE END 1 */
