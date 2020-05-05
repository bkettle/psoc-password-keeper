/*******************************************************************************
* File Name: I2CRTC_INT.c
* Version 3.50
*
* Description:
*  This file provides the source code of Interrupt Service Routine (ISR)
*  for the I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "I2CRTC_PVT.h"
#include "cyapicallbacks.h"


/*******************************************************************************
*  Place your includes, defines and code here.
********************************************************************************/
/* `#START I2CRTC_ISR_intc` */

/* `#END` */


/*******************************************************************************
* Function Name: I2CRTC_ISR
********************************************************************************
*
* Summary:
*  The handler for the I2C interrupt. The slave and master operations are
*  handled here.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
CY_ISR(I2CRTC_ISR)
{
#if (I2CRTC_MODE_SLAVE_ENABLED)
   uint8  tmp8;
#endif  /* (I2CRTC_MODE_SLAVE_ENABLED) */

    uint8  tmpCsr;
    
#ifdef I2CRTC_ISR_ENTRY_CALLBACK
    I2CRTC_ISR_EntryCallback();
#endif /* I2CRTC_ISR_ENTRY_CALLBACK */
    

#if(I2CRTC_TIMEOUT_FF_ENABLED)
    if(0u != I2CRTC_TimeoutGetStatus())
    {
        I2CRTC_TimeoutReset();
        I2CRTC_state = I2CRTC_SM_EXIT_IDLE;
        /* I2CRTC_CSR_REG should be cleared after reset */
    }
#endif /* (I2CRTC_TIMEOUT_FF_ENABLED) */


    tmpCsr = I2CRTC_CSR_REG;      /* Make copy as interrupts clear */

#if(I2CRTC_MODE_MULTI_MASTER_SLAVE_ENABLED)
    if(I2CRTC_CHECK_START_GEN(I2CRTC_MCSR_REG))
    {
        I2CRTC_CLEAR_START_GEN;

        /* Set transfer complete and error flags */
        I2CRTC_mstrStatus |= (I2CRTC_MSTAT_ERR_XFER |
                                        I2CRTC_GET_MSTAT_CMPLT);

        /* Slave was addressed */
        I2CRTC_state = I2CRTC_SM_SLAVE;
    }
#endif /* (I2CRTC_MODE_MULTI_MASTER_SLAVE_ENABLED) */


#if(I2CRTC_MODE_MULTI_MASTER_ENABLED)
    if(I2CRTC_CHECK_LOST_ARB(tmpCsr))
    {
        /* Set errors */
        I2CRTC_mstrStatus |= (I2CRTC_MSTAT_ERR_XFER     |
                                        I2CRTC_MSTAT_ERR_ARB_LOST |
                                        I2CRTC_GET_MSTAT_CMPLT);

        I2CRTC_DISABLE_INT_ON_STOP; /* Interrupt on Stop is enabled by write */

        #if(I2CRTC_MODE_MULTI_MASTER_SLAVE_ENABLED)
            if(I2CRTC_CHECK_ADDRESS_STS(tmpCsr))
            {
                /* Slave was addressed */
                I2CRTC_state = I2CRTC_SM_SLAVE;
            }
            else
            {
                I2CRTC_BUS_RELEASE;

                I2CRTC_state = I2CRTC_SM_EXIT_IDLE;
            }
        #else
            I2CRTC_BUS_RELEASE;

            I2CRTC_state = I2CRTC_SM_EXIT_IDLE;

        #endif /* (I2CRTC_MODE_MULTI_MASTER_SLAVE_ENABLED) */
    }
