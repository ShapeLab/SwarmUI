/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file RF24.h
 *
 * Class declaration for RF24 and helper enums
 */

#ifndef __RF24_H__
#define __RF24_H__

#include "main.h"
#include "utilities.h"
#include "nRF24L01.h"

#define _BV(x) (1 << (x))

//#include "RF24_config.h"

/**
 * Power Amplifier level.
 *
 * For use with setPALevel()
 */
typedef enum
{
    RF24_PA_MIN = 0,
    RF24_PA_LOW,
    RF24_PA_HIGH,
    RF24_PA_MAX,
    RF24_PA_ERROR
} rf24_pa_dbm_e;

/**
 * Data rate.  How fast data moves through the air.
 *
 * For use with setDataRate()
 */
typedef enum
{
    RF24_1MBPS = 0,
    RF24_2MBPS,
    RF24_250KBPS
} rf24_datarate_e;

/**
 * CRC Length.  How big (if any) of a CRC is included.
 *
 * For use with setCRCLength()
 */
typedef enum
{
    RF24_CRC_DISABLED = 0,
    RF24_CRC_8,
    RF24_CRC_16
} rf24_crclength_e;

/**
 * Driver for nRF24L01(+) 2.4GHz Wireless Transceiver
 */

/**
   * @name Low-level internal interface.
   *
   *  Protected methods that address the chip directly.  Regular users cannot
   *  ever call these.  They are documented for completeness and for developers who
   *  may want to extend this class.
   */

/**
   * Set chip enable
   *
   * @param level HIGH to actively begin transmission or LOW to put in standby.  Please see data sheet
   * for a much more detailed description of this pin.
   */
void ce(int level);

/**
   * Read a chunk of data in from a register
   *
   * @param reg Which register. Use constants from nRF24L01.h
   * @param buf Where to put the data
   * @param len How many bytes of data to transfer
   * @return Current value of status register
   */
bool readRadioRegisters(uint8_t reg, uint8_t *buf, uint8_t len);

/**
   * Read single byte from a register
   *
   * @param reg Which register. Use constants from nRF24L01.h
   * @return Current value of register @p reg
   */
uint8_t readRadioRegister(uint8_t reg);

/**
   * Write a chunk of data to a register
   *
   * @param reg Which register. Use constants from nRF24L01.h
   * @param buf Where to get the data
   * @param len How many bytes of data to transfer
   * @return Current value of status register
   */
bool writeRadioRegisters(uint8_t reg, uint8_t *buf, uint8_t len);

/**
   * Write a single byte to a register
   *
   * @param reg Which register. Use constants from nRF24L01.h
   * @param value The new value to write
   * @return Current value of status register
   */
bool writeRadioRegister(uint8_t reg, uint8_t value);

/**
   * Write the transmit payload
   *
   * The size of data written is the fixed payload size, see getPayloadSize()
   *
   * @param buf Where to get the data
   * @param len Number of bytes to be sent
   * @return Current value of status register
   */
uint8_t write_payload(uint8_t *buf, uint8_t len);

/**
   * Read the receive payload
   *
   * The size of data read is the fixed payload size, see getPayloadSize()
   *
   * @param buf Where to put the data
   * @param len Maximum number of bytes to read
   * @return Current value of status register
   */
uint8_t read_payload(uint8_t *buf, uint8_t len);

/**
   * Empty the receive buffer
   *
   * @return Current value of status register
   */
bool flush_rx(void);

/**
   * Empty the transmit buffer
   *
   * @return Current value of status register
   */
bool flush_tx(void);

/**
   * Sets the width of the addresses, from 3 to 5 bytes
   *
   * @return true if successful
   */
bool setAddressWidth(uint8_t width);

/**
   * Retrieve the current status of the chip
   *
   * @return Current value of status register
   */
uint8_t get_status(void);

/**
   * Decode and print the given status to stdout
   *
   * @param status Status value to print
   *
   * @warning Does nothing if stdout is not defined.  See fdevopen in stdio.h
   */
void print_status(uint8_t status);

/**
   * Decode and print the given 'observe_tx' value to stdout
   *
   * @param value The observe_tx value to print
   *
   * @warning Does nothing if stdout is not defined.  See fdevopen in stdio.h
   */
void print_observe_tx(uint8_t value);

/**
   * Print the name and value of an 8-bit register to stdout
   *
   * Optionally it can print some quantity of successive
   * registers on the same line.  This is useful for printing a group
   * of related registers on one line.
   *
   * @param name Name of the register
   * @param reg Which register. Use constants from nRF24L01.h
   * @param qty How many successive registers to print
   */
