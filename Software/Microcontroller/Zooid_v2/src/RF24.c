/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "RF24.h"

/****************************************************************************/

bool wide_band;                 /* 2Mbs data rate in use? */
bool p_variant;                 /* False for RF24L01 and true for RF24L01P */
uint8_t payload_size;           /**< Fixed size of payloads */
bool ack_payload_available;     /**< Whether there is an ack payload waiting */
bool dynamic_payloads_enabled;  /**< Whether dynamic payloads are enabled. */
uint8_t ack_payload_length;     /**< Dynamic size of pending ack payload. */
uint64_t pipe0_reading_address; /**< Last address set on pipe 0 for reading. */

//const uint64_t pipes[25] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D1LL, 0xF0F0F0F0D1DL, 0xF0F0F0F0D1BL, 0xF0F0F0F0D1CL,
//                             0xF0F0F0F0E2LL, 0xF0F0F0F0D2LL, 0xF0F0F0F0D2DL, 0xF0F0F0F0D2BL, 0xF0F0F0F0D2CL,
//                             0xF0F0F0F0E3LL, 0xF0F0F0F0D3LL, 0xF0F0F0F0D3DL, 0xF0F0F0F0D3BL, 0xF0F0F0F0D3CL,
//                             0xF0F0F0F0E4LL, 0xF0F0F0F0D4LL, 0xF0F0F0F0D4DL, 0xF0F0F0F0D4BL, 0xF0F0F0F0D4CL,
//                             0xF0F0F0F0E5LL, 0xF0F0F0F0D5LL, 0xF0F0F0F0D5DL, 0xF0F0F0F0D5BL, 0xF0F0F0F0D5CL};

static const uint8_t child_pipe[] =
    {RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5};
static const uint8_t child_payload_size[] =
    {RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5};
static const uint8_t child_pipe_enable[] =
    {ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5};


uint8_t myChannel = RADIO_CHANNEL;

bool isListening;

/****************************************************************************/
bool initNRF24L01P()
{
    bool initOK = true;
    uint8_t tmpBuffer = 0;
    wide_band = true;
    p_variant = false;
    payload_size = 32;
    ack_payload_available = false;
    dynamic_payloads_enabled = false;
    pipe0_reading_address = 0;
    isListening = false;

    // Must allow the radio time to settle else configuration bits will not necessarily stick.
    // This is actually only required following power up but some settling time also appears to
    // be required after resets too. For full coverage, we'll always assume the worst.
    // Enabling 16b CRC is by far the most obvious case if the wrong timing is used - or skipped.
    // Technically we require 4.5ms + 14us as a worst case. We'll just call it 5ms for good measure.
    // WARNING: Delay is based on P-variant whereby non-P *may* require different timing.

    ce(LOW);

    HAL_Delay(5);
    // Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
    // WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
    // sizes must never be used. See documentation for a more complete explanation.
    initOK &= setRetries(1, 2); //(0+1)x250us delay 5 tries
    // Restore our default PA level
    initOK &= setPALevel(RF24_PA_MAX);
    // Determine if this is a p or non-p RF24 module and then
    // reset our data rate back to default value. This works
    // because a non-P variant won't allow the data rate to
    // be set to 250Kbps.
    if (setDataRate(RF24_250KBPS))
        p_variant = true;
    else
        initOK = false;

    initOK &= setInterruptSource(true, true, true);
    // Then set the data rate to the slowest (and most reliable) speed supported by all
    // hardware.
    initOK &= setDataRate(RF24_1MBPS);
    // Initialize CRC and request 1-byte (8bit) CRC
    initOK &= setCRCLength(RF24_CRC_8);
    // Disable dynamic payloads, to match dynamic_payloads_enabled setting
    writeRadioRegister(DYNPD, 0);
    initOK &= ((readRadioRegister(DYNPD) == 0) ? true : false);
    // Reset current status
    // Notice reset and flush is the last thing we do

    clearInterruptFlag(true, true, true);
    // Set up default configuration.  Callers can always change it later.
    // This channel should be universally safe and not bleed over into adjacent
    // spectrum.
    initOK &= setChannel(76);

    //enable pipe0
    uint8_t reg = readRadioRegister(EN_RXADDR);
    reg |= _BV(child_pipe_enable[0]);
    writeRadioRegister(EN_RXADDR, readRadioRegister(EN_RXADDR) | _BV(child_pipe_enable[0]));

    // Flush buffers
    flush_rx();
    flush_tx();
    // Open the pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
    return initOK;
}

