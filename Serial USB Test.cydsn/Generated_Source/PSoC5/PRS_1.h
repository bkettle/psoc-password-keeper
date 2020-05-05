/*******************************************************************************
* File Name: PRS_1.h
* Version 2.40
*
* Description:
*  This file provides constants and parameter values for the PRS component.
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

#if !defined(CY_PRS_PRS_1_H)
#define CY_PRS_PRS_1_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h"


/***************************************
*   Conditional Compilation Parameters
****************************************/

#define PRS_1_PRS_SIZE                   (16u)
#define PRS_1_RUN_MODE                   (1u)
#define PRS_1_TIME_MULTIPLEXING_ENABLE   (0u)
#define PRS_1_WAKEUP_BEHAVIOUR           (1u)

#define PRS_1__CLOCKED 0
#define PRS_1__APISINGLESTEP 1


#define PRS_1__RESUMEWORK 1
#define PRS_1__STARTAFRESH 0



/***************************************
*       Type defines
***************************************/

/* Structure to save registers before go to sleep */
typedef struct
{
    uint8 enableState;
    
    #if (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK)
        /* Save dff register for time mult */
        #if (PRS_1_TIME_MULTIPLEXING_ENABLE)
            uint8 dffStatus;
        #endif  /* End PRS_1_TIME_MULTIPLEXING_ENABLE */
    
        /* Save A0 and A1 registers are none-retention */
        #if(PRS_1_PRS_SIZE <= 32u)
            uint16 seed;
            
        #else
            uint32 seedUpper;
            uint32 seedLower;
            
        #endif  /* End (PRS_1_PRS_SIZE <= 32u) */ 
        
    #endif  /* End (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK) */
    
} PRS_1_BACKUP_STRUCT;

extern uint8 PRS_1_initVar;

extern PRS_1_BACKUP_STRUCT PRS_1_backup;

#if ((PRS_1_TIME_MULTIPLEXING_ENABLE) && (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK))
    extern uint8 PRS_1_sleepState;
#endif  /* End ((PRS_1_TIME_MULTIPLEXING_ENABLE) && 
          (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK)) */

                                     
/***************************************
*        Function Prototypes
****************************************/

void PRS_1_Init(void) ;
void PRS_1_Enable(void) ;
void PRS_1_Start(void) ;
void PRS_1_Stop(void) ;
void PRS_1_SaveConfig(void) ;
void PRS_1_Sleep(void) ;


#if ((PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK) && (PRS_1_TIME_MULTIPLEXING_ENABLE))
    void PRS_1_RestoreConfig(void) ;
    void PRS_1_Wakeup(void) ;
#else
    void PRS_1_RestoreConfig(void) ;
    void PRS_1_Wakeup(void) ;
#endif  /* End ((PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK) && 
                (PRS_1_TIME_MULTIPLEXING_ENABLE)) */

#if (PRS_1_RUN_MODE == PRS_1__APISINGLESTEP)
    void PRS_1_Step(void) ;
#endif  /* End (PRS_1_RUN_MODE == PRS_1__APISINGLESTEP) */

#if (PRS_1_PRS_SIZE <= 32u)    /* 8-32 bits PRS */
    uint16 PRS_1_Read(void) ;
    void PRS_1_WriteSeed(uint16 seed)  ;
    uint16 PRS_1_ReadPolynomial(void) 
                                ;
    void PRS_1_WritePolynomial(uint16 polynomial) 
                                          ;
    
#else                                    /* 33-64 bits PRS */
    uint32 PRS_1_ReadUpper(void) ;
    uint32 PRS_1_ReadLower(void) ;
    void PRS_1_WriteSeedUpper(uint32 seed) ;
    void PRS_1_WriteSeedLower(uint32 seed) ;
    uint32 PRS_1_ReadPolynomialUpper(void) ;
    uint32 PRS_1_ReadPolynomialLower(void) ;
    void PRS_1_WritePolynomialUpper(uint32 polynomial) 
                                                ;
    void PRS_1_WritePolynomialLower(uint32 polynomial) 
                                                ;
    
#endif  /* End (PRS_1_PRS_SIZE <= 32u) */

#if (PRS_1_RUN_MODE == PRS_1__CLOCKED)
    #if (PRS_1_PRS_SIZE <= 32u) /* 8-32 bits PRS */
        void PRS_1_ResetSeedInit(uint16 seed)  
                                            ;
    #else
        void PRS_1_ResetSeedInitUpper(uint32 seed) ;
        void PRS_1_ResetSeedInitLower(uint32 seed) ;
    #endif  /* End (PRS_1_PRS_SIZE <= 32u) */
#endif  /* End (PRS_1_RUN_MODE == PRS_1__CLOCKED) */


/***************************************
*    Initial Parameter Constants
***************************************/

#if (PRS_1_PRS_SIZE <= 32u)
    #define PRS_1_DEFAULT_POLYNOM            (0xB400u)
    #define PRS_1_DEFAULT_SEED               (0xFFFFu)
    
#else
    #define PRS_1_DEFAULT_POLYNOM_UPPER      (0x0u)
    #define PRS_1_DEFAULT_POLYNOM_LOWER      (0xB400u)
    #define PRS_1_DEFAULT_SEED_UPPER         (0x0u)
    #define PRS_1_DEFAULT_SEED_LOWER         (0xFFFFu)
    
