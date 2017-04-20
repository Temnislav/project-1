/*****************************************************************************/
/**
 * \file terminal.c
 *
 * \author ������� ������ <sokoloff@gmail.com>
 * \author ������� ������ <temnislav@gmail.com>
 *
 * \date 05.01.2017
 * 
 * \brief ��������� UART � �������� ������ ����� ����.
 *
 * \details ������������ UART ��� ��� ����������� �������������, ��������
 *          ���������� ������� ��� ��������� ������ �� �������� ���������
 *          ������ ������� � �������� ���� �� ������ � DMA, ��� ���������-
 *          �� � ������� ���� ��������.
 * - terminal_uart_init - ������������ UART.
 * - terminal_uart_handler - ����������� ������ � �������������� ������
 * array_data_dma  ������ ��� ����������� � UART � � DMA.
 * - ���������� ������� mass_transmit_fifo ������� ������������ ���������
 * ���������� ������ �������.
 *****************************************************************************/

/*****************************************************************************
 *                            ������������ �����.
 *****************************************************************************/

#include "terminal.h"

/*****************************************************************************
 *                         ��������� ����������������.
 *****************************************************************************/

/**----------------------------------------------------------------------------
 *
 * \brief       FIFO_ELEMENTS - ���-�� ��������� ������� ��� ���������� ������  
 *              � ���������
 *              ERROR_MESSAGE_LENGTH - ������ ������� ��� �������� ���������
 *              �� ������
 *-----------------------------------------------------------------------------
 */

#define FIFO_ELEMENTS 100       
#define ERROR_MESSAGE_LENGTH 19   


/*****************************************************************************
 *                           ��������� ���� ������.
 *****************************************************************************/

/*****************************************************************************
 *                             ��������� ������.
 *****************************************************************************/

/**----------------------------------------------------------------------------
 *
 * \brief       UART_HandleTypeDef terminal_uart; � �������� ��� ��� UART
 *              ������� ����� ���������� ������ �� ���������.
 *              array_data_dma � ������ ��� ����������� � ���� ������ ������ 
 *              ������������ �� ����� �������� DMA � UART
 *              mass_transmit_fifo � ������ ������ ��� ��������� ������ 
 *              �� ��������� � ����������� ������������ � array_data_dma.
 *              mass_transmit_fifo_head � ���������� ��������� ��� �����������
 *              �������� ������� � ������� ������ ������.
 *-----------------------------------------------------------------------------
 */

UART_HandleTypeDef terminal_uart;

uint8_t array_data_dma[FIFO_ELEMENTS];
uint8_t mass_transmit_fifo[FIFO_ELEMENTS];
uint8_t mass_transmit_fifo_head = 0;

/*****************************************************************************
 *                         ��������� ��������� �������.
 *****************************************************************************/

/*****************************************************************************
 *                             ��������� �������.
 *****************************************************************************/

/*****************************************************************************
 *                        ���������� �������
 *****************************************************************************/

/*������������� UART � ������������ ��� �����������*/
void terminal_uart_init(void)
{
  terminal_uart.Instance = USART1;
  terminal_uart.Init.BaudRate = 9600;
  terminal_uart.Init.WordLength = UART_WORDLENGTH_8B;
  terminal_uart.Init.StopBits = UART_STOPBITS_1;
  terminal_uart.Init.Parity = UART_PARITY_NONE;
  terminal_uart.Init.Mode = UART_MODE_TX_RX;
  terminal_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  terminal_uart.Init.OverSampling = UART_OVERSAMPLING_16;
  terminal_uart.Init.OneBitSampling              = UART_ONE_BIT_SAMPLE_DISABLE;
  terminal_uart.AdvancedInit.AdvFeatureInit      = UART_ADVFEATURE_DMADISABLEONERROR_INIT;
  terminal_uart.AdvancedInit.DMADisableonRxError = UART_ADVFEATURE_DMA_DISABLEONRXERROR;
  HAL_UART_Init(&terminal_uart);
}

/*���������� �������� ������ ����� UART � ��������� �� "������" ������*/
void terminal_uart_handler(void)
{
  if(mass_transmit_fifo_head != 0)
  {
    memcpy(array_data_dma, mass_transmit_fifo, FIFO_ELEMENTS);
    HAL_UART_Transmit(&terminal_uart, array_data_dma, mass_transmit_fifo_head, 10);
    mass_transmit_fifo_head = 0;
  }
}

/*�������� ������� ��� ���������� ������� ��� ����������� �������� ������, 
�������� �� ������������ ������� � ��������� ��������� �� ������*/
void terminal_send_data(uint8_t *pData, uint16_t Size)
{
  uint8_t error = 0;
  for(uint8_t i = 0; i < (Size - 1); ++i)
  {
    mass_transmit_fifo[mass_transmit_fifo_head++] = *pData++;
    if(mass_transmit_fifo_head == FIFO_ELEMENTS)
    {
      mass_transmit_fifo_head = 0;
      error = 1;
      break;
    }
  }
  if(error == 1)
  {
    uint8_t error_message[ERROR_MESSAGE_LENGTH] = "terminal:data lost";
    for(uint8_t i = 0; i < (ERROR_MESSAGE_LENGTH - 1); ++i)
    {
          mass_transmit_fifo[mass_transmit_fifo_head++] = error_message[i];
    }
  }
  /*mass_transmit_fifo[mass_transmit_fifo_head++] = END_OF_DATA;*/ 
}

/*****************************************************************************
 *                            ����� �����
 *****************************************************************************/