void print_byte_register(const char *name, uint8_t reg, uint8_t qty);

/**
   * Print the name and value of a 40-bit address register to stdout
   *
   * Optionally it can print some quantity of successive
   * registers on the same line.  This is useful for printing a group
   * of related registers on one line.
   *
   * @param name Name of the register
   * @param reg Which register. Use constants from nRF24L01.h
   * @param qty How many successive registers to print
   */
void print_address_register(const char *name, uint8_t reg, uint8_t qty);

/**
   * Turn on or off the special features of the chip
   *
   * The chip has certain 'features' which are only available when the 'features'
   * are enabled.  See the datasheet for details.
   */
void toggle_features(void);
/**@}*/

/**
   * @name Primary public interface
   *
   *  These are the main methods you need to operate the chip
   */
/**@{*/

/**
   * Constructor
   *
   * Creates a new instance of this driver.  Before using, you create an instance
   * and send in the unique pins that this chip is connected to.
   *
   * @param _cepin The pin attached to Chip Enable on the RF module
   * @param _cspin The pin attached to Chip Select
   */

/**
   * Begin operation of the chip
   *
   * Call this in setup(), before calling any other methods.
   */
bool initNRF24L01P();

/**
   * Start listening on the pipes opened for reading.
   *
   * Be sure to call openReadingPipe() first.  Do not call write() while
   * in this mode, without first calling stopListening().  Call
   * isAvailable() to check for incoming traffic, and read() to get it.
   */
void startListening(void);

/**
   * Stop listening for incoming messages
   *
   * Do this before calling write().
   */
void stopListening(void);

/**
   * Write to the open writing pipe
   *
   * Be sure to call openWritingPipe() first to set the destination
   * of where to write to.
   *
   * This blocks until the message is successfully acknowledged by
   * the receiver or the timeout/retransmit maxima are reached.  In
   * the current configuration, the max delay here is 60ms.
   *
   * The maximum size of data written is the fixed payload size, see
   * getPayloadSize().  However, you can write less, and the remainder
   * will just be filled with zeroes.
   *
   * @param buf Pointer to the data to be sent
   * @param len Number of bytes to be sent
   * @return True if the payload was delivered successfully false if not
   */
bool writeRadio(uint8_t *buf, uint8_t len);

/**
   * Test whether there are bytes available to be read
   *
   * @return True if there is a payload available, false if none is
   */
//  bool available(void);

/**
   * Read the payload
   *
   * Return the last payload received
   *
   * The size of data read is the fixed payload size, see getPayloadSize()
   *
   * @note I specifically chose 'void*' as a data type to make it easier
   * for beginners to use.  No casting needed.
   *
   * @param buf Pointer to a buffer where the data should be written
   * @param len Maximum number of bytes to read into the buffer
   * @return True if the payload was delivered successfully false if not
   */
bool readRadio(uint8_t *buf, uint8_t len);

/**
   * Open a pipe for writing
   *
   * Only one pipe can be open at once, but you can change the pipe
   * you'll listen to.  Do not call this while actively listening.
   * Remember to stopListening() first.
   *
   * Addresses are 40-bit hex values, e.g.:
   *
   * @code
   *   openWritingPipe(0xF0F0F0F0F0);
   * @endcode
   *
   * @param address The 40-bit address of the pipe to open.  This can be
   * any value whatsoever, as long as you are the only one writing to it
   * and only one other radio is listening to it.  Coordinate these pipe
   * addresses amongst nodes on the network.
   */
void openWritingPipe(uint64_t value);

/**
   * Open a pipe for reading
   *
   * Up to 6 pipes can be open for reading at once.  Open all the
   * reading pipes, and then call startListening().
   *
   * @see openWritingPipe
   *
   * @warning Pipes 1-5 should share the first 32 bits.
   * Only the least significant byte should be unique, e.g.
   * @code
   *   openReadingPipe(1,0xF0F0F0F0AA);
   *   openReadingPipe(2,0xF0F0F0F066);
   * @endcode
   *
   * @warning Pipe 0 is also used by the writing pipe.  So if you open
   * pipe 0 for reading, and then startListening(), it will overwrite the
   * writing pipe.  Ergo, do an openWritingPipe() again before write().
   *
   * @todo Enforce the restriction that pipes 1-5 must share the top 32 bits
   *
   * @param number Which pipe# to open, 0-5.
   * @param address The 40-bit address of the pipe to open.
   */