#endif  /* End (PRS_1_PRS_SIZE <= 32u) */


/***************************************
*           API Constants
***************************************/

#define PRS_1_MASK                           (0xFFFFu)


/***************************************
*             Registers
***************************************/


#if (PRS_1_RUN_MODE == PRS_1__CLOCKED)
    #define PRS_1_EXECUTE_DFF_RESET  \
        (PRS_1_CONTROL_REG |= (PRS_1_INIT_STATE | PRS_1_CTRL_RESET_COMMON))
    
    #define PRS_1_EXECUTE_DFF_SET    \
        (PRS_1_CONTROL_REG |= PRS_1_CTRL_RESET_COMMON)

#else
    #define PRS_1_EXECUTE_DFF_RESET  \
        do { \
            PRS_1_CONTROL_REG |= (PRS_1_INIT_STATE | PRS_1_CTRL_RESET_COMMON | \
            PRS_1_CTRL_RISING_EDGE );  \
            PRS_1_CONTROL_REG &= ((uint8)~(PRS_1_CTRL_RESET_COMMON | \
            PRS_1_CTRL_RISING_EDGE));    \
        } while (0)
    
    #define PRS_1_EXECUTE_DFF_SET    \
        do { \
            PRS_1_CONTROL_REG |= (PRS_1_CTRL_RESET_COMMON | \
            PRS_1_CTRL_RISING_EDGE);\
            PRS_1_CONTROL_REG &= ((uint8)~(PRS_1_CTRL_RESET_COMMON | \
            PRS_1_CTRL_RISING_EDGE));    \
        } while (0)

    #define PRS_1_EXECUTE_STEP       \
        do { \
            PRS_1_CONTROL_REG |= PRS_1_CTRL_RISING_EDGE; \
            PRS_1_CONTROL_REG &= ((uint8)~PRS_1_CTRL_RISING_EDGE);    \
        } while (0)
    
#endif  /* End (PRS_1_RUN_MODE == PRS_1__CLOCKED) */

