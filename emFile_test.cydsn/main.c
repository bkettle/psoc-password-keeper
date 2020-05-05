/*******************************************************************************
* File Name: main.c
*
* Version: 1.10
*
* Description:
*  The example project consists of 5 simple steps. Each of the steps performs 
*  single File System operation. 
*  Step#1: Gets SD card name and displays it on the LCD.
*  Step#2: Performs Format SD operation and displays the status of the format 
*  operation execution.
*  Step#3: Creates directory with the name - "Dir" and displays a status of 
*  directory creation operation.
*  Step#4: Creates a file - "File.txt" in the root directory and writes the 
*  following string to the file -"123456789". Displays a status of operations 
*  execution.
*  Step#5: Copies newly created file ("File.txt") to directory "Dir" with the 
*  name - "File1.txt". 
*
********************************************************************************
* Copyright 2011-2012, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <project.h>
//#include <FS.h>
//#include <string.h>
//#include <Global.h>


char sdFile[9] = "File.txt";


//void NextStep(U16 step, char * str)
//{
//    /* Clear LCD after previously completed operation */
//    //LCD_ClearDisplay();
//    
//    /* This will display step number on the LCD prior
//    * every operation on the SD card.
//    */
//    //LCD_Position(0u, 0u);
//    //LCD_PrintString("Step#");
//    //LCD_PrintDecUint16(step);
//    //LCD_Position(1u, 0u);
//    //LCD_PrintString(str);
//    UART_1_PutString("\n\r Current Step: ");
//    UART_1_PutString(str);
//    
//    /* Need some delay to make step number visible */
//    CyDelay(2000u);
//    
//    /* Clear LCD after step number was prompted */
//    //LCD_ClearDisplay();
//}