/****************************************************************************/
void startListening(void)
{
    if (isListening == false)
    {
        writeRadioRegister(CONFIG, readRadioRegister(CONFIG) | _BV(PWR_UP) | _BV(PRIM_RX));
        clearInterruptFlag(true, true, true);

        // Restore the pipe0 adddress, if exists
        if (pipe0_reading_address)
            writeRadioRegisters(RX_ADDR_P0, (uint8_t *)&pipe0_reading_address, 5);

        // Flush buffers
        flush_rx();
        flush_tx();

        // Go!
        ce(HIGH);

        // wait for the radio to come up (130us actually only needed)

        delayMicroseconds(150);
        isListening = true;
    }
}

/****************************************************************************/
void stopListening(void)
{
    if (isListening == true)
    {
        ce(LOW);
        flush_tx();
        flush_rx();
        isListening = false;
    }
}

/******************************************************************/
bool writeRadio(uint8_t *buf, uint8_t len)
{
    bool result = false;

    // Begin the write
    startWrite(buf, len);

    // ------------
    // At this point we could return from a non-blocking write, and then call
    // the rest after an interrupt

    // Instead, we are going to block here until we get TX_DS (transmission completed and ack'd)
    // or MAX_RT (maximum retries, transmission failed).  Also, we'll timeout in case the radio
    // is flaky and we get neither.

    // IN the end, the send should be blocking.  It comes back in 60ms worst case, or much faster
    // if I tighted up the retry logic.  (Default settings will be 1500us.
    // Monitor the send
    uint8_t observe_tx;
    uint8_t status;
    uint32_t sent_at = HAL_GetTick();
    const uint32_t timeout = 10; //ms to wait for timeout

    do
    {
        status = readRadioRegisters(OBSERVE_TX, &observe_tx, 1);

        //IF_SERIAL_DEBUG(Serial.print(observe_tx,HEX));
    } while (!(status & (_BV(TX_DS) | _BV(MAX_RT))) && (HAL_GetTick() - sent_at < timeout));

 
    // The part above is what you could recreate with your own interrupt handler,
    // and then call this when you got an interrupt
    // ------------

    // Call this when you get an interrupt
    // The status tells us three things
    // * The send was successful (TX_DS)
    // * The send failed, too many retries (MAX_RT)
    // * There is an ack packet waiting (RX_DR)
    bool tx_ok, tx_fail;
    whatHappened(&tx_ok, &tx_fail, &ack_payload_available);

    result = tx_ok;
    //IF_SERIAL_DEBUG(Serial.print(result?"...OK.":"...Failed"));

    // Handle the ack packet
    if (ack_payload_available)
    {
        ack_payload_length = getDynamicPayloadSize();\
    }

    // Yay, we are done.

    // Power down
    powerDown();

    // Flush buffers (Is this a relic of past experimentation, and not needed anymore??)
    flush_tx();

    return result;
}

/****************************************************************************/
void startWrite(uint8_t *buf, uint8_t len)
{
    uint8_t config = readRadioRegister(CONFIG);
    // Clear the Status bit
    clearInterruptFlag(false, true, false);

    // Send the payload
    write_payload(buf, len);

    // Transmitter power-up
    config = (config | _BV(PWR_UP)) & ~_BV(PRIM_RX);
    writeRadioRegister(CONFIG, config);

    // Allons!
    ce(HIGH);
    delayMicroseconds(15);
    ce(LOW);

    delayMicroseconds(150);
}

/****************************************************************************/
uint8_t write_payload(uint8_t *buf, uint8_t len)
{
    if (len > PAYLOAD_MAX_SIZE)
        len = PAYLOAD_MAX_SIZE;

    return SPIWriteRegisters(W_TX_PAYLOAD, len, buf);
}

