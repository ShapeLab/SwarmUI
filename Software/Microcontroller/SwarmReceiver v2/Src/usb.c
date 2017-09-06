#include "usb.h"
#include "RingBuffer.h"


/* USB handler declaration */
USBD_HandleTypeDef USBD_Device;
bool isUsbEnumerated;
/*============================================================================
Name    :   initUSB
------------------------------------------------------------------------------
Purpose :   initializes the USB Communication
Input   :   
Output  :   none
Return	:
Notes   :
============================================================================*/
bool initUSB()
{
    bool initOK = true;
    
    unsigned int initTimeOut = 0;
    isUsbEnumerated = false;

    /* Init Device Library */
    if(USBD_Init(&USBD_Device, &VCP_Desc, 0) != USBD_OK)
      initOK = false;
  
    /* Add Supported Class */
    if(USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS) != USBD_OK)
        initOK = false;

    /* Add CDC Interface Class */
    if(USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops) != USBD_OK)
        initOK = false;

    /* Start Device Process */
    if(USBD_Start(&USBD_Device) != USBD_OK)
      initOK = false;

    //todo add a while to wait for USB enumeration

    initTimeOut = HAL_GetTick();
    while(!isUsbEnumerated)
    {
        if(HAL_GetTick() - initTimeOut > 3000)
        {
          initOK = false;
          break;
        }
        HAL_Delay(1);
    }
    if (initOK && DEBUG_ENABLED()) 
        debug_printf("USB Configuration OK!\n");

    return initOK;
}

/*============================================================================
Name    :   writeUSB
------------------------------------------------------------------------------
Purpose :   sends a message through USB
Input   :  
Output  :   
Return	:  
Notes   :
============================================================================*/
bool writeUSB(uint16_t length, uint8_t* data)
{
    bool result = false;
    if(isUsbEnumerated && data)
    {
        __disable_irq();

        USBD_CDC_SetTxBuffer(&USBD_Device, data, length);
        if (USBD_CDC_TransmitPacket(&USBD_Device) == USBD_OK)
          result = true;

        __enable_irq();
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
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)USBD_Device.pClassData;

    if(isUsbEnumerated)
    {
	if(hcdc->RxState == 0 && data)
	{
	    __disable_irq();
	      memcpy_fast(data, hcdc->RxBuffer, hcdc->RxLength);
	    __enable_irq();
	    return hcdc->RxLength;
	}
    }
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
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)USBD_Device.pClassData;
    //checking that STAT_TX is set to VALID
    if(hcdc->TxState == 0)
        return true;
    else
        return false;
}
