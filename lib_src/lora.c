
#include <stdio.h>

#include <lora.h>
#include <lora_defs.h>

// inspired by:
// https://github.com/sandeepmistry/arduino-LoRa/blob/master/src/LoRa.cpp

#define WAIT(time){uint32_t cnt = time * 2; while(cnt--){ asm volatile("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");}}

static spiExchangeHandle spiExchange = 0;
static setPortHandle setSS = 0;
static setPortHandle setRESET = 0;

static uint64_t lora_freq = 0;
static uint8_t read_callback = 0;
static uint8_t writing_in_progress = 0;

static volatile uint8_t  rcv_complete = 0;
static volatile uint8_t* rcv_buffer;
static volatile uint8_t  rcv_size = 0;

static uint8_t singleTransfer(uint8_t address, uint8_t value)
{
    setSS(0);
    spiExchange(address);
    uint8_t ret = spiExchange(value);
    setSS(1);
    return ret;
}

static void writeRegister(uint8_t addr, uint8_t value) { singleTransfer(addr | 0x80, value); }
static uint8_t readRegister(uint8_t address) { return singleTransfer(address & 0x7f, 0x00); }

void loraSleep()
    { writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP); }

void loraStandBy()
    { writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY); }

void loraExplicitHeaderMode()
    { writeRegister(REG_MODEM_CONFIG_1, readRegister(REG_MODEM_CONFIG_1) & 0xfe);}

void loraImplicitHeaderMode()
    { writeRegister(REG_MODEM_CONFIG_1, readRegister(REG_MODEM_CONFIG_1) | 0x01); }

float loraPacketSnr()
    { return ((int8_t)readRegister(REG_PKT_SNR_VALUE)) * 0.25f; }

int loraPacketRssi()
    { return (readRegister(REG_PKT_RSSI_VALUE) - (lora_freq < 868E6 ? 164 : 157)); }

void loraSetSyncWord(int sw)
    { writeRegister(REG_SYNC_WORD, sw); }

void loraSetFrequency(uint64_t frequency)
{
  uint64_t frf = ((uint64_t)frequency << 19) / 32000000;
  lora_freq = frequency;

  writeRegister(REG_FRF_MSB, (uint8_t)(frf >> 16));
  writeRegister(REG_FRF_MID, (uint8_t)(frf >> 8));
  writeRegister(REG_FRF_LSB, (uint8_t)(frf >> 0));
}

void loraSetOCP(uint8_t mA)
{
  uint8_t ocpTrim = 27;

  if (mA <= 120) { ocpTrim = (mA - 45) / 5; }
  else if (mA <=240) { ocpTrim = (mA + 30) / 10; }

  writeRegister(REG_OCP, 0x20 | (0x1F & ocpTrim));
}

void loraSetTxPower(int level)
{
    if (level > 17) 
    {
        if (level > 20) { level = 20; }
        level -= 3;
        // High Power +20 dBm Operation (Semtech SX1276/77/78/79 5.4.3.)
        writeRegister(REG_PA_DAC, 0x87);
        loraSetOCP(140);
    }
    else
    {
        //Default value PA_HF/LF or +17dBm
        if (level < 2) { level = 2; }
        writeRegister(REG_PA_DAC, 0x84);
        loraSetOCP(100);
    }

    writeRegister(REG_PA_CONFIG, PA_BOOST | (level - 2));
}

void loraReset()
{
    setRESET(0);
    WAIT(10000);
    setRESET(1);
    WAIT(10000);
}

void dio0IntCallback(void)
{
    WAIT(100);
    int irqFlags = readRegister(REG_IRQ_FLAGS);
    writeRegister(REG_IRQ_FLAGS, irqFlags);

    if ((irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) != 0) 
    {
        printf("crc err\n");
    }

    if ((irqFlags & IRQ_RX_DONE_MASK) != 0) 
    {
        uint8_t rcv_len = readRegister(REG_RX_NB_BYTES);
        //printf("rx fin %d\n", rcv_len);
        writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_CURRENT_ADDR));

        for(int idx = 0; idx < rcv_len; idx++)
            rcv_buffer[idx] = readRegister(REG_FIFO);

        rcv_size = rcv_len;
        rcv_complete = 1;
    }

    if ((irqFlags & IRQ_TX_DONE_MASK) != 0) 
    {
        WAIT(100);
        writeRegister(REG_DIO_MAPPING_1, 0x00); // DIO0 => RXDONE
        writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
        writing_in_progress = 0;
    }

    return;
}

intCallbackHandle loraInit(spiExchangeHandle spi_exchange_handle, setPortHandle ss_handle, setPortHandle rst_handle, uint64_t freq, uint8_t* buffer)
{
    if(spi_exchange_handle == 0 || ss_handle == 0 || rst_handle == 0 || buffer == 0)
        return 0;

    spiExchange = spi_exchange_handle;
    setSS = ss_handle;
    setRESET = rst_handle;
    rcv_buffer = buffer;

    setSS(1);
    loraReset();

    if (readRegister(REG_VERSION) != 0x12)
        return 0;

    loraSleep();
    loraSetFrequency(freq);

    writeRegister(REG_FIFO_TX_BASE_ADDR, 0);
    writeRegister(REG_FIFO_RX_BASE_ADDR, 0);

    // set auto AGC
    writeRegister(REG_LNA, readRegister(REG_LNA) | 0x03);
    // set auto AGC
    writeRegister(REG_MODEM_CONFIG_3, 0x04);

       // set output power to 17 dBm
    loraSetTxPower(17);

    writeRegister(REG_DIO_MAPPING_1, 0x00); // DIO0 => RXDONE
    loraExplicitHeaderMode();
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);

    return &dio0IntCallback;
}

uint8_t loraReadPacket(uint8_t* data)
{
    while (rcv_complete == 0);

    rcv_complete = 0;

    return rcv_size;
}

void loraWritePacket(uint8_t* data, uint8_t size)
{
    while (writing_in_progress) WAIT(100);
    loraStandBy();
    writing_in_progress = 1;
    writeRegister(REG_FIFO_ADDR_PTR, 0);
    writeRegister(REG_PAYLOAD_LENGTH, size);
    writeRegister(REG_DIO_MAPPING_1, 0x40);

    while (size--)
        writeRegister(REG_FIFO, *data++);

    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
}