/****************************************************************************/
bool readRadio(uint8_t *buf, uint8_t len)
{
    // Fetch the payload
    read_payload(buf, len);
    delayMicroseconds(150);

    return true;
    // was this the last of the data available?
    //    return readRadioRegister(FIFO_STATUS) & _BV(RX_EMPTY);
}

/****************************************************************************/
uint8_t read_payload(uint8_t *buf, uint8_t len)
{
    uint8_t status;

    uint8_t bufferRx[PAYLOAD_MAX_SIZE] = {0};

    if (len > PAYLOAD_MAX_SIZE)
        len = PAYLOAD_MAX_SIZE;

    if ((status = SPIReadBytes(R_RX_PAYLOAD, len, bufferRx)) != 0xff)
    {
        memcpy_fast(buf, bufferRx, len);
        return status;
    }
}

/****************************************************************************/
bool powerDown(void)
{
    uint8_t config = readRadioRegister(CONFIG) & ~_BV(PWR_UP);
    writeRadioRegister(CONFIG, config);

    return ((readRadioRegister(CONFIG) == config) ? true : false);
}

/****************************************************************************/
bool powerUp(void)
{
    uint8_t config = readRadioRegister(CONFIG) | _BV(PWR_UP);
    writeRadioRegister(CONFIG, config);

    return ((readRadioRegister(CONFIG) == config) ? true : false);
}

/****************************************************************************/
uint8_t getDynamicPayloadSize(void)
{
    uint8_t payloadSize = 0;
    SPIWriteCommand(R_RX_PL_WID, &payloadSize);
    return payloadSize;
}

/****************************************************************************/
bool availableRadio(uint8_t *pipe_num)
{
    uint8_t status = get_status();

    // Too noisy, enable if you really want lots o data!!
    //IF_SERIAL_DEBUG(print_status(status));

    bool result = (status & _BV(RX_DR));

    if (result)
    {
        // If the caller wants the pipe number, include that
        if (pipe_num)
            *pipe_num = (status >> RX_P_NO) & 0b111;

        // Clear the status bit

        // ??? Should this REALLY be cleared now?  Or wait until we
        // actually READ the payload?

        clearInterruptFlag(true, false, false);

        // Handle ack payload receipt
        if (status & _BV(TX_DS))
            clearInterruptFlag(false, true, true);
    }

    return result;
}

/****************************************************************************/
bool isRxFifoEMpty()
{
    return readRadioRegister(FIFO_STATUS) & _BV(RX_EMPTY);
}

/****************************************************************************/
void whatHappened(bool *tx_ok, bool *tx_fail, bool *rx_ready)
{
    // Read the status & reset the status in one easy call
    // Or is that such a good idea?

    uint8_t status = get_status();

    *tx_ok = (status & _BV(TX_DS)) >> TX_DS;
    *tx_fail = (status & _BV(MAX_RT)) >> MAX_RT;
    *rx_ready = (status & _BV(RX_DR)) >> RX_DR;
    // Report to the user what happened

    //    if (DEBUG_ENABLED())
    //    {
    //        if (*tx_ok)
    //            debug_printf("TX_DS\n");
    //        if (*tx_fail)
    //            debug_printf("MAX_RT\n");
    //        if (*rx_ready)
    //            debug_printf("RX_DR\n");
    //    }
}

/****************************************************************************/
void openWritingPipe(uint64_t value)
{
    // Note that AVR 8-bit uC's store this LSB first, and the NRF24L01(+)
    // expects it LSB first too, so we're good.
    writeRadioRegisters(TX_ADDR, (uint8_t *)&value, 5);
}