void openReadingPipe(uint8_t pipeNb, uint64_t address);

/**@}*/
/**
   * @name Optional Configurators
   *
   *  Methods you can use to get or set the configuration of the chip.
   *  None are required.  Calling begin() sets up a reasonable set of
   *  defaults.
   */
/**@{*/
/**
   * Set the number and delay of retries upon failed submit
   *
   * @param delay How long to wait between each retry, in multiples of 250us,
   * max is 15.  0 means 250us, 15 means 4000us.
   * @param count How many retries before giving up, max 15
   */
bool setRetries(uint8_t delay, uint8_t count);

/**
   * Set RF communication channel
   *
   * @param channel Which RF channel to communicate on, 0-127
   */
bool setChannel(uint8_t channel);

/**
   * Get RF communication channel
   *
   * returns which RF channel to communicate on, 0-127
   */
uint8_t getChannel();

/**
   * Set Static Payload Size
   *
   * This implementation uses a pre-stablished fixed payload size for all
   * transmissions.  If this method is never called, the driver will always
   * transmit the maximum payload size (32 bytes), no matter how much
   * was sent to write().
   *
   * @todo Implement variable-sized payloads feature
   *
   * @param size The number of bytes in the payload
   */
void setPayloadSize(uint8_t size);

/**
   * Get Static Payload Size
   *
   * @see setPayloadSize()
   *
   * @return The number of bytes in the payload
   */
uint8_t getPayloadSize(void);

/**
   * Get Dynamic Payload Size
   *
   * For dynamic payloads, this pulls the size of the payload off
   * the chip
   *
   * @return Payload length of last-received dynamic payload
   */
uint8_t getDynamicPayloadSize(void);

/**
   * Enable custom payloads on the acknowledge packets
   *
   * Ack payloads are a handy way to return data back to senders without
   * manually changing the radio modes on both units.
   *
   * @see examples/pingpair_pl/pingpair_pl.pde
   */
bool enableAckPayload(void);

/**
   * Enable dynamically-sized payloads
   *
   * This way you don't always have to send large packets just to send them
   * once in a while.  This enables dynamic payloads on ALL pipes.
   *
   * @see examples/pingpair_pl/pingpair_dyn.pde
   */
void enableDynamicPayloads(void);

/**
   * Determine whether the hardware is an nRF24L01+ or not.
   *
   * @return true if the hardware is nRF24L01+ (or compatible) and false
   * if its not.
   */
bool isPVariant(void);

/**
   * Enable or disable auto-acknowlede packets
   *
   * This is enabled by default, so it's only needed if you want to turn
   * it off for some reason.
   *
   * @param enable Whether to enable (true) or disable (false) auto-acks
   */
bool setAutoAck(bool enable);

/**
   * Enable or disable auto-acknowlede packets on a per pipeline basis.
   *
   * AA is enabled by default, so it's only needed if you want to turn
   * it off/on for some reason on a per pipeline basis.
   *
   * @param pipe Which pipeline to modify
   * @param enable Whether to enable (true) or disable (false) auto-acks
   */
//  void setAutoAck( uint8_t pipe, bool enable ) ;

/**
   * Set Power Amplifier (PA) level to one of four levels.
   * Relative mnemonics have been used to allow for future PA level
   * changes. According to 6.5 of the nRF24L01+ specification sheet,
   * they translate to: RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm,
   * RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
   *
   * @param level Desired PA level.
   */
bool setPALevel(rf24_pa_dbm_e level);

/**
   * Fetches the current PA level.
   *
   * @return Returns a value from the rf24_pa_dbm_e enum describing
   * the current PA setting. Please remember, all values represented
   * by the enum mnemonics are negative dBm. See setPALevel for
   * return value descriptions.
   */
rf24_pa_dbm_e getPALevel(void);
int8_t getRfOutputPower(void);

/**
   * Set the transmission data rate
   *
   * @warning setting RF24_250KBPS will fail for non-plus units
   *
   * @param speed RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
   * @return true if the change was successful
   */
bool setDataRate(rf24_datarate_e speed);

/**
   * Fetches the transmission data rate
   *
   * @return Returns the hardware's currently configured datarate. The value
   * is one of 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS
   *  enum.
   */
uint16_t getDataRate(void);

/**
   * Set the CRC length
   *
   * @param length RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
   */
bool setCRCLength(rf24_crclength_e length);

