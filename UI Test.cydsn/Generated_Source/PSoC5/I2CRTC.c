/*******************************************************************************
* File Name: I2CRTC.c
* Version 3.50
*
* Description:
*  This file provides the source code of APIs for the I2C component.
*  The actual protocol and operation code resides in the interrupt service
*  routine file.
*
*******************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "I2CRTC_PVT.h"


/**********************************
*      System variables
**********************************/

uint8 I2CRTC_initVar = 0u; /* Defines if component was initialized */

volatile uint8 I2CRTC_state;  /* Current state of I2C FSM */


/*******************************************************************************
* Function Name: I2CRTC_Init
********************************************************************************
*
* Summary:
*  Initializes I2C registers with initial values provided from customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void I2CRTC_Init(void) 
{
#if (I2CRTC_FF_IMPLEMENTED)
    /* Configure fixed function block */
    I2CRTC_CFG_REG  = I2CRTC_DEFAULT_CFG;
    I2CRTC_XCFG_REG = I2CRTC_DEFAULT_XCFG;
    I2CRTC_ADDR_REG = I2CRTC_DEFAULT_ADDR;
    I2CRTC_CLKDIV1_REG = LO8(I2CRTC_DEFAULT_DIVIDE_FACTOR);
    I2CRTC_CLKDIV2_REG = HI8(I2CRTC_DEFAULT_DIVIDE_FACTOR);

#else
    uint8 intState;

    /* Configure control and interrupt sources */
    I2CRTC_CFG_REG      = I2CRTC_DEFAULT_CFG;
    I2CRTC_INT_MASK_REG = I2CRTC_DEFAULT_INT_MASK;

    /* Enable interrupt generation in status */
    intState = CyEnterCriticalSection();
    I2CRTC_INT_ENABLE_REG |= I2CRTC_INTR_ENABLE;
    CyExitCriticalSection(intState);

    /* Configure bit counter */
    #if (I2CRTC_MODE_SLAVE_ENABLED)
        I2CRTC_PERIOD_REG = I2CRTC_DEFAULT_PERIOD;
    #endif  /* (I2CRTC_MODE_SLAVE_ENABLED) */

    /* Configure clock generator */
    #if (I2CRTC_MODE_MASTER_ENABLED)
        I2CRTC_MCLK_PRD_REG = I2CRTC_DEFAULT_MCLK_PRD;
        I2CRTC_MCLK_CMP_REG = I2CRTC_DEFAULT_MCLK_CMP;
    #endif /* (I2CRTC_MODE_MASTER_ENABLED) */
#endif /* (I2CRTC_FF_IMPLEMENTED) */

#if (I2CRTC_TIMEOUT_ENABLED)
    I2CRTC_TimeoutInit();
#endif /* (I2CRTC_TIMEOUT_ENABLED) */

    /* Configure internal interrupt */
    CyIntDisable    (I2CRTC_ISR_NUMBER);
    CyIntSetPriority(I2CRTC_ISR_NUMBER, I2CRTC_ISR_PRIORITY);
    #if (I2CRTC_INTERN_I2C_INTR_HANDLER)
        (void) CyIntSetVector(I2CRTC_ISR_NUMBER, &I2CRTC_ISR);
    #endif /* (I2CRTC_INTERN_I2C_INTR_HANDLER) */

    /* Set FSM to default state */
    I2CRTC_state = I2CRTC_SM_IDLE;

#if (I2CRTC_MODE_SLAVE_ENABLED)
    /* Clear status and buffers index */
    I2CRTC_slStatus = 0u;
    I2CRTC_slRdBufIndex = 0u;
    I2CRTC_slWrBufIndex = 0u;

    /* Configure matched address */
    I2CRTC_SlaveSetAddress(I2CRTC_DEFAULT_ADDR);
#endif /* (I2CRTC_MODE_SLAVE_ENABLED) */

#if (I2CRTC_MODE_MASTER_ENABLED)
    /* Clear status and buffers index */
    I2CRTC_mstrStatus = 0u;
    I2CRTC_mstrRdBufIndex = 0u;
    I2CRTC_mstrWrBufIndex = 0u;
#endif /* (I2CRTC_MODE_MASTER_ENABLED) */
}


