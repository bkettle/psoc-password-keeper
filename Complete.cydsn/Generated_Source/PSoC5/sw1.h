/*******************************************************************************
* File Name: sw1.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_sw1_H) /* Pins sw1_H */
#define CY_PINS_sw1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "sw1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 sw1__PORT == 15 && ((sw1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    sw1_Write(uint8 value);
void    sw1_SetDriveMode(uint8 mode);
uint8   sw1_ReadDataReg(void);
uint8   sw1_Read(void);
void    sw1_SetInterruptMode(uint16 position, uint16 mode);
uint8   sw1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the sw1_SetDriveMode() function.
     *  @{
     */
        #define sw1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define sw1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define sw1_DM_RES_UP          PIN_DM_RES_UP
        #define sw1_DM_RES_DWN         PIN_DM_RES_DWN
        #define sw1_DM_OD_LO           PIN_DM_OD_LO
        #define sw1_DM_OD_HI           PIN_DM_OD_HI
        #define sw1_DM_STRONG          PIN_DM_STRONG
        #define sw1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define sw1_MASK               sw1__MASK
#define sw1_SHIFT              sw1__SHIFT
#define sw1_WIDTH              1u

/* Interrupt constants */
#if defined(sw1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in sw1_SetInterruptMode() function.
     *  @{
     */
        #define sw1_INTR_NONE      (uint16)(0x0000u)
        #define sw1_INTR_RISING    (uint16)(0x0001u)
        #define sw1_INTR_FALLING   (uint16)(0x0002u)
        #define sw1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define sw1_INTR_MASK      (0x01u) 
#endif /* (sw1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define sw1_PS                     (* (reg8 *) sw1__PS)
/* Data Register */
#define sw1_DR                     (* (reg8 *) sw1__DR)
/* Port Number */
#define sw1_PRT_NUM                (* (reg8 *) sw1__PRT) 
/* Connect to Analog Globals */                                                  
#define sw1_AG                     (* (reg8 *) sw1__AG)                       
/* Analog MUX bux enable */
#define sw1_AMUX                   (* (reg8 *) sw1__AMUX) 
/* Bidirectional Enable */                                                        
#define sw1_BIE                    (* (reg8 *) sw1__BIE)
/* Bit-mask for Aliased Register Access */
#define sw1_BIT_MASK               (* (reg8 *) sw1__BIT_MASK)
/* Bypass Enable */
#define sw1_BYP                    (* (reg8 *) sw1__BYP)
/* Port wide control signals */                                                   
#define sw1_CTL                    (* (reg8 *) sw1__CTL)
/* Drive Modes */
#define sw1_DM0                    (* (reg8 *) sw1__DM0) 
#define sw1_DM1                    (* (reg8 *) sw1__DM1)
#define sw1_DM2                    (* (reg8 *) sw1__DM2) 
/* Input Buffer Disable Override */
#define sw1_INP_DIS                (* (reg8 *) sw1__INP_DIS)
/* LCD Common or Segment Drive */
#define sw1_LCD_COM_SEG            (* (reg8 *) sw1__LCD_COM_SEG)
/* Enable Segment LCD */
#define sw1_LCD_EN                 (* (reg8 *) sw1__LCD_EN)
/* Slew Rate Control */
#define sw1_SLW                    (* (reg8 *) sw1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define sw1_PRTDSI__CAPS_SEL       (* (reg8 *) sw1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define sw1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) sw1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define sw1_PRTDSI__OE_SEL0        (* (reg8 *) sw1__PRTDSI__OE_SEL0) 
#define sw1_PRTDSI__OE_SEL1        (* (reg8 *) sw1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define sw1_PRTDSI__OUT_SEL0       (* (reg8 *) sw1__PRTDSI__OUT_SEL0) 
#define sw1_PRTDSI__OUT_SEL1       (* (reg8 *) sw1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define sw1_PRTDSI__SYNC_OUT       (* (reg8 *) sw1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(sw1__SIO_CFG)
    #define sw1_SIO_HYST_EN        (* (reg8 *) sw1__SIO_HYST_EN)
    #define sw1_SIO_REG_HIFREQ     (* (reg8 *) sw1__SIO_REG_HIFREQ)
    #define sw1_SIO_CFG            (* (reg8 *) sw1__SIO_CFG)
    #define sw1_SIO_DIFF           (* (reg8 *) sw1__SIO_DIFF)
#endif /* (sw1__SIO_CFG) */

/* Interrupt Registers */
#if defined(sw1__INTSTAT)
    #define sw1_INTSTAT            (* (reg8 *) sw1__INTSTAT)
    #define sw1_SNAP               (* (reg8 *) sw1__SNAP)
    
	#define sw1_0_INTTYPE_REG 		(* (reg8 *) sw1__0__INTTYPE)
#endif /* (sw1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_sw1_H */


/* [] END OF FILE */