/**
   * Get the CRC length
   *
   * @return RF24_DISABLED if disabled or RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
   */
rf24_crclength_e getCRCLength(void);

/**
   * Disable CRC validation
   *
   */
void disableCRC(void);

/**@}*/
/**
   * @name Advanced Operation
   *
   *  Methods you can use to drive the chip in more advanced ways
   */
/**@{*/

/**
   * Print a giant block of debugging information to stdout
   *
   * @warning Does nothing if stdout is not defined.  See fdevopen in stdio.h
   */
void printDetails(void);

/**
   * Enter low-power mode
   *
   * To return to normal power mode, either write() some data or
   * startListening, or powerUp().
   */
bool powerDown(void);

/**
   * Leave low-power mode - making radio more responsive
   *
   * To return to low power mode, call powerDown().
   */
bool powerUp(void);

/**
   * Test whether there are bytes available to be read
   *
   * Use this version to discover on which pipe the message
   * arrived.
   *
   * @param[out] pipe_num Which pipe has the payload available
   * @return True if there is a payload available, false if none is
   */
bool availableRadio(uint8_t *pipe_num);

/**
   * Non-blocking write to the open writing pipe
   *
   * Just like write(), but it returns immediately. To find out what happened
   * to the send, catch the IRQ and then call whatHappened().
   *
   * @see write()
   * @see whatHappened()
   *
   * @param buf Pointer to the data to be sent
   * @param len Number of bytes to be sent
   * @return True if the payload was delivered successfully false if not
   */
void startWrite(uint8_t *buf, uint8_t len);

/**
   * Write an ack payload for the specified pipe
   *
   * The next time a message is received on @p pipe, the data in @p buf will
   * be sent back in the acknowledgement.
   *
   * @warning According to the data sheet, only three of these can be pending
   * at any time.  I have not tested this.
   *
   * @param pipe Which pipe# (typically 1-5) will get this response.
   * @param buf Pointer to data that is sent
   * @param len Length of the data to send, up to 32 bytes max.  Not affected
   * by the static payload set by setPayloadSize().
   */
void writeAckPayload(uint8_t pipe, uint8_t *buf, uint8_t len);

/**
   * Determine if an ack payload was received in the most recent call to
   * write().
   *
   * Call read() to retrieve the ack payload.
   *
   * @warning Calling this function clears the internal flag which indicates
   * a payload is available.  If it returns true, you must read the packet
   * out as the very next interaction with the radio, or the results are
   * undefined.
   *
   * @return True if an ack payload is available.
   */
bool isAckPayloadAvailable(void);

/**
   * Call this when you get an interrupt to find out why
   *
   * Tells you what caused the interrupt, and clears the state of
   * interrupts.
   *
   * @param[out] tx_ok The send was successful (TX_DS)
   * @param[out] tx_fail The send failed, too many retries (MAX_RT)
   * @param[out] rx_ready There is a message waiting to be read (RX_DS)
   */
void whatHappened(bool *tx_ok, bool *tx_fail, bool *rx_ready);

/**
   * Test whether there was a carrier on the line for the
   * previous listening period.
   *
   * Useful to check for interference on the current channel.
   *
   * @return true if was carrier, false if not
   */
bool testCarrier(void);

/**
   * Test whether a signal (carrier or otherwise) greater than
   * or equal to -64dBm is present on the channel. Valid only
   * on nRF24L01P (+) hardware. On nRF24L01, use testCarrier().
   *
   * Useful to check for interference on the current channel and
   * channel hopping strategies.
   *
   * @return true if signal => -64dBm, false if not
   */
bool testRPD(void);

/**
   * Checks if the RX Fifo is empty or not
   *
   * @return true if empty, false if not
   */
bool isRxFifoEMpty();

/**
   * Sets the interrupt sources for the IRQ pin
   *
   * @return true if the register has been correctly set, false if not
   */
bool setInterruptSource(bool rx_data_ready, bool tx_data_sent, bool max_retransmit);

/**
 * Clear the interrupt flags given in parameter
 *
 * @returns nothing
 */
void clearInterruptFlag(bool rx_dr, bool tx_ds, bool max_rt);

/**
 * Gets the current number of lost packets
 *
 * @returns the number of lost packets
 */
uint8_t getNbLostPacket(void);

/**
 * Resets the lost packets counter
 *
 * @returns true if successful
 */
bool resetNbLostPacket(void);

#endif // __RF24_H__
// vim:ai:cin:sts=2 sw=2 ft=cpp