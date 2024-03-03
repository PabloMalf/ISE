#include "stm32f4xx_hal.h"
#include "Board_Buttons.h"

// Buttons for NUCLEOF429ZI
#define BUTTONS_COUNT                  (1)

#define BUTTONS_USER_MASK            	(1<<0)
#define BUTTONS_USER_PORT             GPIOC
#define BUTTONS_USER_PIN              GPIO_PIN_13


/**
  \fn          int32_t Buttons_Initialize (void)
  \brief       Initialize buttons
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Buttons_Initialize (void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOC_CLK_ENABLE();

  /* Configure GPIO pin: PG15 (USER) */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  return 0;
}

/**
  \fn          int32_t Buttons_Uninitialize (void)
  \brief       De-initialize buttons
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Buttons_Uninitialize (void) {

  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_13);

  return 0;
}

/**
  \fn          uint32_t Buttons_GetState (void)
  \brief       Get buttons state
  \returns     Buttons state
*/
uint32_t Buttons_GetState (void) {
  uint32_t val = 0;

  if (HAL_GPIO_ReadPin(BUTTONS_USER_PORT,   BUTTONS_USER_PIN)   == GPIO_PIN_RESET) {
    val |= BUTTONS_USER_MASK;
  }

  return val;
}

/**
  \fn          uint32_t Buttons_GetCount (void)
  \brief       Get number of available buttons
  \return      Number of available buttons
*/
uint32_t Buttons_GetCount (void) {
  return BUTTONS_COUNT;
}