#endif /* (I2CRTC_MODE_MULTI_MASTER_ENABLED) */

    /* Check for master operation mode */
    if(I2CRTC_CHECK_SM_MASTER)
    {
    #if(I2CRTC_MODE_MASTER_ENABLED)
        if(I2CRTC_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            switch (I2CRTC_state)
            {
            case I2CRTC_SM_MSTR_WR_ADDR:  /* After address is sent, write data */
            case I2CRTC_SM_MSTR_RD_ADDR:  /* After address is sent, read data */

                tmpCsr &= ((uint8) ~I2CRTC_CSR_STOP_STATUS); /* Clear Stop bit history on address phase */

                if(I2CRTC_CHECK_ADDR_ACK(tmpCsr))
                {
                    /* Setup for transmit or receive of data */
                    if(I2CRTC_state == I2CRTC_SM_MSTR_WR_ADDR)   /* TRANSMIT data */
                    {
                        /* Check if at least one byte to transfer */
                        if(I2CRTC_mstrWrBufSize > 0u)
                        {
                            /* Load the 1st data byte */
                            I2CRTC_DATA_REG = I2CRTC_mstrWrBufPtr[0u];
                            I2CRTC_TRANSMIT_DATA;
                            I2CRTC_mstrWrBufIndex = 1u;   /* Set index to 2nd element */

                            /* Set transmit state until done */
                            I2CRTC_state = I2CRTC_SM_MSTR_WR_DATA;
                        }
                        /* End of buffer: complete writing */
                        else if(I2CRTC_CHECK_NO_STOP(I2CRTC_mstrControl))
                        {
                            /* Set write complete and master halted */
                            I2CRTC_mstrStatus |= (I2CRTC_MSTAT_XFER_HALT |
                                                            I2CRTC_MSTAT_WR_CMPLT);

                            I2CRTC_state = I2CRTC_SM_MSTR_HALT; /* Expect ReStart */
                            I2CRTC_DisableInt();
                        }
                        else
                        {
                            I2CRTC_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                            I2CRTC_GENERATE_STOP;
                        }
                    }
                    else  /* Master receive data */
                    {
                        I2CRTC_READY_TO_READ; /* Release bus to read data */

                        I2CRTC_state  = I2CRTC_SM_MSTR_RD_DATA;
                    }
                }
                /* Address is NACKed */
                else if(I2CRTC_CHECK_ADDR_NAK(tmpCsr))
                {
                    /* Set Address NAK error */
                    I2CRTC_mstrStatus |= (I2CRTC_MSTAT_ERR_XFER |
                                                    I2CRTC_MSTAT_ERR_ADDR_NAK);

                    if(I2CRTC_CHECK_NO_STOP(I2CRTC_mstrControl))
                    {
                        I2CRTC_mstrStatus |= (I2CRTC_MSTAT_XFER_HALT |
                                                        I2CRTC_GET_MSTAT_CMPLT);

                        I2CRTC_state = I2CRTC_SM_MSTR_HALT; /* Expect RESTART */
                        I2CRTC_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        I2CRTC_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                        I2CRTC_GENERATE_STOP;
                    }
                }
                else
                {
                    /* Address phase is not set for some reason: error */
                    #if(I2CRTC_TIMEOUT_ENABLED)
                        /* Exit interrupt to take chance for timeout timer to handle this case */
                        I2CRTC_DisableInt();
                        I2CRTC_ClearPendingInt();
                    #else
                        /* Block execution flow: unexpected condition */
                        CYASSERT(0u != 0u);
                    #endif /* (I2CRTC_TIMEOUT_ENABLED) */
                }
                break;

            case I2CRTC_SM_MSTR_WR_DATA:

                if(I2CRTC_CHECK_DATA_ACK(tmpCsr))
                {
                    /* Check if end of buffer */
                    if(I2CRTC_mstrWrBufIndex  < I2CRTC_mstrWrBufSize)
                    {
                        I2CRTC_DATA_REG =
                                                 I2CRTC_mstrWrBufPtr[I2CRTC_mstrWrBufIndex];
                        I2CRTC_TRANSMIT_DATA;
                        I2CRTC_mstrWrBufIndex++;
                    }
                    /* End of buffer: complete writing */
                    else if(I2CRTC_CHECK_NO_STOP(I2CRTC_mstrControl))
                    {
                        /* Set write complete and master halted */
                        I2CRTC_mstrStatus |= (I2CRTC_MSTAT_XFER_HALT |
                                                        I2CRTC_MSTAT_WR_CMPLT);

                        I2CRTC_state = I2CRTC_SM_MSTR_HALT;    /* Expect restart */
                        I2CRTC_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        I2CRTC_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                        I2CRTC_GENERATE_STOP;
                    }
                }
                /* Last byte NAKed: end writing */
                else if(I2CRTC_CHECK_NO_STOP(I2CRTC_mstrControl))
                {
                    /* Set write complete, short transfer and master halted */
                    I2CRTC_mstrStatus |= (I2CRTC_MSTAT_ERR_XFER       |
                                                    I2CRTC_MSTAT_ERR_SHORT_XFER |
                                                    I2CRTC_MSTAT_XFER_HALT      |
                                                    I2CRTC_MSTAT_WR_CMPLT);

                    I2CRTC_state = I2CRTC_SM_MSTR_HALT;    /* Expect ReStart */
                    I2CRTC_DisableInt();
                }
                else  /* Do normal Stop */
                {
                    I2CRTC_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                    I2CRTC_GENERATE_STOP;

                    /* Set short transfer and error flag */
                    I2CRTC_mstrStatus |= (I2CRTC_MSTAT_ERR_SHORT_XFER |
                                                    I2CRTC_MSTAT_ERR_XFER);
                }

                break;

            case I2CRTC_SM_MSTR_RD_DATA:

                I2CRTC_mstrRdBufPtr[I2CRTC_mstrRdBufIndex] = I2CRTC_DATA_REG;
                I2CRTC_mstrRdBufIndex++;

                /* Check if end of buffer */
                if(I2CRTC_mstrRdBufIndex < I2CRTC_mstrRdBufSize)
                {
                    I2CRTC_ACK_AND_RECEIVE;       /* ACK and receive byte */
                }
                /* End of buffer: complete reading */
                else if(I2CRTC_CHECK_NO_STOP(I2CRTC_mstrControl))
                {
                    /* Set read complete and master halted */
                    I2CRTC_mstrStatus |= (I2CRTC_MSTAT_XFER_HALT |
                                                    I2CRTC_MSTAT_RD_CMPLT);

                    I2CRTC_state = I2CRTC_SM_MSTR_HALT;    /* Expect ReStart */
                    I2CRTC_DisableInt();
                }
                else
                {
                    I2CRTC_ENABLE_INT_ON_STOP;
                    I2CRTC_NAK_AND_RECEIVE;       /* NACK and TRY to generate Stop */
                }
                break;

            default: /* This is an invalid state and should not occur */

            #if(I2CRTC_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handles this case */
                I2CRTC_DisableInt();
                I2CRTC_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CYASSERT(0u != 0u);
            #endif /* (I2CRTC_TIMEOUT_ENABLED) */

                break;
            }
        }

        /* Catches Stop: end of transaction */
        if(I2CRTC_CHECK_STOP_STS(tmpCsr))
        {
            I2CRTC_mstrStatus |= I2CRTC_GET_MSTAT_CMPLT;

            I2CRTC_DISABLE_INT_ON_STOP;
            I2CRTC_state = I2CRTC_SM_IDLE;
        }
    #endif /* (I2CRTC_MODE_MASTER_ENABLED) */
    }
    else if(I2CRTC_CHECK_SM_SLAVE)
    {
    #if(I2CRTC_MODE_SLAVE_ENABLED)

        if((I2CRTC_CHECK_STOP_STS(tmpCsr)) || /* Stop || Restart */
           (I2CRTC_CHECK_BYTE_COMPLETE(tmpCsr) && I2CRTC_CHECK_ADDRESS_STS(tmpCsr)))
        {
            /* Catch end of master write transaction: use interrupt on Stop */
            /* The Stop bit history on address phase does not have correct state */
            if(I2CRTC_SM_SL_WR_DATA == I2CRTC_state)
            {
                I2CRTC_DISABLE_INT_ON_STOP;

                I2CRTC_slStatus &= ((uint8) ~I2CRTC_SSTAT_WR_BUSY);
                I2CRTC_slStatus |= ((uint8)  I2CRTC_SSTAT_WR_CMPLT);

                I2CRTC_state = I2CRTC_SM_IDLE;
            }
        }

        if(I2CRTC_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            /* The address only issued after Start or ReStart: so check the address
               to catch these events:
                FF : sets an address phase with a byte_complete interrupt trigger.
                UDB: sets an address phase immediately after Start or ReStart. */
            if(I2CRTC_CHECK_ADDRESS_STS(tmpCsr))
            {
            /* Check for software address detection */
            #if(I2CRTC_SW_ADRR_DECODE)
                tmp8 = I2CRTC_GET_SLAVE_ADDR(I2CRTC_DATA_REG);

                if(tmp8 == I2CRTC_slAddress)   /* Check for address match */
                {
                    if(0u != (I2CRTC_DATA_REG & I2CRTC_READ_FLAG))
                    {
                        /* Place code to prepare read buffer here                  */
                        /* `#START I2CRTC_SW_PREPARE_READ_BUF_interrupt` */

                        /* `#END` */

                    #ifdef I2CRTC_SW_PREPARE_READ_BUF_CALLBACK
                        I2CRTC_SwPrepareReadBuf_Callback();
                    #endif /* I2CRTC_SW_PREPARE_READ_BUF_CALLBACK */
                        
                        /* Prepare next operation to read, get data and place in data register */
                        if(I2CRTC_slRdBufIndex < I2CRTC_slRdBufSize)
                        {
                            /* Load first data byte from array */
                            I2CRTC_DATA_REG = I2CRTC_slRdBufPtr[I2CRTC_slRdBufIndex];
                            I2CRTC_ACK_AND_TRANSMIT;
                            I2CRTC_slRdBufIndex++;

                            I2CRTC_slStatus |= I2CRTC_SSTAT_RD_BUSY;
                        }
                        else    /* Overflow: provide 0xFF on bus */
                        {
                            I2CRTC_DATA_REG = I2CRTC_OVERFLOW_RETURN;
                            I2CRTC_ACK_AND_TRANSMIT;

                            I2CRTC_slStatus  |= (I2CRTC_SSTAT_RD_BUSY |
                                                           I2CRTC_SSTAT_RD_ERR_OVFL);
                        }

                        I2CRTC_state = I2CRTC_SM_SL_RD_DATA;
                    }
                    else  /* Write transaction: receive 1st byte */
                    {
                        I2CRTC_ACK_AND_RECEIVE;
                        I2CRTC_state = I2CRTC_SM_SL_WR_DATA;

                        I2CRTC_slStatus |= I2CRTC_SSTAT_WR_BUSY;
                        I2CRTC_ENABLE_INT_ON_STOP;
                    }
                }
                else
                {
                    /*     Place code to compare for additional address here    */
                    /* `#START I2CRTC_SW_ADDR_COMPARE_interruptStart` */

                    /* `#END` */

                #ifdef I2CRTC_SW_ADDR_COMPARE_ENTRY_CALLBACK
                    I2CRTC_SwAddrCompare_EntryCallback();
                #endif /* I2CRTC_SW_ADDR_COMPARE_ENTRY_CALLBACK */
                    
                    I2CRTC_NAK_AND_RECEIVE;   /* NACK address */

                    /* Place code to end of condition for NACK generation here */
                    /* `#START I2CRTC_SW_ADDR_COMPARE_interruptEnd`  */

                    /* `#END` */

                #ifdef I2CRTC_SW_ADDR_COMPARE_EXIT_CALLBACK
                    I2CRTC_SwAddrCompare_ExitCallback();
                #endif /* I2CRTC_SW_ADDR_COMPARE_EXIT_CALLBACK */
                }

            #else /* (I2CRTC_HW_ADRR_DECODE) */

                if(0u != (I2CRTC_DATA_REG & I2CRTC_READ_FLAG))
                {
                    /* Place code to prepare read buffer here                  */
                    /* `#START I2CRTC_HW_PREPARE_READ_BUF_interrupt` */

                    /* `#END` */
                    
                #ifdef I2CRTC_HW_PREPARE_READ_BUF_CALLBACK
                    I2CRTC_HwPrepareReadBuf_Callback();
                #endif /* I2CRTC_HW_PREPARE_READ_BUF_CALLBACK */

                    /* Prepare next operation to read, get data and place in data register */
                    if(I2CRTC_slRdBufIndex < I2CRTC_slRdBufSize)
                    {
                        /* Load first data byte from array */
                        I2CRTC_DATA_REG = I2CRTC_slRdBufPtr[I2CRTC_slRdBufIndex];
                        I2CRTC_ACK_AND_TRANSMIT;
                        I2CRTC_slRdBufIndex++;

                        I2CRTC_slStatus |= I2CRTC_SSTAT_RD_BUSY;
                    }
                    else    /* Overflow: provide 0xFF on bus */
                    {
                        I2CRTC_DATA_REG = I2CRTC_OVERFLOW_RETURN;
                        I2CRTC_ACK_AND_TRANSMIT;

                        I2CRTC_slStatus  |= (I2CRTC_SSTAT_RD_BUSY |
                                                       I2CRTC_SSTAT_RD_ERR_OVFL);
                    }

                    I2CRTC_state = I2CRTC_SM_SL_RD_DATA;
                }
                else  /* Write transaction: receive 1st byte */
                {
                    I2CRTC_ACK_AND_RECEIVE;
                    I2CRTC_state = I2CRTC_SM_SL_WR_DATA;

                    I2CRTC_slStatus |= I2CRTC_SSTAT_WR_BUSY;
                    I2CRTC_ENABLE_INT_ON_STOP;
                }

            #endif /* (I2CRTC_SW_ADRR_DECODE) */
            }
            /* Data states */
            /* Data master writes into slave */
            else if(I2CRTC_state == I2CRTC_SM_SL_WR_DATA)
            {
                if(I2CRTC_slWrBufIndex < I2CRTC_slWrBufSize)
                {
                    tmp8 = I2CRTC_DATA_REG;
                    I2CRTC_ACK_AND_RECEIVE;
                    I2CRTC_slWrBufPtr[I2CRTC_slWrBufIndex] = tmp8;
                    I2CRTC_slWrBufIndex++;
                }
                else  /* of array: complete write, send NACK */
                {
                    I2CRTC_NAK_AND_RECEIVE;

                    I2CRTC_slStatus |= I2CRTC_SSTAT_WR_ERR_OVFL;
                }
            }
            /* Data master reads from slave */
            else if(I2CRTC_state == I2CRTC_SM_SL_RD_DATA)
            {
                if(I2CRTC_CHECK_DATA_ACK(tmpCsr))
                {
                    if(I2CRTC_slRdBufIndex < I2CRTC_slRdBufSize)
                    {
                         /* Get data from array */
                        I2CRTC_DATA_REG = I2CRTC_slRdBufPtr[I2CRTC_slRdBufIndex];
                        I2CRTC_TRANSMIT_DATA;
                        I2CRTC_slRdBufIndex++;
                    }
                    else   /* Overflow: provide 0xFF on bus */
                    {
                        I2CRTC_DATA_REG = I2CRTC_OVERFLOW_RETURN;
                        I2CRTC_TRANSMIT_DATA;

                        I2CRTC_slStatus |= I2CRTC_SSTAT_RD_ERR_OVFL;
                    }
                }
                else  /* Last byte was NACKed: read complete */
                {
                    /* Only NACK appears on bus */
                    I2CRTC_DATA_REG = I2CRTC_OVERFLOW_RETURN;
                    I2CRTC_NAK_AND_TRANSMIT;

                    I2CRTC_slStatus &= ((uint8) ~I2CRTC_SSTAT_RD_BUSY);
                    I2CRTC_slStatus |= ((uint8)  I2CRTC_SSTAT_RD_CMPLT);

                    I2CRTC_state = I2CRTC_SM_IDLE;
                }
            }
            else
            {
            #if(I2CRTC_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handle this case */
                I2CRTC_DisableInt();
                I2CRTC_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CYASSERT(0u != 0u);
            #endif /* (I2CRTC_TIMEOUT_ENABLED) */
            }
        }
    #endif /* (I2CRTC_MODE_SLAVE_ENABLED) */
    }
    else
    {
        /* The FSM skips master and slave processing: return to IDLE */
        I2CRTC_state = I2CRTC_SM_IDLE;
    }

