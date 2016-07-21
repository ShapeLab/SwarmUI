#include "usb.h"

/* USB handler declaration */
USBD_HandleTypeDef USBD_Device;
extern PCD_HandleTypeDef hpcd;
bool isUSBInitialized = false;

extern UART_HandleTypeDef UartHandle;
/*============================================================================
Name    :   initUSB
------------------------------------------------------------------------------
Purpose :   initializes the USB Communication
Input   :   
Output  :   none
Return	:
Notes   :
============================================================================*/
void initUSB()
{
    /* Init Device Library */
    USBD_Init(&USBD_Device, &VCP_Desc, 0);

    /* Add Supported Class */
    USBD_RegisterClass(&USBD_Device, &USBD_CDC);

    /* Add CDC Interface Class */
    USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

    /* Start Device Process */
    USBD_Start(&USBD_Device);
}

/*============================================================================
Name    :   writeUSB
------------------------------------------------------------------------------
Purpose :   sends a message through USB
Input   :   msg : pointer to the message to be sent
            payloadSize : size of the payload of the message to be sent
Output  :   
Return	:  
Notes   :
============================================================================*/
bool writeUSB(Message *msg, uint8_t payloadSize)
{
    uint32_t prim = __get_PRIMASK();
    bool result = false;
    uint8_t *usbBufferOut = (uint8_t *)malloc(payloadSize + sizeof(Header) + 3); //3 = '~' + payloadSize + '!'
    if (usbBufferOut)
    {
        if (msg != NULL && isUSBReadyToWrite())
        {
            usbBufferOut[0] = '~';
            usbBufferOut[1] = msg->header.type;
            usbBufferOut[2] = msg->header.id;
            usbBufferOut[3] = payloadSize;
            memcpy_fast((uint8_t *)&usbBufferOut[4], msg->payload, payloadSize);
            usbBufferOut[payloadSize + 4] = '!';
           
            __disable_irq();
            USBD_CDC_SetTxBuffer(&USBD_Device, usbBufferOut, (payloadSize + sizeof(Header) + 3));
            if (USBD_CDC_TransmitPacket(&USBD_Device) == USBD_OK)
                result = true;

            if (!prim)
              __enable_irq();
        }
        free(usbBufferOut);
    }
    return result;
}

/*============================================================================
Name    :   readUSB
------------------------------------------------------------------------------
Purpose :   reads data coming in the USB 
Input   :   *data : pointer to a buffer where data will be saved
Output  :   number of bytes read
Return	:  
Notes   :
============================================================================*/
uint8_t readUSB(uint8_t *data)
{
    uint32_t prim = __get_PRIMASK();
    __disable_irq();
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)USBD_Device.pClassData;
    memcpy_fast(data, hcdc->RxBuffer, hcdc->RxLength);
         
    if (!prim)
        __enable_irq();
    //clear buffer after reception
    return hcdc->RxLength;
}

/*============================================================================
Name    :   isUSBReadyToWrite
------------------------------------------------------------------------------
Purpose :   indicates whether USB is ready to be used or not
Input   :   
Output  :   true if USB is ready, no otherwise
Return	:  
Notes   :
============================================================================*/
bool isUSBReadyToWrite()
{
    //checking that STAT_TX is set to VALID
    if (hpcd.Instance->EP1R & 0x0030) 
        return true;
    else
        return false;
}