/****************************************************************************/
void openReadingPipe(uint8_t child, uint64_t address)
{
    // If this is pipe 0, cache the address.  This is needed because
    // openWritingPipe() will overwrite the pipe 0 address, so
    // startListening() will have to restore it.
    if (child == 0)
        pipe0_reading_address = address;

    // For pipes 2-5, only write the LSB
    if (child < 2)
        writeRadioRegisters(child_pipe[child], (uint8_t *)&address, 5);
    else
        writeRadioRegisters(child_pipe[child], (uint8_t *)&address, 1);

    //        writeRadioRegister(child_payload_size[child], payload_size);

    // Note it would be more efficient to set all of the bits for all open
    // pipes at once.  However, I thought it would make the calling code
    // more simple to do it this way.

    //    uint8_t reg = readRadioRegister(EN_RXADDR);
    //    reg |= _BV(child_pipe_enable[child]);
    //
    //    writeRadioRegister(EN_RXADDR, readRadioRegister(EN_RXADDR) | _BV(child_pipe_enable[child]));
}

/****************************************************************************/
void toggle_features(void)
{
    SPIWriteRegister(ACTIVATE, 0x73);
}

/****************************************************************************/
void enableDynamicPayloads(void)
{
    // Enable dynamic payload throughout the system
    writeRadioRegister(FEATURE, readRadioRegister(FEATURE) | _BV(EN_DPL));

    // If it didn't work, the features are not enabled
    if (!readRadioRegister(FEATURE))
    {
        // So enable them and try again
        toggle_features();
        writeRadioRegister(FEATURE, readRadioRegister(FEATURE) | _BV(EN_DPL));
    }

    //IF_SERIAL_DEBUG(printf("FEATURE=%i\r\n",readRadioRegister(FEATURE)));

    // Enable dynamic payload on all pipes
    //
    // Not sure the use case of only having dynamic payload on certain
    // pipes, so the library does not support it.
    writeRadioRegister(DYNPD, readRadioRegister(DYNPD) | _BV(DPL_P5) | _BV(DPL_P4) | _BV(DPL_P3) | _BV(DPL_P2) | _BV(DPL_P1) | _BV(DPL_P0));

    dynamic_payloads_enabled = true;
}

/****************************************************************************/
bool enableAckPayload(void)
{
    //
    // enable ack payload and dynamic payload features
    //

    enableDynamicPayloads();
    uint8_t features = readRadioRegister(FEATURE) | _BV(EN_ACK_PAY) | _BV(EN_DPL);
    uint8_t dynpd = readRadioRegister(DYNPD) | _BV(DPL_P5) | _BV(DPL_P4) | _BV(DPL_P3) | _BV(DPL_P2) | _BV(DPL_P1) | _BV(DPL_P0);
    writeRadioRegister(FEATURE, features);

    // If it didn't work, the features are not enabled
    if (readRadioRegister(FEATURE) != features)
    {
        // So enable them and try again
        toggle_features();
        writeRadioRegister(FEATURE, readRadioRegister(FEATURE) | _BV(EN_ACK_PAY) | _BV(EN_DPL));
    }

    writeRadioRegister(DYNPD, dynpd);

    return ((readRadioRegister(FEATURE) == features && readRadioRegister(DYNPD)) ? true : false);
}

/****************************************************************************/
void writeAckPayload(uint8_t pipe, uint8_t *buf, uint8_t len)
{
    if (len <= 32)
        SPIWriteRegisters(W_ACK_PAYLOAD | (pipe & 0b111), len, buf);
    else
        SPIWriteRegisters(W_ACK_PAYLOAD | (pipe & 0b111), 32, buf);
}

/****************************************************************************/
bool isAckPayloadAvailable(void)
{
    bool result = ack_payload_available;
    ack_payload_available = false;
    return result;
}

/****************************************************************************/
bool isPVariant(void)
{
    return p_variant;
}

/****************************************************************************/
bool setAutoAck(bool enable)
{
    uint8_t autoAck;
    if (enable)
        autoAck = 0b00111111;
    else
        autoAck = 0;

    writeRadioRegister(EN_AA, autoAck);

    return ((readRadioRegister(EN_AA) == autoAck) ? true : false);
}

/****************************************************************************/
bool testCarrier(void)
{
    return (readRadioRegister(CD) & 1);
}

/****************************************************************************/

bool testRPD(void)
{
    return (readRadioRegister(RPD) & 1);
}