/*******************************************************************************
* Function Name: I2CRTC_Enable
********************************************************************************
*
* Summary:
*  Enables I2C operations.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
*******************************************************************************/
void I2CRTC_Enable(void) 
{
#if (I2CRTC_FF_IMPLEMENTED)
    uint8 intState;

    /* Enable power to block */
    intState = CyEnterCriticalSection();
    I2CRTC_ACT_PWRMGR_REG  |= I2CRTC_ACT_PWR_EN;
    I2CRTC_STBY_PWRMGR_REG |= I2CRTC_STBY_PWR_EN;
    CyExitCriticalSection(intState);
#else
    #if (I2CRTC_MODE_SLAVE_ENABLED)
        /* Enable bit counter */
        uint8 intState = CyEnterCriticalSection();
        I2CRTC_COUNTER_AUX_CTL_REG |= I2CRTC_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    #endif /* (I2CRTC_MODE_SLAVE_ENABLED) */

    /* Enable slave or master bits */
    I2CRTC_CFG_REG |= I2CRTC_ENABLE_MS;
#endif /* (I2CRTC_FF_IMPLEMENTED) */

#if (I2CRTC_TIMEOUT_ENABLED)
    I2CRTC_TimeoutEnable();
#endif /* (I2CRTC_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: I2CRTC_Start
********************************************************************************
*
* Summary:
*  Starts the I2C hardware. Enables Active mode power template bits or clock
*  gating as appropriate. It is required to be executed before I2C bus
*  operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  This component automatically enables its interrupt.  If I2C is enabled !
*  without the interrupt enabled, it can lock up the I2C bus.
*
* Global variables:
*  I2CRTC_initVar - This variable is used to check the initial
*                             configuration, modified on the first
*                             function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void I2CRTC_Start(void) 
{
    if (0u == I2CRTC_initVar)
    {
        I2CRTC_Init();
        I2CRTC_initVar = 1u; /* Component initialized */
    }

    I2CRTC_Enable();
    I2CRTC_EnableInt();
}


/*******************************************************************************
* Function Name: I2CRTC_Stop
********************************************************************************
*
* Summary:
*  Disables I2C hardware and disables I2C interrupt. Disables Active mode power
*  template bits or clock gating as appropriate.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void I2CRTC_Stop(void) 
{
    I2CRTC_DisableInt();

#if (I2CRTC_TIMEOUT_ENABLED)
    I2CRTC_TimeoutStop();
#endif  /* End (I2CRTC_TIMEOUT_ENABLED) */

#if (I2CRTC_FF_IMPLEMENTED)
    {
        uint8 intState;
        uint16 blockResetCycles;

        /* Store registers effected by block disable */
        I2CRTC_backup.addr    = I2CRTC_ADDR_REG;
        I2CRTC_backup.clkDiv1 = I2CRTC_CLKDIV1_REG;
        I2CRTC_backup.clkDiv2 = I2CRTC_CLKDIV2_REG;

        /* Calculate number of cycles to reset block */
        blockResetCycles = ((uint16) ((uint16) I2CRTC_CLKDIV2_REG << 8u) | I2CRTC_CLKDIV1_REG) + 1u;

        /* Disable block */
        I2CRTC_CFG_REG &= (uint8) ~I2CRTC_CFG_EN_SLAVE;
        /* Wait for block reset before disable power */
        CyDelayCycles((uint32) blockResetCycles);

        /* Disable power to block */
        intState = CyEnterCriticalSection();
        I2CRTC_ACT_PWRMGR_REG  &= (uint8) ~I2CRTC_ACT_PWR_EN;
        I2CRTC_STBY_PWRMGR_REG &= (uint8) ~I2CRTC_STBY_PWR_EN;
        CyExitCriticalSection(intState);

        /* Enable block */
        I2CRTC_CFG_REG |= (uint8) I2CRTC_ENABLE_MS;

        /* Restore registers effected by block disable. Ticket ID#198004 */
        I2CRTC_ADDR_REG    = I2CRTC_backup.addr;
        I2CRTC_ADDR_REG    = I2CRTC_backup.addr;
        I2CRTC_CLKDIV1_REG = I2CRTC_backup.clkDiv1;
        I2CRTC_CLKDIV2_REG = I2CRTC_backup.clkDiv2;
    }
#else

    /* Disable slave or master bits */
    I2CRTC_CFG_REG &= (uint8) ~I2CRTC_ENABLE_MS;

#if (I2CRTC_MODE_SLAVE_ENABLED)
    {
        /* Disable bit counter */
        uint8 intState = CyEnterCriticalSection();
        I2CRTC_COUNTER_AUX_CTL_REG &= (uint8) ~I2CRTC_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    }
#endif /* (I2CRTC_MODE_SLAVE_ENABLED) */

    /* Clear interrupt source register */
    (void) I2CRTC_CSR_REG;
#endif /* (I2CRTC_FF_IMPLEMENTED) */

    /* Disable interrupt on stop (enabled by write transaction) */
    I2CRTC_DISABLE_INT_ON_STOP;
    I2CRTC_ClearPendingInt();

    /* Reset FSM to default state */
    I2CRTC_state = I2CRTC_SM_IDLE;

    /* Clear busy statuses */
#if (I2CRTC_MODE_SLAVE_ENABLED)
    I2CRTC_slStatus &= (uint8) ~(I2CRTC_SSTAT_RD_BUSY | I2CRTC_SSTAT_WR_BUSY);
#endif /* (I2CRTC_MODE_SLAVE_ENABLED) */
}


/* [] END OF FILE */
