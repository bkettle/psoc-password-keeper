/*******************************************************************************
* File Name: Dial.h  
* Version 3.0
*
* Description:
*  This file provides constants and parameter values for the Quadrature
*  Decoder component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_QUADRATURE_DECODER_Dial_H)
#define CY_QUADRATURE_DECODER_Dial_H

#include "cyfitter.h"
#include "CyLib.h"
#include "cytypes.h"

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component QuadDec_v3_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#define Dial_COUNTER_SIZE               (8u)
#define Dial_COUNTER_SIZE_8_BIT         (8u)
#define Dial_COUNTER_SIZE_16_BIT        (16u)
#define Dial_COUNTER_SIZE_32_BIT        (32u)

#if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT)
    #include "Dial_Cnt8.h"
#else 
    /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT) || 
    *  (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT) 
    */
    #include "Dial_Cnt16.h"
#endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT */

extern uint8 Dial_initVar;


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define Dial_COUNTER_RESOLUTION         (1u)


/***************************************
*       Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
} Dial_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  Dial_Init(void) ;
void  Dial_Start(void) ;
void  Dial_Stop(void) ;
void  Dial_Enable(void) ;
uint8 Dial_GetEvents(void) ;
void  Dial_SetInterruptMask(uint8 mask) ;
uint8 Dial_GetInterruptMask(void) ;
int8 Dial_GetCounter(void) ;
void  Dial_SetCounter(int8 value)
;
void  Dial_Sleep(void) ;
void  Dial_Wakeup(void) ;
void  Dial_SaveConfig(void) ;
void  Dial_RestoreConfig(void) ;

#if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)
    CY_ISR_PROTO(Dial_ISR);
#endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT */


/***************************************
*           API Constants
***************************************/

#if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)
    #define Dial_ISR_NUMBER             ((uint8) Dial_isr__INTC_NUMBER)
    #define Dial_ISR_PRIORITY           ((uint8) Dial_isr__INTC_PRIOR_NUM)
#endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT */


/***************************************
*    Enumerated Types and Parameters
***************************************/

#define Dial_GLITCH_FILTERING           (1u)
#define Dial_INDEX_INPUT                (0u)


/***************************************
*    Initial Parameter Constants
***************************************/

#if (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT)
    #define Dial_COUNTER_INIT_VALUE    (0x80u)
#else 
    /* (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_16_BIT) ||
    *  (Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_32_BIT)
    */
    #define Dial_COUNTER_INIT_VALUE    (0x8000u)
    #define Dial_COUNTER_MAX_VALUE     (0x7FFFu)
#endif /* Dial_COUNTER_SIZE == Dial_COUNTER_SIZE_8_BIT */


/***************************************
*             Registers
***************************************/

#define Dial_STATUS_REG                 (* (reg8 *) Dial_bQuadDec_Stsreg__STATUS_REG)
#define Dial_STATUS_PTR                 (  (reg8 *) Dial_bQuadDec_Stsreg__STATUS_REG)
#define Dial_STATUS_MASK                (* (reg8 *) Dial_bQuadDec_Stsreg__MASK_REG)
#define Dial_STATUS_MASK_PTR            (  (reg8 *) Dial_bQuadDec_Stsreg__MASK_REG)
#define Dial_SR_AUX_CONTROL             (* (reg8 *) Dial_bQuadDec_Stsreg__STATUS_AUX_CTL_REG)
#define Dial_SR_AUX_CONTROL_PTR         (  (reg8 *) Dial_bQuadDec_Stsreg__STATUS_AUX_CTL_REG)


/***************************************
*        Register Constants
***************************************/

#define Dial_COUNTER_OVERFLOW_SHIFT     (0x00u)
#define Dial_COUNTER_UNDERFLOW_SHIFT    (0x01u)
#define Dial_COUNTER_RESET_SHIFT        (0x02u)
#define Dial_INVALID_IN_SHIFT           (0x03u)
#define Dial_COUNTER_OVERFLOW           ((uint8) (0x01u << Dial_COUNTER_OVERFLOW_SHIFT))
#define Dial_COUNTER_UNDERFLOW          ((uint8) (0x01u << Dial_COUNTER_UNDERFLOW_SHIFT))
#define Dial_COUNTER_RESET              ((uint8) (0x01u << Dial_COUNTER_RESET_SHIFT))
#define Dial_INVALID_IN                 ((uint8) (0x01u << Dial_INVALID_IN_SHIFT))

#define Dial_INTERRUPTS_ENABLE_SHIFT    (0x04u)
#define Dial_INTERRUPTS_ENABLE          ((uint8)(0x01u << Dial_INTERRUPTS_ENABLE_SHIFT))
#define Dial_INIT_INT_MASK              (0x0Fu)


/******************************************************************************************
* Following code are OBSOLETE and must not be used starting from Quadrature Decoder 2.20
******************************************************************************************/
#define Dial_DISABLE                    (0x00u)


#endif /* CY_QUADRATURE_DECODER_Dial_H */


/* [] END OF FILE */