/****************************************************************************/
bool setPALevel(rf24_pa_dbm_e level)
{
    uint8_t setup = readRadioRegister(RF_SETUP);
    setup &= ~(_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH));

    // switch uses RAM (evil!)
    if (level == RF24_PA_MAX)
    {
        setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH));
    }
    else if (level == RF24_PA_HIGH)
    {
        setup |= _BV(RF_PWR_HIGH);
    }
    else if (level == RF24_PA_LOW)
    {
        setup |= _BV(RF_PWR_LOW);
    }
    else if (level == RF24_PA_MIN)
    {
        // nothing
    }
    else if (level == RF24_PA_ERROR)
    {
        // On error, go to maximum PA
        setup |= (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH));
    }

    writeRadioRegister(RF_SETUP, setup);

    return ((readRadioRegister(RF_SETUP) == setup) ? true : false);
}

/****************************************************************************/
rf24_pa_dbm_e getPALevel(void)
{
    rf24_pa_dbm_e result = RF24_PA_ERROR;
    uint8_t power = readRadioRegister(RF_SETUP) & (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH));

    // switch uses RAM (evil!)
    if (power == (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH)))
    {
        result = RF24_PA_MAX;
    }
    else if (power == _BV(RF_PWR_HIGH))
    {
        result = RF24_PA_HIGH;
    }
    else if (power == _BV(RF_PWR_LOW))
    {
        result = RF24_PA_LOW;
    }
    else
    {
        result = RF24_PA_MIN;
    }

    return result;
}


/****************************************************************************/
int8_t getRfOutputPower(void)
{
    uint8_t rfPwr = readRadioRegister(RF_SETUP) & RF_PWR;

    switch (rfPwr)
    {
    case NRF24L01P_RF_SETUP_RF_PWR_0DBM:
        return NRF24L01P_TX_PWR_ZERO_DB;
    case NRF24L01P_RF_SETUP_RF_PWR_MINUS_6DBM:
        return NRF24L01P_TX_PWR_MINUS_6_DB;
    case NRF24L01P_RF_SETUP_RF_PWR_MINUS_12DBM:
        return NRF24L01P_TX_PWR_MINUS_12_DB;
    case NRF24L01P_RF_SETUP_RF_PWR_MINUS_18DBM:
        return NRF24L01P_TX_PWR_MINUS_18_DB;
    default:
        return 0xff;
    }
}

/****************************************************************************/
bool setDataRate(rf24_datarate_e speed)
{
    bool result = false;
    uint8_t setup = readRadioRegister(RF_SETUP);

    // HIGH and LOW '00' is 1Mbs - our default
    wide_band = false;
    setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));
    if (speed == RF24_250KBPS)
    {
        // Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
        // Making it '10'.
        wide_band = false;
        setup |= _BV(RF_DR_LOW);
    }
    else
    {
        // Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
        // Making it '01'
        if (speed == RF24_2MBPS)
        {
            wide_band = true;
            setup |= _BV(RF_DR_HIGH);
        }
        else
        {
            // 1Mbs
            wide_band = false;
        }
    }
    writeRadioRegister(RF_SETUP, setup);

    // Verify our result
    if (readRadioRegister(RF_SETUP) == setup)
    {
        result = true;
    }
    else
    {
        wide_band = false;
    }

    return result;
}

/****************************************************************************/
uint16_t getDataRate(void)
{
    uint16_t result;
    uint8_t dr = readRadioRegister(RF_SETUP) & (_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));

    // switch uses RAM (evil!)
    // Order matters in our case below
    if (dr == _BV(RF_DR_LOW))
    {
        // '10' = 250KBPS
        result = NRF24L01P_DATARATE_250_KBPS;
    }
    else if (dr == _BV(RF_DR_HIGH))
    {
        // '01' = 2MBPS
        result = NRF24L01P_DATARATE_2_MBPS;
    }
    else
    {
        // '00' = 1MBPS
        result = NRF24L01P_DATARATE_1_MBPS;
    }
    return result;
}

