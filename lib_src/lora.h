#include <stdint.h>

#ifndef LORA_H
#define LORA_h

typedef uint8_t (*spiExchangeHandle) (uint8_t);
typedef void (*setPortHandle) (uint8_t);
typedef void (*intCallbackHandle) (void);

intCallbackHandle loraInit(spiExchangeHandle spi_exchange_handle, setPortHandle ss_handle, setPortHandle rst_handle, uint64_t freq, uint8_t* buffer);

#endif