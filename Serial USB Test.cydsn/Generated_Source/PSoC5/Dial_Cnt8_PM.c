/*******************************************************************************
* File Name: Dial_Cnt8_PM.c  
* Version 3.0
*
*  Description:
*    This file provides the power management source code to API for the
*    Counter.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Dial_Cnt8.h"

static Dial_Cnt8_backupStruct Dial_Cnt8_backup;


/*******************************************************************************
* Function Name: Dial_Cnt8_SaveConfig
********************************************************************************
* Summary:
*     Save the current user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Dial_Cnt8_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void Dial_Cnt8_SaveConfig(void) 
{
    #if (!Dial_Cnt8_UsingFixedFunction)

        Dial_Cnt8_backup.CounterUdb = Dial_Cnt8_ReadCounter();

        #if(!Dial_Cnt8_ControlRegRemoved)
            Dial_Cnt8_backup.CounterControlRegister = Dial_Cnt8_ReadControlRegister();
        #endif /* (!Dial_Cnt8_ControlRegRemoved) */

    #endif /* (!Dial_Cnt8_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Dial_Cnt8_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Dial_Cnt8_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Dial_Cnt8_RestoreConfig(void) 
{      
    #if (!Dial_Cnt8_UsingFixedFunction)

       Dial_Cnt8_WriteCounter(Dial_Cnt8_backup.CounterUdb);

        #if(!Dial_Cnt8_ControlRegRemoved)
            Dial_Cnt8_WriteControlRegister(Dial_Cnt8_backup.CounterControlRegister);
        #endif /* (!Dial_Cnt8_ControlRegRemoved) */

    #endif /* (!Dial_Cnt8_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Dial_Cnt8_Sleep
********************************************************************************
* Summary:
*     Stop and Save the user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Dial_Cnt8_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Dial_Cnt8_Sleep(void) 
{
    #if(!Dial_Cnt8_ControlRegRemoved)
        /* Save Counter's enable state */
        if(Dial_Cnt8_CTRL_ENABLE == (Dial_Cnt8_CONTROL & Dial_Cnt8_CTRL_ENABLE))
        {
            /* Counter is enabled */
            Dial_Cnt8_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            Dial_Cnt8_backup.CounterEnableState = 0u;
        }
    #else
        Dial_Cnt8_backup.CounterEnableState = 1u;
        if(Dial_Cnt8_backup.CounterEnableState != 0u)
        {
            Dial_Cnt8_backup.CounterEnableState = 0u;
        }
    #endif /* (!Dial_Cnt8_ControlRegRemoved) */
    
    Dial_Cnt8_Stop();
    Dial_Cnt8_SaveConfig();
}


/*******************************************************************************
* Function Name: Dial_Cnt8_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Dial_Cnt8_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Dial_Cnt8_Wakeup(void) 
{
    Dial_Cnt8_RestoreConfig();
    #if(!Dial_Cnt8_ControlRegRemoved)
        if(Dial_Cnt8_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            Dial_Cnt8_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!Dial_Cnt8_ControlRegRemoved) */
    
}


/* [] END OF FILE */