#if (PRS_1_TIME_MULTIPLEXING_ENABLE)
    
    #if (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK)
        #define PRS_1_STATUS                     (*(reg8 *) PRS_1_Sts_StsReg__STATUS_REG )
        #define PRS_1_STATUS_PTR                 ( (reg8 *) PRS_1_Sts_StsReg__STATUS_REG )
    #endif  /* End (PRS_1_WAKEUP_BEHAVIOUR == PRS_1__RESUMEWORK) */
    
    #if (PRS_1_PRS_SIZE <= 16u)      /* 16 bits PRS */
        /* Polynomial */
        #define PRS_1_POLYNOM_A__D1_REG          (*(reg8 *) PRS_1_b0_PRSdp_a__D1_REG )
        #define PRS_1_POLYNOM_A__D1_PTR      	( (reg8 *) PRS_1_b0_PRSdp_a__D1_REG )
        #define PRS_1_POLYNOM_A__D0_REG          (*(reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        #define PRS_1_POLYNOM_A__D0_PTR      	( (reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        /* Seed */
        #define PRS_1_SEED_A__A1_REG             (*(reg8 *) PRS_1_b0_PRSdp_a__A1_REG )
        #define PRS_1_SEED_A__A1_PTR         	( (reg8 *) PRS_1_b0_PRSdp_a__A1_REG )
        #define PRS_1_SEED_A__A0_REG             (*(reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        #define PRS_1_SEED_A__A0_PTR         	( (reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        /* Seed COPY */
        #define PRS_1_SEED_COPY_A__A1_REG        (*(reg8 *) PRS_1_b0_PRSdp_a__F1_REG )
        #define PRS_1_SEED_COPY_A__A1_PTR    	( (reg8 *) PRS_1_b0_PRSdp_a__F1_REG )
        #define PRS_1_SEED_COPY_A__A0_REG        (*(reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        #define PRS_1_SEED_COPY_A__A0_PTR    	( (reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        
    #elif (PRS_1_PRS_SIZE <= 24u)      /* 24 bits PRS */
        /* Polynomial */
        #define PRS_1_POLYNOM_B__D1_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        #define PRS_1_POLYNOM_B__D1_PTR      	( (reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        #define PRS_1_POLYNOM_B__D0_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_B__D0_PTR      	( (reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_A__D0_REG          (*(reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        #define PRS_1_POLYNOM_A__D0_PTR     	 	( (reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        /* Seed */
        #define PRS_1_SEED_B__A1_REG             (*(reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        #define PRS_1_SEED_B__A1_PTR         	( (reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        #define PRS_1_SEED_B__A0_REG             (*(reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_B__A0_PTR     	    ( (reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_A__A0_REG             (*(reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        #define PRS_1_SEED_A__A0_PTR 	        ( (reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        /* Seed COPY */
        #define PRS_1_SEED_COPY_B__A1_REG        (*(reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        #define PRS_1_SEED_COPY_B__A1_PTR 	    ( (reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        #define PRS_1_SEED_COPY_B__A0_REG        (*(reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_COPY_B__A0_PTR	    ( (reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_COPY_A__A0_REG        (*(reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        #define PRS_1_SEED_COPY_A__A0_PTR    	( (reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        
    #elif (PRS_1_PRS_SIZE <= 32u)      /* 32 bits PRS */
        /* Polynomial */
        #define PRS_1_POLYNOM_B__D1_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        #define PRS_1_POLYNOM_B__D1_PTR    		( (reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        #define PRS_1_POLYNOM_A__D1_REG          (*(reg8 *) PRS_1_b0_PRSdp_a__D1_REG )
        #define PRS_1_POLYNOM_A__D1_PTR     		( (reg8 *) PRS_1_b0_PRSdp_a__D1_REG )
        #define PRS_1_POLYNOM_B__D0_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_B__D0_PTR		    ( (reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_A__D0_REG          (*(reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        #define PRS_1_POLYNOM_A__D0_PTR  	    ( (reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        /* Seed */
        #define PRS_1_SEED_B__A1_REG             (*(reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        #define PRS_1_SEED_B__A1_PTR    		    ( (reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        #define PRS_1_SEED_A__A1_REG         	(*(reg8 *) PRS_1_b0_PRSdp_a__A1_REG )
        #define PRS_1_SEED_A__A1_PTR      	    ( (reg8 *) PRS_1_b0_PRSdp_a__A1_REG )
        #define PRS_1_SEED_B__A0_REG             (*(reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_B__A0_PTR     	    ( (reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_A__A0_REG             (*(reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        #define PRS_1_SEED_A__A0_PTR     	    ( (reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        /* Seed COPY */
        #define PRS_1_SEED_COPY_B__A1_REG        (*(reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        #define PRS_1_SEED_COPY_B__A1_PTR  		( (reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        #define PRS_1_SEED_COPY_A__A1_REG        (*(reg8 *) PRS_1_b0_PRSdp_a__F1_REG )
        #define PRS_1_SEED_COPY_A__A1_PTR   	 	( (reg8 *) PRS_1_b0_PRSdp_a__F1_REG )
        #define PRS_1_SEED_COPY_B__A0_REG        (*(reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_COPY_B__A0_PTR  	  	( (reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_COPY_A__A0_REG        (*(reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        #define PRS_1_SEED_COPY_A__A0_PTR   	 	( (reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        
    #elif (PRS_1_PRS_SIZE <= 40u)      /* 40 bits PRS */
        /* Polynomial Upper */
        #define PRS_1_POLYNOM_UPPER_C__D1_REG            (*(reg8 *) PRS_1_b2_PRSdp_c__D1_REG )
        #define PRS_1_POLYNOM_UPPER_C__D1_PTR        	( (reg8 *) PRS_1_b2_PRSdp_c__D1_REG )
        /* Polynomial Lower */
        #define PRS_1_POLYNOM_LOWER_B__D1_REG            (*(reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        #define PRS_1_POLYNOM_LOWER_B__D1_PTR        	( (reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        #define PRS_1_POLYNOM_LOWER_C__D0_REG            (*(reg8 *) PRS_1_b2_PRSdp_c__D0_REG )
        #define PRS_1_POLYNOM_LOWER_C__D0_PTR        	( (reg8 *) PRS_1_b2_PRSdp_c__D0_REG )
        #define PRS_1_POLYNOM_LOWER_B__D0_REG            (*(reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_LOWER_B__D0_PTR        	( (reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_LOWER_A__D0_REG            (*(reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        #define PRS_1_POLYNOM_LOWER_A__D0_PTR        	( (reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        /* Seed Upper */
        #define PRS_1_SEED_UPPER_C__A1_REG               (*(reg8 *) PRS_1_b2_PRSdp_c__A1_REG )
        #define PRS_1_SEED_UPPER_C__A1_PTR           	( (reg8 *) PRS_1_b2_PRSdp_c__A1_REG )
        /* Seed Lower */
        #define PRS_1_SEED_LOWER_B__A1_REG               (*(reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        #define PRS_1_SEED_LOWER_B__A1_PTR           	( (reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        #define PRS_1_SEED_LOWER_C__A0_REG               (*(reg8 *) PRS_1_b2_PRSdp_c__A0_REG )
        #define PRS_1_SEED_LOWER_C__A0_PTR           	( (reg8 *) PRS_1_b2_PRSdp_c__A0_REG )
        #define PRS_1_SEED_LOWER_B__A0_REG               (*(reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_LOWER_B__A0_PTR           	( (reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_LOWER_A__A0_REG               (*(reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        #define PRS_1_SEED_LOWER_A__A0_PTR           	( (reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        /* Seed COPY Upper */
        #define PRS_1_SEED_UPPER_COPY_C__A1_REG          (*(reg8 *) PRS_1_b2_PRSdp_c__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_C__A1_PTR      	( (reg8 *) PRS_1_b2_PRSdp_c__F1_REG )
        /* Seed COPY Lower */
        #define PRS_1_SEED_LOWER_COPY_B__A1_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        #define PRS_1_SEED_LOWER_COPY_B__A1_PTR      	( (reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        #define PRS_1_SEED_LOWER_COPY_C__A0_REG          (*(reg8 *) PRS_1_b2_PRSdp_c__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_C__A0_PTR      	( (reg8 *) PRS_1_b2_PRSdp_c__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_B__A0_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_B__A0_PTR      	( (reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_A__A0_REG          (*(reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_A__A0_PTR      	( (reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        
    #elif (PRS_1_PRS_SIZE <= 48u)      /* 48 bits PRS */
        /* Polynomial Upper */
        #define PRS_1_POLYNOM_UPPER_C__D1_REG            (*(reg8 *) PRS_1_b2_PRSdp_c__D1_REG )
        #define PRS_1_POLYNOM_UPPER_C__D1_PTR        	( (reg8 *) PRS_1_b2_PRSdp_c__D1_REG )
        #define PRS_1_POLYNOM_UPPER_B__D1_REG            (*(reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        #define PRS_1_POLYNOM_UPPER_B__D1_PTR        	( (reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        /* Polynomial Lower */
        #define PRS_1_POLYNOM_LOWER_A__D1_REG            (*(reg8 *) PRS_1_b0_PRSdp_a__D1_REG )
        #define PRS_1_POLYNOM_LOWER_A__D1_PTR        	( (reg8 *) PRS_1_b0_PRSdp_a__D1_REG )
        #define PRS_1_POLYNOM_LOWER_C__D0_REG            (*(reg8 *) PRS_1_b2_PRSdp_c__D0_REG )
        #define PRS_1_POLYNOM_LOWER_C__D0_PTR        	( (reg8 *) PRS_1_b2_PRSdp_c__D0_REG )
        #define PRS_1_POLYNOM_LOWER_B__D0_REG            (*(reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_LOWER_B__D0_PTR        	( (reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_LOWER_A__D0_REG            (*(reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        #define PRS_1_POLYNOM_LOWER_A__D0_PTR        	( (reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        /* Seed Upper */
        #define PRS_1_SEED_UPPER_C__A1_REG               (*(reg8 *) PRS_1_b2_PRSdp_c__A1_REG )
        #define PRS_1_SEED_UPPER_C__A1_PTR           	( (reg8 *) PRS_1_b2_PRSdp_c__A1_REG )
        #define PRS_1_SEED_UPPER_B__A1_REG               (*(reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        #define PRS_1_SEED_UPPER_B__A1_PTR           	( (reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        /* Seed Lower */
        #define PRS_1_SEED_LOWER_A__A1_REG               (*(reg8 *) PRS_1_b0_PRSdp_a__A1_REG )
        #define PRS_1_SEED_LOWER_A__A1_PTR           	( (reg8 *) PRS_1_b0_PRSdp_a__A1_REG )
        #define PRS_1_SEED_LOWER_C__A0_REG               (*(reg8 *) PRS_1_b2_PRSdp_c__A0_REG )
        #define PRS_1_SEED_LOWER_C__A0_PTR           	( (reg8 *) PRS_1_b2_PRSdp_c__A0_REG )
        #define PRS_1_SEED_LOWER_B__A0_REG               (*(reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_LOWER_B__A0_PTR           	( (reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_LOWER_A__A0_REG               (*(reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        #define PRS_1_SEED_LOWER_A__A0_PTR           	( (reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        /* Seed COPY Upper */
        #define PRS_1_SEED_UPPER_COPY_C__A1_REG          (*(reg8 *) PRS_1_b2_PRSdp_c__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_C__A1_PTR      	( (reg8 *) PRS_1_b2_PRSdp_c__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_B__A1_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_B__A1_PTR      	( (reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        /* Seed COPY Lower */
        #define PRS_1_SEED_LOWER_COPY_A__A1_REG          (*(reg8 *) PRS_1_b0_PRSdp_a__F1_REG )
        #define PRS_1_SEED_LOWER_COPY_A__A1_PTR      	( (reg8 *) PRS_1_b0_PRSdp_a__F1_REG )
        #define PRS_1_SEED_LOWER_COPY_C__A0_REG          (*(reg8 *) PRS_1_b2_PRSdp_c__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_C__A0_PTR      	( (reg8 *) PRS_1_b2_PRSdp_c__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_B__A0_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_B__A0_PTR      	( (reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_A__A0_REG          (*(reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_A__A0_PTR      	( (reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        
    #elif (PRS_1_PRS_SIZE <= 56u)      /* 56 bits PRS */
        /* Polynom Upper */
        #define PRS_1_POLYNOM_UPPER_D__D1_REG            (*(reg8 *) PRS_1_b3_PRSdp_d__D1_REG )
        #define PRS_1_POLYNOM_UPPER_D__D1_PTR        	( (reg8 *) PRS_1_b3_PRSdp_d__D1_REG )
        #define PRS_1_POLYNOM_UPPER_C__D1_REG            (*(reg8 *) PRS_1_b2_PRSdp_c__D1_REG )
        #define PRS_1_POLYNOM_UPPER_C__D1_PTR        	( (reg8 *) PRS_1_b2_PRSdp_c__D1_REG )
        #define PRS_1_POLYNOM_UPPER_B__D1_REG            (*(reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        #define PRS_1_POLYNOM_UPPER_B__D1_PTR        	( (reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        /* Polynom Lower */
        #define PRS_1_POLYNOM_LOWER_D__D0_REG            (*(reg8 *) PRS_1_b3_PRSdp_d__D0_REG )
        #define PRS_1_POLYNOM_LOWER_D__D0_PTR        	( (reg8 *) PRS_1_b3_PRSdp_d__D0_REG )
        #define PRS_1_POLYNOM_LOWER_C__D0_REG            (*(reg8 *) PRS_1_b2_PRSdp_c__D0_REG )
        #define PRS_1_POLYNOM_LOWER_C__D0_PTR        	( (reg8 *) PRS_1_b2_PRSdp_c__D0_REG )
        #define PRS_1_POLYNOM_LOWER_B__D0_REG            (*(reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_LOWER_B__D0_PTR        	( (reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_LOWER_A__D0_REG            (*(reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        #define PRS_1_POLYNOM_LOWER_A__D0_PTR        	( (reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        /* Seed Upper */
        #define PRS_1_SEED_UPPER_D__A1_REG               (*(reg8 *) PRS_1_b3_PRSdp_d__A1_REG )
        #define PRS_1_SEED_UPPER_D__A1_PTR           	( (reg8 *) PRS_1_b3_PRSdp_d__A1_REG )
        #define PRS_1_SEED_UPPER_C__A1_REG               (*(reg8 *) PRS_1_b2_PRSdp_c__A1_REG )
        #define PRS_1_SEED_UPPER_C__A1_PTR           	( (reg8 *) PRS_1_b2_PRSdp_c__A1_REG )
        #define PRS_1_SEED_UPPER_B__A1_REG               (*(reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        #define PRS_1_SEED_UPPER_B__A1_PTR           	( (reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        /* Seed Lower */
        #define PRS_1_SEED_LOWER_D__A0_REG               (*(reg8 *) PRS_1_b3_PRSdp_d__A0_REG )
        #define PRS_1_SEED_LOWER_D__A0_PTR           	( (reg8 *) PRS_1_b3_PRSdp_d__A0_REG )
        #define PRS_1_SEED_LOWER_C__A0_REG               (*(reg8 *) PRS_1_b2_PRSdp_c__A0_REG )
        #define PRS_1_SEED_LOWER_C__A0_PTR           	( (reg8 *) PRS_1_b2_PRSdp_c__A0_REG )
        #define PRS_1_SEED_LOWER_B__A0_REG               (*(reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_LOWER_B__A0_PTR           	( (reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_LOWER_A__A0_REG               (*(reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        #define PRS_1_SEED_LOWER_A__A0_PTR           	( (reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        /* COPY Seed Upper */
        #define PRS_1_SEED_UPPER_COPY_D__A1_REG          (*(reg8 *) PRS_1_b3_PRSdp_d__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_D__A1_PTR      	( (reg8 *) PRS_1_b3_PRSdp_d__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_C__A1_REG          (*(reg8 *) PRS_1_b2_PRSdp_c__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_C__A1_PTR      	( (reg8 *) PRS_1_b2_PRSdp_c__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_B__A1_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_B__A1_PTR      	( (reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        /* COPY Seed Lower */
        #define PRS_1_SEED_LOWER_COPY_D__A0_REG          (*(reg8 *) PRS_1_b3_PRSdp_d__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_D__A0_PTR      	( (reg8 *) PRS_1_b3_PRSdp_d__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_C__A0_REG          (*(reg8 *) PRS_1_b2_PRSdp_c__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_C__A0_PTR      	( (reg8 *) PRS_1_b2_PRSdp_c__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_B__A0_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_B__A0_PTR      	( (reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_A__A0_REG          (*(reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_A__A0_PTR      	( (reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        
    #else                                        /* 64 bits PRS */
        /* Polynom Upper */
        #define PRS_1_POLYNOM_UPPER_D__D1_REG            (*(reg8 *) PRS_1_b3_PRSdp_d__D1_REG )
        #define PRS_1_POLYNOM_UPPER_D__D1_PTR        	( (reg8 *) PRS_1_b3_PRSdp_d__D1_REG )
        #define PRS_1_POLYNOM_UPPER_C__D1_REG            (*(reg8 *) PRS_1_b2_PRSdp_c__D1_REG )
        #define PRS_1_POLYNOM_UPPER_C__D1_PTR        	( (reg8 *) PRS_1_b2_PRSdp_c__D1_REG )
        #define PRS_1_POLYNOM_UPPER_B__D1_REG            (*(reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        #define PRS_1_POLYNOM_UPPER_B__D1_PTR        	( (reg8 *) PRS_1_b1_PRSdp_b__D1_REG )
        #define PRS_1_POLYNOM_UPPER_A__D1_REG            (*(reg8 *) PRS_1_b0_PRSdp_a__D1_REG )
        #define PRS_1_POLYNOM_UPPER_A__D1_PTR        	( (reg8 *) PRS_1_b0_PRSdp_a__D1_REG )
        /* Polynom Lower */
        #define PRS_1_POLYNOM_LOWER_D__D0_REG            (*(reg8 *) PRS_1_b3_PRSdp_d__D0_REG )
        #define PRS_1_POLYNOM_LOWER_D__D0_PTR        	( (reg8 *) PRS_1_b3_PRSdp_d__D0_REG )
        #define PRS_1_POLYNOM_LOWER_C__D0_REG            (*(reg8 *) PRS_1_b2_PRSdp_c__D0_REG )
        #define PRS_1_POLYNOM_LOWER_C__D0_PTR        	( (reg8 *) PRS_1_b2_PRSdp_c__D0_REG )
        #define PRS_1_POLYNOM_LOWER_B__D0_REG            (*(reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_LOWER_B__D0_PTR        	( (reg8 *) PRS_1_b1_PRSdp_b__D0_REG )
        #define PRS_1_POLYNOM_LOWER_A__D0_REG            (*(reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        #define PRS_1_POLYNOM_LOWER_A__D0_PTR        	( (reg8 *) PRS_1_b0_PRSdp_a__D0_REG )
        /* Seed Upper */
        #define PRS_1_SEED_UPPER_D__A1_REG               (*(reg8 *) PRS_1_b3_PRSdp_d__A1_REG )
        #define PRS_1_SEED_UPPER_D__A1_PTR           	( (reg8 *) PRS_1_b3_PRSdp_d__A1_REG )
        #define PRS_1_SEED_UPPER_C__A1_REG               (*(reg8 *) PRS_1_b2_PRSdp_c__A1_REG )
        #define PRS_1_SEED_UPPER_C__A1_PTR           	( (reg8 *) PRS_1_b2_PRSdp_c__A1_REG )
        #define PRS_1_SEED_UPPER_B__A1_REG               (*(reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        #define PRS_1_SEED_UPPER_B__A1_PTR           	( (reg8 *) PRS_1_b1_PRSdp_b__A1_REG )
        #define PRS_1_SEED_UPPER_A__A1_REG               (*(reg8 *) PRS_1_b0_PRSdp_a__A1_REG )
        #define PRS_1_SEED_UPPER_A__A1_PTR           	( (reg8 *) PRS_1_b0_PRSdp_a__A1_REG )
        /* Seed Lower */
        #define PRS_1_SEED_LOWER_D__A0_REG               (*(reg8 *) PRS_1_b3_PRSdp_d__A0_REG )
        #define PRS_1_SEED_LOWER_D__A0_PTR           	( (reg8 *) PRS_1_b3_PRSdp_d__A0_REG )
        #define PRS_1_SEED_LOWER_C__A0_REG               (*(reg8 *) PRS_1_b2_PRSdp_c__A0_REG )
        #define PRS_1_SEED_LOWER_C__A0_PTR           	( (reg8 *) PRS_1_b2_PRSdp_c__A0_REG )
        #define PRS_1_SEED_LOWER_B__A0_REG               (*(reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_LOWER_B__A0_PTR           	( (reg8 *) PRS_1_b1_PRSdp_b__A0_REG )
        #define PRS_1_SEED_LOWER_A__A0_REG               (*(reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        #define PRS_1_SEED_LOWER_A__A0_PTR           	( (reg8 *) PRS_1_b0_PRSdp_a__A0_REG )
        /* COPY Seed Upper */
        #define PRS_1_SEED_UPPER_COPY_D__A1_REG          (*(reg8 *) PRS_1_b3_PRSdp_d__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_D__A1_PTR      	( (reg8 *) PRS_1_b3_PRSdp_d__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_C__A1_REG          (*(reg8 *) PRS_1_b2_PRSdp_c__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_C__A1_PTR      	( (reg8 *) PRS_1_b2_PRSdp_c__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_B__A1_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_B__A1_PTR      	( (reg8 *) PRS_1_b1_PRSdp_b__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_A__A1_REG          (*(reg8 *) PRS_1_b0_PRSdp_a__F1_REG )
        #define PRS_1_SEED_UPPER_COPY_A__A1_PTR      	( (reg8 *) PRS_1_b0_PRSdp_a__F1_REG )
        /* COPY Seed Lower */
        #define PRS_1_SEED_LOWER_COPY_D__A0_REG          (*(reg8 *) PRS_1_b3_PRSdp_d__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_D__A0_PTR      	( (reg8 *) PRS_1_b3_PRSdp_d__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_C__A0_REG          (*(reg8 *) PRS_1_b2_PRSdp_c__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_C__A0_PTR    		( (reg8 *) PRS_1_b2_PRSdp_c__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_B__A0_REG          (*(reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_B__A0_PTR      	( (reg8 *) PRS_1_b1_PRSdp_b__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_A__A0_REG          (*(reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
        #define PRS_1_SEED_LOWER_COPY_A__A0_PTR      	( (reg8 *) PRS_1_b0_PRSdp_a__F0_REG )
    #endif  /* End (PRS_1_PRS_SIZE <= 8u) */

