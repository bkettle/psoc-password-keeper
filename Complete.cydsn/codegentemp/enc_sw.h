/*******************************************************************************
* File Name: enc_sw.h  
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

#if !defined(CY_PINS_enc_sw_H) /* Pins enc_sw_H */
#define CY_PINS_enc_sw_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "enc_sw_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 enc_sw__PORT == 15 && ((enc_sw__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    enc_sw_Write(uint8 value);
void    enc_sw_SetDriveMode(uint8 mode);
uint8   enc_sw_ReadDataReg(void);
uint8   enc_sw_Read(void);
void    enc_sw_SetInterruptMode(uint16 position, uint16 mode);
uint8   enc_sw_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the enc_sw_SetDriveMode() function.
     *  @{
     */
        #define enc_sw_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define enc_sw_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define enc_sw_DM_RES_UP          PIN_DM_RES_UP
        #define enc_sw_DM_RES_DWN         PIN_DM_RES_DWN
        #define enc_sw_DM_OD_LO           PIN_DM_OD_LO
        #define enc_sw_DM_OD_HI           PIN_DM_OD_HI
        #define enc_sw_DM_STRONG          PIN_DM_STRONG
        #define enc_sw_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define enc_sw_MASK               enc_sw__MASK
#define enc_sw_SHIFT              enc_sw__SHIFT
#define enc_sw_WIDTH              1u

/* Interrupt constants */
#if defined(enc_sw__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in enc_sw_SetInterruptMode() function.
     *  @{
     */
        #define enc_sw_INTR_NONE      (uint16)(0x0000u)
        #define enc_sw_INTR_RISING    (uint16)(0x0001u)
        #define enc_sw_INTR_FALLING   (uint16)(0x0002u)
        #define enc_sw_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define enc_sw_INTR_MASK      (0x01u) 
#endif /* (enc_sw__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define enc_sw_PS                     (* (reg8 *) enc_sw__PS)
/* Data Register */
#define enc_sw_DR                     (* (reg8 *) enc_sw__DR)
/* Port Number */
#define enc_sw_PRT_NUM                (* (reg8 *) enc_sw__PRT) 
/* Connect to Analog Globals */                                                  
#define enc_sw_AG                     (* (reg8 *) enc_sw__AG)                       
/* Analog MUX bux enable */
#define enc_sw_AMUX                   (* (reg8 *) enc_sw__AMUX) 
/* Bidirectional Enable */                                                        
#define enc_sw_BIE                    (* (reg8 *) enc_sw__BIE)
/* Bit-mask for Aliased Register Access */
#define enc_sw_BIT_MASK               (* (reg8 *) enc_sw__BIT_MASK)
/* Bypass Enable */
#define enc_sw_BYP                    (* (reg8 *) enc_sw__BYP)
/* Port wide control signals */                                                   
#define enc_sw_CTL                    (* (reg8 *) enc_sw__CTL)
/* Drive Modes */
#define enc_sw_DM0                    (* (reg8 *) enc_sw__DM0) 
#define enc_sw_DM1                    (* (reg8 *) enc_sw__DM1)
#define enc_sw_DM2                    (* (reg8 *) enc_sw__DM2) 
/* Input Buffer Disable Override */
#define enc_sw_INP_DIS                (* (reg8 *) enc_sw__INP_DIS)
/* LCD Common or Segment Drive */
#define enc_sw_LCD_COM_SEG            (* (reg8 *) enc_sw__LCD_COM_SEG)
/* Enable Segment LCD */
#define enc_sw_LCD_EN                 (* (reg8 *) enc_sw__LCD_EN)
/* Slew Rate Control */
#define enc_sw_SLW                    (* (reg8 *) enc_sw__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define enc_sw_PRTDSI__CAPS_SEL       (* (reg8 *) enc_sw__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define enc_sw_PRTDSI__DBL_SYNC_IN    (* (reg8 *) enc_sw__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define enc_sw_PRTDSI__OE_SEL0        (* (reg8 *) enc_sw__PRTDSI__OE_SEL0) 
#define enc_sw_PRTDSI__OE_SEL1        (* (reg8 *) enc_sw__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define enc_sw_PRTDSI__OUT_SEL0       (* (reg8 *) enc_sw__PRTDSI__OUT_SEL0) 
#define enc_sw_PRTDSI__OUT_SEL1       (* (reg8 *) enc_sw__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define enc_sw_PRTDSI__SYNC_OUT       (* (reg8 *) enc_sw__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(enc_sw__SIO_CFG)
    #define enc_sw_SIO_HYST_EN        (* (reg8 *) enc_sw__SIO_HYST_EN)
    #define enc_sw_SIO_REG_HIFREQ     (* (reg8 *) enc_sw__SIO_REG_HIFREQ)
    #define enc_sw_SIO_CFG            (* (reg8 *) enc_sw__SIO_CFG)
    #define enc_sw_SIO_DIFF           (* (reg8 *) enc_sw__SIO_DIFF)
#endif /* (enc_sw__SIO_CFG) */

/* Interrupt Registers */
#if defined(enc_sw__INTSTAT)
    #define enc_sw_INTSTAT            (* (reg8 *) enc_sw__INTSTAT)
    #define enc_sw_SNAP               (* (reg8 *) enc_sw__SNAP)
    
	#define enc_sw_0_INTTYPE_REG 		(* (reg8 *) enc_sw__0__INTTYPE)
#endif /* (enc_sw__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_enc_sw_H */


/* [] END OF FILE */
