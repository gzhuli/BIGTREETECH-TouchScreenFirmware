#include "GPIO_Init.h"
#include "variants.h"  // for GPIO_TypeDef etc.

static GPIO_TypeDef * const GPIO_Port[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG};

void GPIO_InitSet(uint16_t io, GPIO_MODE mode, uint8_t AF)
{
  uint16_t port = GPIO_GET_PORT(io);
  uint16_t pin = GPIO_GET_PIN(io);

  RCC->APB2ENR |= 1 << (port+2);                        // enable GPIO Clock, GPIOA start at APB2 bit2

  if (mode == MGPIO_MODE_IPD)
  {
    GPIO_Port[port]->BSRR = 1 << (pin+16);              // set low
    mode = MGPIO_MODE_IPU;
  }
  else if (mode == MGPIO_MODE_IPU)
  {
    GPIO_Port[port]->BSRR = 1 << pin;                   // set high
  }

  if (pin <= 7)
  {
    GPIO_Port[port]->CRL &= ~(0xF << 4 * (pin & 0x7));  // clear control reg bits
    GPIO_Port[port]->CRL |= mode << 4 * (pin & 0x7);    // clear control reg bits
  }
  else
  {
    GPIO_Port[port]->CRH &= ~(0xF << 4 * (pin & 0x7));  // clear control reg bits
    GPIO_Port[port]->CRH |= mode << 4 * (pin & 0x7);    // clear control reg bits
  }
}

void GPIO_SetLevel(uint16_t io, uint8_t level)
{
  uint16_t port = GPIO_GET_PORT(io);
  uint16_t pin = GPIO_GET_PIN(io);

  GPIO_Port[port]->BSRR = 1 << (level ? pin : (pin+16));
}

void GPIO_ToggleLevel(uint16_t io)
{
  uint16_t port = GPIO_GET_PORT(io);
  uint16_t pin = GPIO_GET_PIN(io);

  GPIO_Port[port]->ODR ^= (1 << pin);
}

uint8_t GPIO_GetLevel(uint16_t io)
{
  uint16_t port = GPIO_GET_PORT(io);
  uint16_t pin = GPIO_GET_PIN(io);

  if ((GPIO_Port[port]->IDR & (1 << pin)) != 0)
    return 1;

  return 0;
}