#else
    #if (CY_PSOC3 || CY_PSOC5)
        #if (PRS_1_PRS_SIZE <= 32u)         /* 8-32 bits PRS */
            /* Polynomial */
            #define PRS_1_POLYNOM_PTR      ( (reg16 *)\
                                                PRS_1_sC16_PRSdp_u0__D0_REG )
            /* Seed */
            #define PRS_1_SEED_PTR         ( (reg16 *)\
                                                PRS_1_sC16_PRSdp_u0__A0_REG )
            /* Seed COPY */
            #define PRS_1_SEED_COPY_PTR    ( (reg16 *)\
                                                PRS_1_sC16_PRSdp_u0__F0_REG )
            
        #endif  /* End (PRS_1_PRS_SIZE <= 32u) */
    #else /* PSoC4 */
        #if (PRS_1_PRS_SIZE <= 8u)         /* 8 bits PRS */
            /* Polynomial */
            #define PRS_1_POLYNOM_PTR      ( (reg16 *)\
                                                PRS_1_sC16_PRSdp_u0__D0_REG )
            /* Seed */
            #define PRS_1_SEED_PTR         ( (reg16 *)\
                                                PRS_1_sC16_PRSdp_u0__A0_REG )
            /* Seed COPY */
            #define PRS_1_SEED_COPY_PTR    ( (reg16 *)\
                                                PRS_1_sC16_PRSdp_u0__F0_REG )
            
        #elif (PRS_1_PRS_SIZE <= 16u)         /* 16 bits PRS */
            /* Polynomial */
            #define PRS_1_POLYNOM_PTR      ( (reg16 *)\
                                            PRS_1_sC16_PRSdp_u0__16BIT_D0_REG )
            /* Seed */
            #define PRS_1_SEED_PTR         ( (reg16 *)\
                                            PRS_1_sC16_PRSdp_u0__16BIT_A0_REG )
            /* Seed COPY */
            #define PRS_1_SEED_COPY_PTR    ( (reg16 *)\
                                            PRS_1_sC16_PRSdp_u0__16BIT_F0_REG )

        #elif (PRS_1_PRS_SIZE <= 24u)         /* 24 bits PRS */
            /* Polynomial */
            #define PRS_1_POLYNOM_PTR      ( (reg16 *)\
                                                PRS_1_sC16_PRSdp_u0__D0_REG )
            /* Seed */
            #define PRS_1_SEED_PTR         ( (reg16 *)\
                                                PRS_1_sC16_PRSdp_u0__A0_REG )
            /* Seed COPY */
            #define PRS_1_SEED_COPY_PTR    ( (reg16 *)\
                                                PRS_1_sC16_PRSdp_u0__F0_REG )
         
         #else                                          /* 32 bits PRS */
            /* Polynomial */
            #define PRS_1_POLYNOM_PTR      ( (reg16 *)\
                                            PRS_1_sC16_PRSdp_u0__32BIT_D0_REG )
            /* Seed */
            #define PRS_1_SEED_PTR         ( (reg16 *)\
                                            PRS_1_sC16_PRSdp_u0__32BIT_A0_REG )
            /* Seed COPY */
            #define PRS_1_SEED_COPY_PTR    ( (reg16 *)\
                                            PRS_1_sC16_PRSdp_u0__32BIT_F0_REG )
         
        #endif  /* End (PRS_1_PRS_SIZE <= 32u) */

    #endif  /* End (CY_PSOC3 || CY_PSOC5) */
    
