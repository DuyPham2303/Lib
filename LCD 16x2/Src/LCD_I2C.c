/******************************************************************************************************************
@File:  	LCD I2C Chip PCF8574
@Author:  Khue Nguyen
@Website: khuenguyencreator.com
@Youtube: https://www.youtube.com/channel/UCt8cFnPOaHrQXWmVkk-lfvg
Huong dan su dung:
- Su dung thu vien HAL
- Khoi tao bien LCD: LCD_I2C_Name LCD1;
- Khoi tao LCD do: LCD_I2C_Init(&LCD1,&hi2c1,0x4e,20,4);
- Su dung cac ham phai truyen dia chi cua LCD do: 
LCD_I2C_SetCursor(&LCD1, 0, 1);
LCD_I2C_WriteString(&LCD1,"hello anh em ");
******************************************************************************************************************/
#include "LCD_I2C.h"

//************************** LOW HARDWARE LAYER  ****************************************************************//
static void LCD_Delay(uint16_t Time)
{
	HAL_Delay(Time);
}
static void LCD_WriteI2C(LCD_I2C_Name* LCD, uint8_t bit, uint8_t Mode)
{
	char bit_high;
	char bit_low;
	uint8_t bit_I2C[4];
	bit_high = bit&0xF0;
	bit_low = (bit<<4)&0xF0;
	if(LCD->BACKLIGHT)
	{
		bit_high |= LCD_BACKLIGHT; 
		bit_low |= LCD_BACKLIGHT; 
	}
	if(Mode == LCD_DATA)
	{
		bit_high |= LCD_RS;
		bit_low |= LCD_RS;
	}
	else if(Mode == LCD_COMMAND)
	{
		bit_high &= ~LCD_RS;
		bit_low &= ~LCD_RS;
	}
	bit_I2C[0] = bit_high|LCD_EN;
	LCD_Delay(1);
	bit_I2C[1] = bit_high;
	bit_I2C[2] = bit_low|LCD_EN;
	LCD_Delay(1);
	bit_I2C[3] = bit_low;
	HAL_I2C_Master_Transmit(LCD->I2C, LCD->ADDRESS, (uint8_t *)bit_I2C, sizeof(bit_I2C), 1000);
}
//************************** MIDDLE FUNCTION LAYER ****************************************************************//
void LCD_I2C_Init(LCD_I2C_Name* LCD, I2C_HandleTypeDef* hi2c_lowCD, uint8_t Address, uint8_t Colums, uint8_t Rows)
{
	LCD->I2C = hi2c_lowCD;
	LCD->ADDRESS = Address;
	LCD->COLUMS = Colums;
	LCD->ROWS = Rows;
	
	LCD->FUNCTIONSET = LCD_FUNCTIONSET|LCD_4BITMODE|LCD_2LINE|LCD_5x8DOTS;
	LCD->ENTRYMODE = LCD_ENTRYMODESET|LCD_ENTRYLEFT|LCD_ENTRYSHIFTDECREMENT;
	LCD->DISPLAYCTRL =LCD_DISPLAYCONTROL|LCD_DISPLAYON|LCD_CURSOROFF|LCD_BLINKOFF;
	LCD->CURSORSHIFT = LCD_CURSORSHIFT|LCD_CURSORMOVE|LCD_MOVERIGHT;
	LCD->BACKLIGHT = LCD_BACKLIGHT;

	LCD_Delay(50);
	LCD_WriteI2C(LCD, 0x33, LCD_COMMAND);
//	LCD_Delay(5);
	LCD_WriteI2C(LCD, 0x33, LCD_COMMAND);
	LCD_Delay(5);
	LCD_WriteI2C(LCD, 0x32, LCD_COMMAND);
	LCD_Delay(5);
	LCD_WriteI2C(LCD, 0x20, LCD_COMMAND);
	LCD_Delay(5);
	
	LCD_WriteI2C(LCD, LCD->ENTRYMODE,LCD_COMMAND);
	LCD_WriteI2C(LCD, LCD->DISPLAYCTRL,LCD_COMMAND);
	LCD_WriteI2C(LCD, LCD->CURSORSHIFT,LCD_COMMAND);
	LCD_WriteI2C(LCD, LCD->FUNCTIONSET,LCD_COMMAND);
	
	LCD_WriteI2C(LCD, LCD_CLEARDISPLAY,LCD_COMMAND);
	LCD_WriteI2C(LCD, LCD_RETURNHOME,LCD_COMMAND);
}
void LCD_I2C_SetCursor(LCD_I2C_Name* LCD, uint8_t Xpos, uint8_t Ypos)
{
	uint8_t DRAM_ADDRESS = 0x00;
	if(Xpos >= LCD->COLUMS)
	{
		Xpos = LCD->COLUMS - 1;
	}
	if(Ypos >= LCD->ROWS)
	{
		Ypos = LCD->ROWS -1;
	}
	if(Ypos == 0)
	{
		DRAM_ADDRESS = 0x00 + Xpos;
	}
	else if(Ypos == 1)
	{
		DRAM_ADDRESS = 0x40 + Xpos;
	}
	else if(Ypos == 2)
	{
		DRAM_ADDRESS = 0x14 + Xpos;
	}
	else if(Ypos == 3)
	{
		DRAM_ADDRESS = 0x54 + Xpos;
	}
	LCD_WriteI2C(LCD, LCD_SETDDRAMADDR|DRAM_ADDRESS, LCD_COMMAND);
}
void LCD_I2C_WriteChar(LCD_I2C_Name* LCD, char character)
{
	LCD_WriteI2C(LCD, character, LCD_DATA);
}
void LCD_I2C_WriteString(LCD_I2C_Name* LCD, char *String)
{
	while(*String)LCD_I2C_WriteChar(LCD, *String++);
}
void LCD_I2C_Clear(LCD_I2C_Name* LCD)
{
	LCD_WriteI2C(LCD, LCD_CLEARDISPLAY, LCD_COMMAND);
	LCD_Delay(5);
}
void LCD_I2C_ReturnHome(LCD_I2C_Name* LCD)
{
	LCD_WriteI2C(LCD, LCD_RETURNHOME, LCD_COMMAND);
	LCD_Delay(5);
}
// CURSUR CONTROL FUNCTION
void LCD_I2C_CursorOn(LCD_I2C_Name* LCD)
{
	LCD->DISPLAYCTRL |= LCD_CURSORON;
	LCD_WriteI2C(LCD, LCD->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_CursorOff(LCD_I2C_Name* LCD)
{
	LCD->DISPLAYCTRL &= ~LCD_CURSORON;
	LCD_WriteI2C(LCD, LCD->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_BlinkOn(LCD_I2C_Name* LCD)
{
	LCD->DISPLAYCTRL |= LCD_BLINKON;
	LCD_WriteI2C(LCD, LCD->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_BlinkOff(LCD_I2C_Name* LCD)
{
	LCD->DISPLAYCTRL &= ~LCD_BLINKON;
	LCD_WriteI2C(LCD, LCD->DISPLAYCTRL, LCD_COMMAND);
}
// DISPLAY CONTROL FUNCTION
void LCD_I2C_DisplayOff(LCD_I2C_Name* LCD)
{
	LCD->DISPLAYCTRL |= LCD_DISPLAYOFF;
	LCD_WriteI2C(LCD, LCD->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_DisplayOn(LCD_I2C_Name* LCD){
	LCD->DISPLAYCTRL |= LCD_DISPLAYON;
	LCD_WriteI2C(LCD, LCD->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_DisplayMove(LCD_I2C_Name* LCD){
	LCD->DISPLAYCTRL |=  LCD_DISPLAYMOVE ;
	LCD_WriteI2C(LCD, LCD->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_CursorMove(LCD_I2C_Name* LCD){
	LCD->DISPLAYCTRL |=   LCD_CURSORMOVE;
	LCD_WriteI2C(LCD, LCD->DISPLAYCTRL, LCD_COMMAND);
}
// CURSOR SHIFT FUNCTION
void LCD_I2C_MoveRight(LCD_I2C_Name* LCD){
	LCD->CURSORSHIFT |= LCD_MOVERIGHT;
	LCD_WriteI2C(LCD, LCD->CURSORSHIFT, LCD_COMMAND);
}
void LCD_I2C_MoveLeft(LCD_I2C_Name* LCD){
	LCD->CURSORSHIFT |= LCD_MOVELEFT;
	LCD_WriteI2C(LCD, LCD->CURSORSHIFT, LCD_COMMAND);
}
//ENTRY MODE FUNCTION
void LCD_I2C_ShiftIncrement(LCD_I2C_Name* LCD){
	LCD->ENTRYMODE |= LCD_ENTRYSHIFTINCREMENT;
	LCD_WriteI2C(LCD, LCD->ENTRYMODE , LCD_COMMAND);
}
void LCD_I2C_ShiftDecrement(LCD_I2C_Name* LCD){
	LCD->ENTRYMODE |= LCD_ENTRYSHIFTDECREMENT;
	LCD_WriteI2C(LCD, LCD->ENTRYMODE , LCD_COMMAND);
}
