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
#ifndef __ROBOTRINGBUFFER_H
#define __ROBOTRINGBUFFER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "config.h" 

#define ROBOTRINGBUFFER_SIZE    50


typedef struct RobotRingBuffer
{
  uint8_t head;
  uint8_t tail;
  uint8_t count;
  Robot buf[ROBOTRINGBUFFER_SIZE];
} RobotRingBuffer;

void  RobotRingBuffer_init(RobotRingBuffer *_this);
uint8_t RobotRingBuffer_empty(RobotRingBuffer *_this);
uint8_t RobotRingBuffer_full(RobotRingBuffer *_this);
Robot* RobotRingBuffer_pop(RobotRingBuffer *_this);
void  RobotRingBuffer_push(RobotRingBuffer *_this, Robot value);
void  RobotRingBuffer_flush(RobotRingBuffer *_this, uint8_t clearBuffer);
bool RobotRingBuffer_erase(RobotRingBuffer *_this, uint8_t range);
Robot* RobotRingBuffer_get(RobotRingBuffer *_this, uint8_t index);
Robot* RobotRingBuffer_findWithId(RobotRingBuffer *_this, uint8_t id);

#ifdef __cplusplus
}
#endif

#endif 