/****************************************************************************/
bool setCRCLength(rf24_crclength_e length)
{
    uint8_t config = readRadioRegister(CONFIG) & ~(_BV(CRCO) | _BV(EN_CRC));

    // switch uses RAM (evil!)
    if (length == RF24_CRC_DISABLED)
    {
        // Do nothing, we turned it off above.
    }
    else if (length == RF24_CRC_8)
    {
        config |= _BV(EN_CRC);
    }
    else
    {
        config |= _BV(EN_CRC);
        config |= _BV(CRCO);
    }
    writeRadioRegister(CONFIG, config);

    return ((readRadioRegister(CONFIG) == config) ? true : false);
}

/****************************************************************************/
rf24_crclength_e getCRCLength(void)
{
    rf24_crclength_e result = RF24_CRC_DISABLED;
    uint8_t config = readRadioRegister(CONFIG) & (_BV(CRCO) | _BV(EN_CRC));

    if (config & _BV(EN_CRC))
    {
        if (config & _BV(CRCO))
            result = RF24_CRC_16;
        else
            result = RF24_CRC_8;
    }

    return result;
}

/****************************************************************************/
void disableCRC(void)
{
    uint8_t disable = readRadioRegister(CONFIG) & ~_BV(EN_CRC);
    writeRadioRegister(CONFIG, disable);
}

/****************************************************************************/
bool setRetries(uint8_t delay, uint8_t count)
{
    uint8_t retries = (delay & 0x0F) << ARD | (count & 0x0F) << ARC;
    writeRadioRegister(SETUP_RETR, retries);

    return ((readRadioRegister(SETUP_RETR) == retries) ? true : false);
}

/****************************************************************************/
void clearInterruptFlag(bool rx_dr, bool tx_ds, bool max_rt)
{
    uint8_t reg = 0;
    if (rx_dr)
        reg |= _BV(RX_DR);
    if (tx_ds)
        reg |= _BV(TX_DS);
    if (max_rt)
        reg |= _BV(MAX_RT);

    writeRadioRegister(STATUS, reg);
}

/****************************************************************************/
bool setInterruptSource(bool rx_dr, bool tx_ds, bool max_rt)
{
    uint8_t config = readRadioRegister(CONFIG);

    config = (rx_dr == true) ? (config & ~_BV(MASK_RX_DR)) : (config | _BV(MASK_RX_DR));
    config = (tx_ds == true) ? (config & ~_BV(MASK_TX_DS)) : (config | _BV(MASK_TX_DS));
    config = (max_rt == true) ? (config & ~_BV(MASK_MAX_RT)) : (config | _BV(MASK_MAX_RT));

    writeRadioRegister(CONFIG, config);

    return ((readRadioRegister(CONFIG) == config) ? true : false);
}

/****************************************************************************/
void print_status(uint8_t status)
{
    if (DEBUG_ENABLED())
    {
        debug_printf("STATUS = 0x%02x RX_DR=%x TX_DS=%x MAX_RT=%x RX_P_NO=%x TX_FULL=%x\r\n",
            status,
            (status & _BV(RX_DR)) ? 1 : 0,
            (status & _BV(TX_DS)) ? 1 : 0,
            (status & _BV(MAX_RT)) ? 1 : 0,
            ((status >> RX_P_NO) & 0b111),
            (status & _BV(TX_FULL)) ? 1 : 0);
    }
}

/****************************************************************************/

void print_observe_tx(uint8_t value)
{
    //  debug_printf("OBSERVE_TX=%02x: POLS_CNT=%x ARC_CNT=%x\r\n",
    //	       value,
    //	       (value >> PLOS_CNT) & 0b1111,
    //	       (value >> ARC_CNT) & 0b1111);
}

/****************************************************************************/
void print_byte_register(const char *name, uint8_t reg, uint8_t qty)
{
    //  char extra_tab = strlen_P(name) < 8 ? '\t' : 0;
    //  printf_P(PSTR(PRIPSTR"\t%c ="),name,extra_tab);
    //  while (qty--)
    //    printf_P(PSTR(" 0x%02x"),readRadioRegister(reg++));
    //  printf_P(PSTR("\r\n"));
}

