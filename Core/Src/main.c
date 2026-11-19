/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CONTADOR_MIN        0U //Valor mínimo del display
#define CONTADOR_MAX        7U //valor máximo del display
#define DEBOUNCE_MS         200 //ms de antirrebote

//DISPLAY
#define BTN_SUBIR_PORT      GPIOB
#define BTN_SUBIR_PIN       GPIO_PIN_12
#define BTN_BAJAR_PORT      GPIOB
#define BTN_BAJAR_PIN       GPIO_PIN_13

/* Mapeo de segmentos DISPLAY:
 * PD0 -> Seg A (Pin 10)
 * PD1 -> Seg B (Pin 9)
 * PD2 -> Seg C (Pin 1)
 * PD3 -> Seg D (Pin 4)
 * PD4 -> Seg E (Pin 3)
 * PD5 -> Seg F (Pin 6)
 * PD6 -> Seg G (Pin 5)
 */
#define SEG_A_PIN           GPIO_PIN_0
#define SEG_B_PIN           GPIO_PIN_1
#define SEG_C_PIN           GPIO_PIN_2
#define SEG_D_PIN           GPIO_PIN_3
#define SEG_E_PIN           GPIO_PIN_4
#define SEG_F_PIN           GPIO_PIN_5
#define SEG_G_PIN           GPIO_PIN_6
#define SEG_ALL_MASK        (SEG_A_PIN | SEG_B_PIN | SEG_C_PIN | \
                             SEG_D_PIN | SEG_E_PIN | SEG_F_PIN | SEG_G_PIN)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* En anodo comun: 1 = segmento encendido, 0 = apagado.
 */
static const uint8_t tabla_digitos_7seg[10] = {
	0x3F, // 0: A B C D E F
    0x06, // 1:   B C
    0x5B, // 2: A B   D E   G
    0x4F, // 3: A B C D     G
    0x66, // 4:   B C     F G
    0x6D, // 5: A   C D   F G
    0x7D, // 6: A   C D E F G
    0x07, // 7: A B C
    0x7F, // 8: A B C D E F G
    0x6F  // 9: A B C D   F G
};
static uint8_t contador = 0;
static volatile bool flag_subir = false; //Flag Display
static volatile bool flag_bajar = false; //Flag Display
static uint32_t ultimo_tick_subir = 0U;
static uint32_t ultimo_tick_bajar = 0U;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void mostrar_digito_7seg(uint8_t digito); // Enciende los segmentos PD0..PD6
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void mostrar_digito_7seg(uint8_t digito)
{
    if (digito > 9U) //Protección ante fallos
    {
        return;
    }

    uint8_t mask = tabla_digitos_7seg[digito];

    /* Apagamos todos los segmentos primero (SET = nivel alto en anodo comun) */
    HAL_GPIO_WritePin(GPIOD, SEG_ALL_MASK, GPIO_PIN_SET);

    /* Encendemos solo los segmentos activos (RESET = nivel bajo en anodo comun) */
    if (mask & (1U << 0)) HAL_GPIO_WritePin(GPIOD, SEG_A_PIN, GPIO_PIN_RESET);
    if (mask & (1U << 1)) HAL_GPIO_WritePin(GPIOD, SEG_B_PIN, GPIO_PIN_RESET);
    if (mask & (1U << 2)) HAL_GPIO_WritePin(GPIOD, SEG_C_PIN, GPIO_PIN_RESET);
    if (mask & (1U << 3)) HAL_GPIO_WritePin(GPIOD, SEG_D_PIN, GPIO_PIN_RESET);
    if (mask & (1U << 4)) HAL_GPIO_WritePin(GPIOD, SEG_E_PIN, GPIO_PIN_RESET);
    if (mask & (1U << 5)) HAL_GPIO_WritePin(GPIOD, SEG_F_PIN, GPIO_PIN_RESET);
    if (mask & (1U << 6)) HAL_GPIO_WritePin(GPIOD, SEG_G_PIN, GPIO_PIN_RESET);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) //Configuración de la función callback (activa flags)
{
    uint32_t ahora = HAL_GetTick();

    if (GPIO_Pin == BTN_SUBIR_PIN) //PIN PB12
    {
        if ((ahora - ultimo_tick_subir) >= DEBOUNCE_MS)
        {
            ultimo_tick_subir = ahora;
            flag_subir = true;
        }
    }
    else if (GPIO_Pin == BTN_BAJAR_PIN) //PIN PB13
    {
        if ((ahora - ultimo_tick_bajar) >= DEBOUNCE_MS)
        {
            ultimo_tick_bajar = ahora;
            flag_bajar = true;
        }
    }
}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  mostrar_digito_7seg(contador);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
      {
          /* USER CODE BEGIN WHILE */
          bool actualizar = false;

          if (flag_subir)
          {
              flag_subir = false;
              contador = (contador >= CONTADOR_MAX) ? CONTADOR_MIN : (uint8_t)(contador + 1U);
              actualizar = true;
          }

          if (flag_bajar)
          {
              flag_bajar = false;
              contador = (contador <= CONTADOR_MIN) ? CONTADOR_MAX : (uint8_t)(contador - 1U);
              actualizar = true;
          }

          if (actualizar)
          {
              mostrar_digito_7seg(contador);
          }
          /* USER CODE END WHILE */

          /* USER CODE BEGIN 3 */
          /* USER CODE END 3 */
      }
  }


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB12 PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 PD2 PD4
                           PD5 PD6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PD3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