#endif  /* End (PRS_1_TIME_MULTIPLEXING_ENABLE) */


#if (PRS_1_RUN_MODE == PRS_1__CLOCKED)
    /* Aux control */
    #if (PRS_1_TIME_MULTIPLEXING_ENABLE)    
        #if (PRS_1_PRS_SIZE <= 16u)      /* 8-16 bits PRS */
            #define PRS_1_AUX_CONTROL_A_REG      (*(reg8 *) PRS_1_b0_PRSdp_a__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_A_PTR      ( (reg8 *) PRS_1_b0_PRSdp_a__DP_AUX_CTL_REG )
            
        #elif (PRS_1_PRS_SIZE <= 24u)      /* 24-39 bits PRS */
            #define PRS_1_AUX_CONTROL_A_REG      (*(reg8 *) PRS_1_b0_PRSdp_a__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_A_PTR      ( (reg8 *) PRS_1_b0_PRSdp_a__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_REG      (*(reg8 *) PRS_1_b1_PRSdp_b__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_PTR      ( (reg8 *) PRS_1_b1_PRSdp_b__DP_AUX_CTL_REG )
            
        #elif (PRS_1_PRS_SIZE <= 40u)      /* 40-55 bits PRS */
            #define PRS_1_AUX_CONTROL_A_REG      (*(reg8 *) PRS_1_b0_PRSdp_a__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_A_PTR      ( (reg8 *) PRS_1_b0_PRSdp_a__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_REG      (*(reg8 *) PRS_1_b1_PRSdp_b__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_PTR      ( (reg8 *) PRS_1_b1_PRSdp_b__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_C_REG      (*(reg8 *) PRS_1_b2_PRSdp_c__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_C_PTR      ( (reg8 *) PRS_1_b2_PRSdp_c__DP_AUX_CTL_REG )

        #else                                         /* 56-64 bits PRS */
            #define PRS_1_AUX_CONTROL_A_REG      (*(reg8 *) PRS_1_b0_PRSdp_a__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_A_PTR      ( (reg8 *) PRS_1_b0_PRSdp_a__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_REG      (*(reg8 *) PRS_1_b1_PRSdp_b__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_PTR      ( (reg8 *) PRS_1_b1_PRSdp_b__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_C_REG      (*(reg8 *) PRS_1_b2_PRSdp_c__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_C_PTR      ( (reg8 *) PRS_1_b2_PRSdp_c__DP_AUX_CTL_REG )
			#define PRS_1_AUX_CONTROL_D_REG      (*(reg8 *) PRS_1_b3_PRSdp_d__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_D_PTR      ( (reg8 *) PRS_1_b3_PRSdp_d__DP_AUX_CTL_REG )     
																							
        #endif  /* End (PRS_1_PRS_SIZE <= 8u) */
    
    #else
        #if (PRS_1_PRS_SIZE <= 8u)      /* 8 bits PRS */
            #define PRS_1_AUX_CONTROL_A_REG      (*(reg8 *) PRS_1_sC8_PRSdp_u0__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_A_PTR      ( (reg8 *) PRS_1_sC8_PRSdp_u0__DP_AUX_CTL_REG )
            
        #elif (PRS_1_PRS_SIZE <= 16u)      /* 16 bits PRS */
            #define PRS_1_AUX_CONTROL_A_REG      (*(reg8 *) PRS_1_sC16_PRSdp_u0__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_A_PTR      ( (reg8 *) PRS_1_sC16_PRSdp_u0__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_REG      (*(reg8 *) PRS_1_sC16_PRSdp_u1__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_PTR      ( (reg8 *) PRS_1_sC16_PRSdp_u1__DP_AUX_CTL_REG )
            
        #elif (PRS_1_PRS_SIZE <= 24u)      /* 24-39 bits PRS */
            #define PRS_1_AUX_CONTROL_A_REG      (*(reg8 *) PRS_1_sC24_PRSdp_u0__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_A_PTR      ( (reg8 *) PRS_1_sC24_PRSdp_u0__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_REG      (*(reg8 *) PRS_1_sC24_PRSdp_u1__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_PTR      ( (reg8 *) PRS_1_sC24_PRSdp_u1__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_C_REG      (*(reg8 *) PRS_1_sC24_PRSdp_u2__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_C_PTR      ( (reg8 *) PRS_1_sC24_PRSdp_u2__DP_AUX_CTL_REG )
                
        #elif (PRS_1_PRS_SIZE <= 32u)      /* 40-55 bits PRS */
            #define PRS_1_AUX_CONTROL_A_REG      (*(reg8 *) PRS_1_sC32_PRSdp_u0__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_A_PTR      ( (reg8 *) PRS_1_sC32_PRSdp_u0__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_REG      (*(reg8 *) PRS_1_sC32_PRSdp_u1__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_B_PTR      ( (reg8 *) PRS_1_sC32_PRSdp_u1__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_C_REG      (*(reg8 *) PRS_1_sC32_PRSdp_u2__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_C_PTR      ( (reg8 *) PRS_1_sC32_PRSdp_u2__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_D_REG      (*(reg8 *) PRS_1_sC32_PRSdp_u3__DP_AUX_CTL_REG )
            #define PRS_1_AUX_CONTROL_D_PTR      ( (reg8 *) PRS_1_sC32_PRSdp_u3__DP_AUX_CTL_REG )
    
        #else                                         /* 56-64 bits PRS */
            /* Not supported */                    
        #endif  /* End (PRS_1_PRS_SIZE <= 8u) */
    
    #endif  /* End (PRS_1_TIME_MULTIPLEXING_ENABLE) */
    
