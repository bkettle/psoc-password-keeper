/*******************************************************************************
* File Name: PRS_1_PM.c
* Version 2.40
*
* Description:
*  This file provides Sleep APIs for PRS component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "PRS_1.h"

#if ((PRS_1_TIME_MULTIPLEXING_ENABLE) && (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK))
    uint8 PRS_1_sleepState = PRS_1_NORMAL_SEQUENCE;    
#endif  /* End ((PRS_1_TIME_MULTIPLEXING_ENABLE) && 
          (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK)) */


/*******************************************************************************
* Function Name: PRS_1_SaveConfig
********************************************************************************
*
* Summary:
*  Saves seed and polynomial registers.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global Variables:
*  PRS_1_backup - modified when non-retention registers are saved.
*
*******************************************************************************/
void PRS_1_SaveConfig(void) 
{    
    #if (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK)
        /* Save dff register for time mult */
        #if (PRS_1_TIME_MULTIPLEXING_ENABLE)
            PRS_1_backup.dffStatus = PRS_1_STATUS;
            /* Clear normal dff sequence set */
            PRS_1_sleepState &= ((uint8)~PRS_1_NORMAL_SEQUENCE);
        #endif  /* End PRS_1_TIME_MULTIPLEXING_ENABLE */
        
        /* Save A0 and A1 registers */
        #if (PRS_1_PRS_SIZE <= 32u)
            PRS_1_backup.seed = PRS_1_Read();
            
        #else
            PRS_1_backup.seedUpper = PRS_1_ReadUpper();
            PRS_1_backup.seedLower = PRS_1_ReadLower();
            
        #endif     /* End (PRS_1_PRS_SIZE <= 32u) */
        
    #endif  /* End (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK) */
}


/*******************************************************************************
* Function Name: PRS_1_Sleep
********************************************************************************
*
* Summary:
*  Stops PRS computation and saves PRS configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global Variables:
*  PRS_1_backup - modified when non-retention registers are saved.
*
*******************************************************************************/
void PRS_1_Sleep(void) 
{
    /* Store PRS enable state */
    if(PRS_1_IS_PRS_ENABLE(PRS_1_CONTROL_REG))
    {
        PRS_1_backup.enableState = 1u;
        PRS_1_Stop();
    }
    else
    {
        PRS_1_backup.enableState = 0u;
    }
    
    PRS_1_SaveConfig();
}


/*******************************************************************************
* Function Name: PRS_1_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores seed and polynomial registers.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
#if (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK)
    #if (PRS_1_TIME_MULTIPLEXING_ENABLE)
        void PRS_1_RestoreConfig(void) 
        {   
            /* Restore A0 and A1 registers */
            #if (PRS_1_PRS_SIZE <= 32u)
                PRS_1_WriteSeed(PRS_1_backup.seed);
            #else
                PRS_1_WriteSeedUpper(PRS_1_backup.seedUpper);
                PRS_1_WriteSeedLower(PRS_1_backup.seedLower);
            #endif  /* End (PRS_1_PRS_SIZE <= 32u) */
            
            #if (PRS_1_RUN_MODE == PRS_1__CLOCKED)
                #if (PRS_1_PRS_SIZE <= 32u)
                    PRS_1_ResetSeedInit(PRS_1_DEFAULT_SEED);                        
                #else
                    PRS_1_ResetSeedInitUpper(PRS_1_DEFAULT_SEED_UPPER);
                    PRS_1_ResetSeedInitLower(PRS_1_DEFAULT_SEED_LOWER); 
                #endif  /* End (PRS_1_PRS_SIZE <= 32u) */ 
            #endif  /* End (PRS_1_RUN_MODE == PRS_1__CLOCKED) */
            
            /* Restore dff state for time mult: use async set/reest */
            /* Set CI, SI, SO, STATE0, STATE1 */
            PRS_1_CONTROL_REG = PRS_1_backup.dffStatus;
            
            /* Make pulse, to set trigger to defined state */
            PRS_1_EXECUTE_DFF_SET;
            
            /* Set normal dff sequence set */
            PRS_1_sleepState |= PRS_1_NORMAL_SEQUENCE;
        }
        
    #else
        void PRS_1_RestoreConfig(void) 
        {   
            /* Restore A0 and A1 registers */
            #if (PRS_1_PRS_SIZE <= 32u)
                PRS_1_WriteSeed(PRS_1_backup.seed);
            #else
                PRS_1_WriteSeedUpper(PRS_1_backup.seedUpper);
                PRS_1_WriteSeedLower(PRS_1_backup.seedLower);
            #endif  /* End (PRS_1_PRS_SIZE <= 32u) */
            
            #if (PRS_1_RUN_MODE == PRS_1__CLOCKED)
                #if (PRS_1_PRS_SIZE <= 32u)
                    PRS_1_ResetSeedInit(PRS_1_DEFAULT_SEED);                        
                #else
                    PRS_1_ResetSeedInitUpper(PRS_1_DEFAULT_SEED_UPPER);
                    PRS_1_ResetSeedInitLower(PRS_1_DEFAULT_SEED_LOWER); 
                #endif  /* End (PRS_1_PRS_SIZE <= 32u) */ 
            #endif  /* End (PRS_1_RUN_MODE == PRS_1__CLOCKED) */
        }
        
    #endif  /* End (PRS_1_TIME_MULTIPLEXING_ENABLE) */
    
#else
    void PRS_1_RestoreConfig(void) 
    {
        PRS_1_Init();
    }
    
#endif  /* End (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK) */


/*******************************************************************************
* Function Name: PRS_1_Wakeup
********************************************************************************
*
* Summary:
*  Restores PRS configuration and starts PRS computation. 
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
#if ((PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK) && (PRS_1_TIME_MULTIPLEXING_ENABLE))
    void PRS_1_Wakeup(void) 
#else
    void PRS_1_Wakeup(void) 
#endif  /* End ((PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK) && 
                (PRS_1_TIME_MULTIPLEXING_ENABLE)) */
{
    PRS_1_RestoreConfig();
    
    /* Restore PRS enable state */
    if (PRS_1_backup.enableState != 0u)
    {
        PRS_1_Enable();
    }
}

/* [] END OF FILE */