/****************************************************************************/
void print_address_register(const char *name, uint8_t reg, uint8_t qty)
{
    /*  char extra_tab = strlen_P(name) < 8 ? '\t' : 0;
  printf_P(PSTR(PRIPSTR"\t%c ="),name,extra_tab);

  while (qty--)
  {
    uint8_t buffer[5];
    readRadioRegister(reg++,buffer,sizeof buffer);

    printf_P(PSTR(" 0x"));
    uint8_t* bufptr = buffer + sizeof buffer;
    while( --bufptr >= buffer )
      printf_P(PSTR("%02x"),*bufptr);
  }

  printf_P(PSTR("\r\n"));*/
}

/****************************************************************************/
bool setChannel(uint8_t channel)
{
    uint8_t ch = (channel < 127) ? channel : 127;

    myChannel = ch;
    writeRadioRegister(RF_CH, ch);
    return ((readRadioRegister(RF_CH) == ch) ? true : false);
}

/****************************************************************************/

uint8_t getChannel()
{
    uint8_t channel = readRadioRegister(RF_CH) & 0x7F;
    return channel;
}

/****************************************************************************/

void setPayloadSize(uint8_t size)
{
    payload_size = size < 32 ? size : 32;
}

/****************************************************************************
uint8_t getPayloadSize(void)
{
    return payload_size;
}

/****************************************************************************/
void printDetails(void)
{
    if (DEBUG_ENABLED())
    {
        //  print_status(get_status());
        debug_printf("nRF24L01+ Frequency    : %d MHz\r\n", getChannel() + NRF24L01P_MIN_RF_FREQUENCY);
        debug_printf("nRF24L01+ Output power : %d dBm\r\n", getRfOutputPower());
        debug_printf("nRF24L01+ Data Rate    : %d kbps\r\n", getDataRate());
    }
}

/****************************************************************************/
uint8_t flush_rx(void)
{
    bool result;
    result = SPIWriteCommand(FLUSH_RX, NULL);
    return result;
}

/****************************************************************************/
uint8_t flush_tx(void)
{
    bool result;
    result = SPIWriteCommand(FLUSH_TX, NULL);
    return result;
}

/****************************************************************************/
bool setAddressWidth(uint8_t width)
{
    if(width >= 3 && width <= 5)
    {
        writeRadioRegister(SETUP_AW, width-2);
        return ((readRadioRegister(SETUP_AW) == width-2) ? true : false);
    }
    return false;
}

/****************************************************************************/
uint8_t get_status(void)
{
    uint8_t result;
    result = SPIWriteCommand(NOP, NULL);
    return result;
}

/****************************************************************************/
uint8_t getNbLostPacket(void)
{
    uint8_t result;
    result = (readRadioRegister(OBSERVE_TX) >> 4) & 0x0F;
    return result;
}

/**************************************************************************/
bool resetNbLostPacket(void)
{
    return setChannel(RADIO_CHANNEL);//myChannel);
}

/****************************************************************************/
void ce(int level)
{
    HAL_GPIO_WritePin(SPIx_CE_GPIO_PORT, SPIx_CE_PIN, level);
}

/*************************************************1***************************/
uint8_t readRadioRegisters(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t result;
    result = SPIReadBytes(R_REGISTER | (REGISTER_MASK & reg), len, buf);
    return result;
}
/****************************************************************************/
uint8_t readRadioRegister(uint8_t reg)
{
    uint8_t result = 0;
    SPIReadByte(R_REGISTER | (REGISTER_MASK & reg), &result);
    return result;
}

/****************************************************************************/
uint8_t writeRadioRegisters(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t result;
    result = SPIWriteRegisters(W_REGISTER | (REGISTER_MASK & reg), len, buf);
    return result;
}

/****************************************************************************/
uint8_t writeRadioRegister(uint8_t reg, uint8_t value)
{
    uint8_t result;
    result = SPIWriteRegister(W_REGISTER | (REGISTER_MASK & reg), value);
    return result;
}