int main()
{
    UART_1_Start();
    
    for (;;) {
        UART_1_PutString("Hello!!!!!!!!");
        CyDelay(300);
    }
//    /* Start LCD */
//    //LCD_Start();
//    UART_1_Start();
//    UART_1_PutString("Hello!!!!!!!!");
//    
//    char sdVolName[10];     /* Buffer that will hold SD card Volume name */
//    U16 step = 1u;
//    FS_FILE * pFile;
//    
//    /* Initialize file system */
//    FS_Init();
//    
//    /* Display step number */
//    NextStep(step++, "SD CARD NAME");
//    
//    
//    
//    //LCD_Position(0u, 0u);
//    
//    /* Get volume name of SD card #0 */
//    if(0 != FS_GetVolumeName(0u, &sdVolName[0], 9u))
//    {
//        UART_1_PutString("Hell!!!!!!!!");
//        /* Getting volume name succeeded so prompt it on the LCD */
//        //LCD_PrintString("SD card name:");
//        //LCD_Position(1u, 0u);
//        //LCD_PrintString(sdVolName);
//        
//        UART_1_PutString("\n\r SD card name:");
//        UART_1_PutString(sdVolName);
//    }
//    else
//    {
//        /* Operation Failed - indicate this */
//        //LCD_PrintString("Failed to get");
//        //LCD_Position(1u, 0u);
//        //LCD_PrintString("SD card name");
//        
//        UART_1_PutString("\n\r Failed to get ");
//        UART_1_PutString("SD card name");
//    }
//    
//    /* Need some delay to indicate output on the LCD */
//    CyDelay(2000u);
//    
//    NextStep(step++, "SD CARD FORMAT");
//        
//    //LCD_Position(0u, 0u);
//    //LCD_PrintString("SD card format");
//    
//    //LCD_Position(0u, 0u);
//    //LCD_PrintString("SD card format");
//    UART_1_PutString("\n\r SD card format");
//    
//    if(0 == FS_FormatSD(sdVolName))
//    {
//        //LCD_Position(1u, 0u);
//        //LCD_PrintString("Succeeded");
//        UART_1_PutString("\n\r Succeeded");
//    }
//    else
//    {
//        //LCD_Position(1u, 0u);
//        //LCD_PrintString("Failed");
//        UART_1_PutString("\n\r Failed");
//    }
//    
//    /* Need some delay to indicate output on the LCD */
//    CyDelay(2000u);
//    
//    NextStep(step++, "DIRECTORY");
//    
//    //LCD_Position(0u, 0u);
//    
//    /* This will create directory "Dir" on SD card */
//    if(0 == FS_MkDir("Dir"))
//    {
//        /* Display successful directory creation message */
//        UART_1_PutString("\n\r \"Dir\" created");
//    }
//    else
//    {
//        /* Display failure message */
//        UART_1_PutString("\n\r Failed to create");
//        UART_1_PutString(" directory");
//    }
//    
//    /* Need some delay to indicate output on the LCD */
//    CyDelay(2000u);
//    
//    NextStep(step++, "FILE");
//    
//    /* Create specific file for modification */
//    pFile = FS_FOpen(sdFile, "w");
//    
//    //LCD_Position(0u, 0u);
//    
//    /* Check if file was created */
//    if(pFile)
//    {
//        /* Indicate successful file creation message */
//        UART_1_PutString("\n\r File ");
//        UART_1_PutString(sdFile);
//        //LCD_Position(1u, 0u);
//        UART_1_PutString(" was opened");
//        
//        /* Need some delay to indicate output on the LCD */
//        CyDelay(2000u);
//        
//        //LCD_ClearDisplay();
//        
//        //LCD_Position(0u, 0u);
//        
//        if(0 != FS_Write(pFile, "0123456789", 10u)) 
//        {
//            /* Inditate that data was written to a file */
//            UART_1_PutString("\n\r\"0123456789\"");
//            UART_1_PutString(sdFile);
//            //LCD_Position(1u, 0u);
//            UART_1_PutString(" written to file");
//        }
//        else
//        {
//            UART_1_PutString("\n\r Failed to write");
//            //LCD_Position(1u, 0u);
//            UART_1_PutString(" data to file");
//        }
//        
//        /* Need some delay to indicate output on the LCD */
//        CyDelay(2000u);
//        
//        //LCD_ClearDisplay();
//        
//        //LCD_Position(0u, 0u);
//        
//        if(0 == FS_FClose(pFile))
//        {
//            UART_1_PutString("\n\r File was closed");
//        }
//        else
//        {
//            UART_1_PutString("\n\r Failed to close");
//            //LCD_Position(1u, 0u);
//            UART_1_PutString("file");
//        }
//    }
//    else
//    {
//        UART_1_PutString("\n\r Failed to create");
//        //LCD_Position(1u, 0u);
//        UART_1_PutString(" file");
//    }
//    
//    /* Need some delay to indicate output on the LCD */
//    CyDelay(2000u);
//        
//    //LCD_ClearDisplay();
//    
//    NextStep(step++, "COPY");
//    
//    //LCD_Position(0u, 0u);
//    
//    /* Copy a file to a previously created directory */
//    if(0 == FS_CopyFile(sdFile, "\\Dir\\file1.txt"))
//    {
//        UART_1_PutString("\n\r File was ");
//        //LCD_Position(1u, 0u);
//        UART_1_PutString("copied to \"DIR\"");
//    }
//    else
//    {
//        UART_1_PutString("Failed to copy");
//        //LCD_Position(1u, 0u);
//        UART_1_PutString("file");
//    }
//    
//    /* Need some delay to indicate output on the LCD */
//    CyDelay(2000);
//    
//    //LCD_ClearDisplay();
//    
//    //LCD_Position(0u, 0u);
//    UART_1_PutString("\n\r Example project ");
//    //LCD_Position(1u, 0u);
//    UART_1_PutString(" completed");
//    
//    for(;;)
//    {
//
//    }
}


/* [] END OF FILE */