#ifdef I2CRTC_ISR_EXIT_CALLBACK
    I2CRTC_ISR_ExitCallback();
#endif /* I2CRTC_ISR_EXIT_CALLBACK */    
}


#if ((I2CRTC_FF_IMPLEMENTED) && (I2CRTC_WAKEUP_ENABLED))
    /*******************************************************************************
    * Function Name: I2CRTC_WAKEUP_ISR
    ********************************************************************************
    *
    * Summary:
    *  The interrupt handler to trigger after a wakeup.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    CY_ISR(I2CRTC_WAKEUP_ISR)
    {
    #ifdef I2CRTC_WAKEUP_ISR_ENTRY_CALLBACK
        I2CRTC_WAKEUP_ISR_EntryCallback();
    #endif /* I2CRTC_WAKEUP_ISR_ENTRY_CALLBACK */
         
        /* Set flag to notify that matched address is received */
        I2CRTC_wakeupSource = 1u;

        /* SCL is stretched until the I2C_Wake() is called */

    #ifdef I2CRTC_WAKEUP_ISR_EXIT_CALLBACK
        I2CRTC_WAKEUP_ISR_ExitCallback();
    #endif /* I2CRTC_WAKEUP_ISR_EXIT_CALLBACK */
    }
#endif /* ((I2CRTC_FF_IMPLEMENTED) && (I2CRTC_WAKEUP_ENABLED)) */


/* [] END OF FILE */
