/****************************************************************************/
/*    							                                                */
/*                                                                          */
/*    These files contain the attributes and methods needed to manage a     */
/*    ring buffer. It turns out that we do this so much, that it may be a   */
/*    good idea to just make a library that does this.                      */
/*                                                                          */
/*      BY:   Ken Wada                                                      */
/*            Aurium Technologies Inc.                                      */
/*            15-July-2013                                                  */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*    THE COMPANY: Aurium Technologies Inc.                                 */
/*                                                                          */
/*    THIS FIRMWARE IS FREE TO USE AND TO MODIFY. IT IS PRESENTED HERE FOR  */
/*    DEMONSTRATION PURPOSES AS AN EXAMPLE ON HOW TO CODE FOR ANY DEEPLY    */
/*    EMBEDDED SYSTEM.                                                      */
/*                                                                          */
/*    AS A RESULT, THE COMPANY SHALL NOT BE HELD LIABLE FOR ANY DIRECT,     */
/*    INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING  */
/*    FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY USERS OF     */
/*    THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR      */
/*    PRODUCTS.                                                             */
/****************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "config.h"

#define RBUF_SIZE 256

typedef struct RingBuffer
{
    uint8_t head;
    uint8_t tail;
    uint8_t count;
    uint8_t buf[RBUF_SIZE];
} RingBuffer;

void RingBuffer_init(RingBuffer *_this);
uint8_t RingBuffer_empty(RingBuffer *_this);
uint8_t RingBuffer_full(RingBuffer *_this);
uint8_t RingBuffer_pop(RingBuffer *_this);
void RingBuffer_push(RingBuffer *_this, uint8_t value);
void RingBuffer_flush(RingBuffer *_this, uint8_t clearBuffer);
bool RingBuffer_erase(RingBuffer *_this, uint8_t range);
uint8_t RingBuffer_get(RingBuffer *_this, uint8_t index);

#ifdef __cplusplus
}
#endif

#endif /* __RINGBUFFER_H */