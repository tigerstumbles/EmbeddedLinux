
//#include <stdio.h>
//#include "xgpio_l.h"
#include "xgpio.h"
#include "tmp.h"


XGpio lcd,led;

//the unit of count is ms
void delay(unsigned int count)
{
		unsigned int a,b;
		for(a=0; a<count; a++)
			for(b=0; b<120; b++);
}

void LCD_Init(void)
{
	delay(15);
	LCD_Write_Command(0x38);
	delay(5);
	LCD_Write_Command(0x38);
	delay(5);
	LCD_Write_Command(0x38);
	delay(5);

	LCD_Write_Command(0x38);
	LCD_Write_Command(0x08);
	LCD_Write_Command(0x01);
	LCD_Write_Command(0x06);
	LCD_Write_Command(0x0c);

}

void LCD_Write_Command(unsigned long comm)
{

	int rs_bit,rw_bit,e_bit,dat;
	rs_bit=0;
	rw_bit=0;
	e_bit=1;
	dat=comm&0xff;
	XGpio_DiscreteWrite(&lcd, 1,(rs_bit<<10)|(rw_bit<<9)|(e_bit<<8)|dat);

	e_bit=0;
	XGpio_DiscreteWrite(&lcd, 1,(rs_bit<<10)|(rw_bit<<9)|(e_bit<<8)|dat);

	delay(1);
}

void LCD_Write_Data(unsigned long data)
{

	int rs_bit,rw_bit,e_bit,dat;
	rs_bit=1;
	rw_bit=0;
	e_bit=1;
	dat=data&0xff;
	XGpio_DiscreteWrite(&lcd, 1,(rs_bit<<10)|(rw_bit<<9)|(e_bit<<8)|dat);

	e_bit=0;
	XGpio_DiscreteWrite(&lcd, 1,(rs_bit<<10)|(rw_bit<<9)|(e_bit<<8)|dat);

	delay(1);
}


/*******Basic Functions*******/
void LCD_Move_Cursor_Home()
{
	LCD_Write_Command(0x00000002);
}

void LCD_Move_Cursor_Left()
{
	LCD_Write_Command(0x00000010);
}

void LCD_Move_Cursor_Right()
{
	LCD_Write_Command(0x00000014);
}

void LCD_Disp_On()
{
	LCD_Write_Command(0x0000000E);
}

void LCD_Disp_Off()
{
	LCD_Write_Command(0x00000008);
}

void LCD_Clear()
{
	LCD_Write_Command(0x00000001);
	LCD_Write_Command(0x00000002);
}
/*******Basic Functions*******/


void LCD_Set_Line(int line)
{ //line1 = 1, line2 = 2
	int i;
	if((line - 1)) {
		LCD_Move_Cursor_Home();
		for(i=0; i<40; i++)
			LCD_Move_Cursor_Right();
	}
	else
		LCD_Move_Cursor_Home();
}


void LCD_Disp_Char(int x,int y,char c)
{

	unsigned char address;
	if(y==1)
		address=0x80+x;
	else
		address=0xc0+x;
	LCD_Write_Command(address);
	LCD_Write_Data(c);

}

void LCD_Disp_String(char * line)
{
	int i=0;
	for(i=0; i<16; i++)
	{
		if(line[i])
			LCD_Disp_Char(i,1,line[i]);
		else
			break;
	}

	if(line[i])
	{
		LCD_Set_Line(2);
		for(i=0;i<16;i++)
		{
			if(line[i+16])
				LCD_Disp_Char(i,2,line[i+16]);
			else
				break;
		}
	}

	return;
}

void LCD_Disp_Num(int x,int y,unsigned int num,unsigned int base)
{
  static char hex[]="0123456789ABCDEF";
  char digit[10];
  int i;
  i = 0;
  do
  {
    digit[i] = hex[num % base];
    num = num / base;
    i++;
  } while (num != 0);
  while (i > 0)
  {
  	i--;
  	LCD_Disp_Char(x,y,digit[i]);
  	x++;
  }
}

void LCD_Disp_Int(int x,int y,unsigned int num)
{
  unsigned int val;
  if (x < 0)
  {
	  LCD_Disp_Char(x,y,'-');
    val = ((unsigned int) ~num ) + 1;
  }
  else
    val = (unsigned int) num;
  LCD_Disp_Num(x,y,val, 10);
}



void	main()
{
	XGpio_Initialize(&lcd,XPAR_LCD_11BIT_DEVICE_ID);
	XGpio_SetDataDirection(&lcd, 1, 0x0);

	XGpio_Initialize(&led,XPAR_LED_2BIT_DEVICE_ID);
	XGpio_SetDataDirection(&led, 1, 0x0);

	xil_printf("\r\nProgram Start\r\n");
	xil_printf("========\r\n");
	LCD_Init();

	int tmp;
	while(1)
	{

		tmp=Read_Tmp(1);
		LCD_Disp_Int(0,1,tmp);

		if(tmp>30)
			XGpio_DiscreteWrite(&led,1,0x1);
		else if(tmp<15)
			XGpio_DiscreteWrite(&led,1,0x2);
		else
			XGpio_DiscreteWrite(&led,1,0x0);

		delay(100);
	}

	exit(0);	
}  