#endif  /* End (PRS_1_RUN_MODE == PRS_1__CLOCKED) */

#define PRS_1_CONTROL_REG               (*(reg8 *) PRS_1_ClkSp_CtrlReg__CONTROL_REG )
#define PRS_1_CONTROL_PTR               ( (reg8 *) PRS_1_ClkSp_CtrlReg__CONTROL_REG )
    

/***************************************
*       Register Constants
***************************************/

/* Control register definitions */

#define PRS_1_CTRL_ENABLE                    (0x01u)
#define PRS_1_CTRL_RISING_EDGE               (0x02u)
#define PRS_1_CTRL_RESET_COMMON              (0x04u)
#define PRS_1_CTRL_RESET_CI                  (0x08u)
#define PRS_1_CTRL_RESET_SI                  (0x10u)
#define PRS_1_CTRL_RESET_SO                  (0x20u)
#define PRS_1_CTRL_RESET_STATE0              (0x40u)
#define PRS_1_CTRL_RESET_STATE1              (0x80u)

#define PRS_1_INIT_STATE                     ( PRS_1_CTRL_RESET_CI |\
                                                          PRS_1_CTRL_RESET_SI |\
                                                          PRS_1_CTRL_RESET_SO |\
                                                          PRS_1_CTRL_RESET_STATE0 |\
                                                          PRS_1_CTRL_RESET_STATE1 )
                                                          
/* Status register definitions */
#define PRS_1_STS_RESET_CI                   (0x08u)
#define PRS_1_STS_RESET_SI                   (0x10u)
#define PRS_1_STS_RESET_SO                   (0x20u)
#define PRS_1_STS_RESET_STATE0               (0x40u)
#define PRS_1_STS_RESET_STATE1               (0x80u)

/* Aux Control register definitions */
#define PRS_1_AUXCTRL_FIFO_SINGLE_REG        (0x03u)

#define PRS_1_NORMAL_SEQUENCE                (0x01u)

#define PRS_1_IS_PRS_ENABLE(reg)             (((reg) & PRS_1_CTRL_ENABLE) != 0u)

#endif  /* End CY_PRS_PRS_1_H */


/* [] END OF FILE */
