/*
 * GccApplication1.c
 *
 * Created: 2/4/2021 12:40:52 PM
 * Author : Alireza Nobakht
 */ 

#include<avr/io.h>
#define F_CPU 8000000UL
#include<util/delay.h>

unsigned char i;

// numbers and shapes here:

unsigned char one[8] = {
	0,
	0,
	72,
	68,
	126,
	64,
	0,
	0
};

unsigned char two[8] = {
	0,
	0,
	68,
	98,
	82,
	76,
	0,
	0
};

unsigned char three[8] = {
	0,
	0,
	68,
	82,
	82,
	44,
	0,
	0
};

unsigned char four[8] = {
	0,
	16,
	24,
	20,
	126,
	16,
	16,
	0
};

unsigned char five[8] = {
	0,
	0,
	30,
	82,
	74,
	74,
	48,
	0
};

unsigned char block[8] = {
	0,
	126,
	126,
	126,
	126,
	126,
	126,
	0
};

unsigned char const bomb[8] = {
	0,
	90,
	36,
	66,
	66,
	36,
	90,
	0
};

unsigned char const flag[8] = {
	0,
	126,
	54,
	34,
	0,
	54,
	126,
	0
};


unsigned char mine[] = {
	0x00, 0x7F, 0x02, 0x04, 0x02, 0x7F,      // Code for char M
	0x00,
	0x00, 0x00, 0x41, 0x7F, 0x41, 0x00,  // Code for char I
	0x00,
	0x00, 0x7F, 0x02, 0x04, 0x08, 0x7F,      // Code for char N
	0x00,
	0x00, 0x7F, 0x49, 0x49, 0x49, 0x41,      // Code for char E	
};

unsigned char sweeper[] = {
	0x00, 0x26, 0x49, 0x49, 0x49, 0x32,      // Code for char S
	0x00,
	0x00, 0x3F, 0x40, 0x3C, 0x40, 0x3F,      // Code for char W
	0x00,
	0x00, 0x7F, 0x49, 0x49, 0x49, 0x41,      // Code for char E
	0x00,
	0x00, 0x7F, 0x49, 0x49, 0x49, 0x41,      // Code for char E
	0x00,
	0x00, 0x7F, 0x09, 0x09, 0x09, 0x06,      // Code for char P
	0x00,
	0x00, 0x7F, 0x49, 0x49, 0x49, 0x41,      // Code for char E
	0x00,
	0x00, 0x7F, 0x09, 0x09, 0x19, 0x66,      // Code for char R
};

unsigned char gameover[]= {
	0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A,      // Code for char G
	0x00, 0x20, 0x54, 0x54, 0x54, 0x78,      // Code for char a
	0x00, 0x7C, 0x04, 0x18, 0x04, 0x78,      // Code for char m
	0x00, 0x38, 0x54, 0x54, 0x54, 0x08,      // Code for char e
	0x00,
	0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E,      // Code for char O
	0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C,      // Code for char v
	0x00, 0x38, 0x54, 0x54, 0x54, 0x08,      // Code for char e
	0x00, 0x44, 0x78, 0x44, 0x04, 0x08,      // Code for char r
	0x00, 0x00, 0x00, 0x5F, 0x00, 0x00,      // Code for char !
};

unsigned char victory[] = {
	 0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F,      // Code for char V
	 0x00, 0x00, 0x00, 0x7D, 0x40, 0x00,      // Code for char i
	 0x00, 0x38, 0x44, 0x44, 0x44, 0x28,      // Code for char c
	 0x00, 0x04, 0x3E, 0x44, 0x24, 0x00,      // Code for char t
	 0x00, 0x38, 0x44, 0x44, 0x44, 0x38,      // Code for char o
	 0x00, 0x44, 0x78, 0x44, 0x04, 0x08,      // Code for char r
	 0x00, 0x9C, 0xA0, 0x60, 0x3C, 0x00,      // Code for char y
	 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00,      // Code for char !
};

unsigned char l_x,l_y,l_shape;

void select_chip(int);

void glcd_int();
void glcd_data(unsigned char);
void glcd_cmd(unsigned char);

void set_block(unsigned char x,unsigned char y,unsigned char shape,unsigned char is_cursor);
void set_display(unsigned char x,unsigned char y,unsigned char shape,unsigned char is_cursor);

void glcd_game_restart();
void glcd_game_over();
void glcd_victory();

void glcd_cmd(unsigned char cmd) //subroutine for command
{
	PORTC=cmd;
	_delay_ms(2);
	PORTD&=~(1<<2); // SET RS = 0 --> COMMAND
	PORTD&=~(1<<1); // SET R/W = 0 --> WRITE

	PORTD|=(1<<0); // SEND PULSE
	_delay_ms(2);
	PORTD&=~(1<<0);
}

void glcd_data(unsigned char dat)  //subroutine for data
{
	PORTC=dat;
	_delay_ms(2);
	PORTD|=(1<<2); // SET RS = 1 --> DATA
	PORTD&=~(1<<1); // SET R/W = 0 --> WRITE

	PORTD|=(1<<0); // SET E = 1
	_delay_ms(2);
	PORTD&=~(1<<0); // SET E = 0
}

void glcd_set()                //subroutine for initialaization
{
	unsigned char command[3]={0xb8,0x40,0x3f};
	select_chip(2);                      //Chip selection
	for(i=0;i<3;i++)
	glcd_cmd(command[i]);
	select_chip(1);
	for(i=0;i<3;i++)
	glcd_cmd(command[i]);
}

void select_chip(int chip)    //Chip selection
{	// CS1 --> PD4
	// CS2 --> PD3
	if(chip==2)
	{
		PORTD|=(1<<4);
		PORTD&=~(1<<3);
	}
	else
	{
		PORTD&=~(1<<4);
		PORTD|=(1<<3);
	}
}

void glcd_init(){
	_delay_ms(2);
	PORTD&=~(1<<5);
	_delay_ms(10);
	PORTD|=(1<<5);
	_delay_ms(2);

	glcd_set();                    //initialization
	_delay_ms(1);
	
	
	select_chip(2);

	glcd_cmd(0xb8);
	glcd_cmd(0x40);
	for(i=0;i<64;i++){
		glcd_data(0);
	}

	glcd_cmd(0xb8|1);
	glcd_cmd(0x40);
	for (i=0;i<18;i++){
		glcd_data(0);
	}
	for(i=0;i<27;i++){
		glcd_data(mine[i]);
	}
	for (i=0;i<19;i++){
		glcd_data(0);
	}
	
	
	glcd_cmd(0xb8|2);
	glcd_cmd(0x40);
	for (i=0;i<8;i++){
		glcd_data(0);
	}
	for(i=0;i<48;i++){
		glcd_data(sweeper[i]);
	}
	for (i=0;i<8;i++){
		glcd_data(0);
	}
	
	glcd_cmd(0xb8|3);
	glcd_cmd(0x40);
	for(i=0;i<64;i++){
		glcd_data(0);
	}
	glcd_cmd(0xb8|4);
	glcd_cmd(0x40);
	for(i=0;i<64;i++){
		glcd_data(0);
	}
	
	glcd_cmd(0xb8|6);
	glcd_cmd(0x40);
	for(i=0;i<64;i++){
		glcd_data(0);
	}
	
	glcd_cmd(0xb8|7);
	glcd_cmd(0x40);
	for(i=0;i<64;i++){
		glcd_data(0);
	}
	
	// glcd_game_restart();
}

void glcd_game_restart(){
	l_x=0;
	l_y=0;
	l_shape=12;
	select_chip(2);
	
	for(i=5;i<6;i++){
		glcd_cmd(0xb8|i);
		glcd_cmd(0x40);
		for(int j=0;j<64;j++){
			glcd_data(0);
		}
	}
	
	for(unsigned char w=0;w<8;w++){
		for(unsigned char z=0;z<8;z++){
			set_block(w,z,12,0);
		}
	}
}

void glcd_game_over(){
	select_chip(2);
	glcd_cmd(0xb8|5);
	glcd_cmd(0x40);
	for(i=0;i<4;i++){
		glcd_data(0);
	}
	for(i=0;i<55;i++){
		glcd_data(gameover[i]);
	}
	for(i=0;i<5;i++){
		glcd_data(0);
	}
}

void glcd_victory(){
	select_chip(2);
	glcd_cmd(0xb8|5);
	glcd_cmd(0x40);
	for(i=0;i<8;i++){
		glcd_data(0);
	}
	for(i=0;i<48;i++){
		glcd_data(victory[i]);
	}
	for(i=0;i<8;i++){
		glcd_data(0);
	}
}

void set_block(unsigned char x,unsigned char y,unsigned char shape,unsigned char is_cursor){
	y=y*8;
	select_chip(1);
	glcd_cmd(0xb8|x);
	glcd_cmd(0x40|y);
	unsigned char out[8];
	if(!is_cursor){
		for(i=0;i<8;i++){
			out[i]=0;
		}
	}
	else{
		out[0]=255;
		out[7]=255;
		for(i=1;i<7;i++){
			out[i]=129;
		}
	}
	
	switch(shape){
		case 0:
		for(i=0;i<8;i++){
			out[i]|=0;
		}
		break;
		case 1:
		for(i=0;i<8;i++){
			out[i]|=one[i];
		}
		break;
		case 2:
		for(i=0;i<8;i++){
			out[i]|=two[i];
		}
		break;
		case 3:
		for(i=0;i<8;i++){
			out[i]|=three[i];
		}
		break;
		case 4:
		for(i=0;i<8;i++){
			out[i]|=four[i];
		}
		break;
		case 5:
		for(i=0;i<8;i++){
			out[i]|=five[i];
		}
		break;
		case 10:
		for(i=0;i<8;i++){
			out[i]|=bomb[i];
		}
		break;
		case 11:
		for(i=0;i<8;i++){
			out[i]|=flag[i];
		}
		break;
		case 12:
		for(i=0;i<8;i++){
			out[i]|=block[i];
		}
		break;
	}
	for(i=0;i<8;i++){
		glcd_data(out[i]);
	}
}

void set_display(unsigned char x,unsigned char y,unsigned char shape,unsigned char is_cursor){
	if(!is_cursor){
		set_block(x,y,shape,is_cursor);
	}
	else{
		set_block(l_x,l_y,l_shape,0);
		set_block(x,y,shape,1);
		l_x=x;
		l_y=y;
		l_shape=shape;
	}
}


unsigned char table1[8][8] = {0,0,1,2,3,2,1,0,
	1,1,1,10,10,10,2,0,
	10,1,1,2,4,10,2,0,
	1,1,0,0,1,1,1,0,
	0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,
	10,1,1,10,1,2,10,3,
1,1,1,1,1,2,10,10};

unsigned char table2[8][8] = { 1,1,0,0,0,0,0,0,
	10,1,0,0,0,0,0,0,
	2,3,1,1,1,1,1,0,
	10,4,10,2,1,10,1,0,
	10,4,10,2,1,1,1,0,
	1,3,2,2,0,0,1,1,
	2,3,10,1,0,0,1,10,
10,10,2,1,0,0,1,1};

unsigned char table3[8][8] = {1,1,1,1,1,1,1,10,
	1,10,1,1,10,1,1,1,
	1,1,1,1,1,2,1,1,
	0,0,0,0,0,1,10,1,
	1,1,1,0,0,1,2,2,
	1,10,3,2,2,1,2,10,
	2,3,10,10,2,10,2,1,
10,2,2,2,2,1,1,0};

unsigned char table4[8][8] = {10,2,10,1,0,0,2,10,
	1,3,2,2,1,1,3,10,
	0,1,10,1,1,10,2,1,
	1,2,2,1,1,1,1,0,
	2,10,1,0,0,0,1,1,
	10,2,2,1,1,0,1,10,
	1,1,1,10,1,0,1,1,
0,0,1,1,1,0,0,0};

unsigned char table5[8][8] = {10,1,0,0,1,10,1,0,
	1,2,2,2,2,1,1,0,
	0,1,10,10,1,0,0,0,
	0,1,2,2,1,0,0,0,
	0,0,0,1,1,1,0,0,
	0,0,0,1,10,2,2,2,
	1,1,0,2,2,3,10,10,
10,1,0,1,10,2,3,10};

unsigned char x=0, y=0, status[8][8], game[8][8], level=0, count=0;

void game_initialization()
{
	x = 0;
	y = 0;
	count = 0;
	
	for(unsigned char i =0; i<8; i+=1)
	for(unsigned char j =0; j<8; j+=1)
	status[i][j] = 12;
	
	
	switch (level)
	{
		case 0:
		for(unsigned char i =0; i<8; i+=1)
		for(unsigned char j =0; j<8; j+=1)
		game[i][j] = table1[i][j];
		break;
		case 1:
		for(unsigned char i =0; i<8; i+=1)
		for(unsigned char j =0; j<8; j+=1)
		game[i][j] = table2[i][j];
		break;
		case 2:
		for(unsigned char i =0; i<8; i+=1)
		for(unsigned char j =0; j<8; j+=1)
		game[i][j] = table3[i][j];
		break;
		case 3:
		for(unsigned char i =0; i<8; i+=1)
		for(unsigned char j =0; j<8; j+=1)
		game[i][j] = table4[i][j];
		break;
		case 4:
		for(unsigned char i =0; i<8; i+=1)
		for(unsigned char j =0; j<8; j+=1)
		game[i][j] = table5[i][j];
		break;
	}
	glcd_game_restart();
	set_display(x,y,status[x][y],1);
}


int main(void)
{
	DDRC=DDRD=0xff;
	DDRB = 0;
	glcd_init();
	level = 0;
	game_initialization();
	while (1)
	{
		if (PINB & (1<<3)) // left
		{
			if ( y == 0)
			y = 7;
			else
			y = y-1;
			set_display(x,y,status[x][y],1);
			_delay_ms(20);
		}
		if (PINB & (1<<1)) // right
		{
			y = (y+1)%8;
			set_display(x,y,status[x][y],1);
			_delay_ms(20);
		}
		if (PINB & (1<<2)) // down
		{
			x = (x+1)%8;
			set_display(x,y,status[x][y],1);
			_delay_ms(20);
		}
		if (PINB & (1)) // up
		{
			if ( x == 0)
			x = 7;
			else
			x = x-1;
			set_display(x,y,status[x][y],1);
			_delay_ms(20);
		}
		if (PINB & (1<<4)) // select
		{
		if(status[x][y] == 12)
		{
			unsigned char temp = game[x][y];
			if(temp > 0 && temp < 9)// number
			{
				status[x][y] = temp;
				set_display(x,y,status[x][y],1);
				count++;
			}
			else if( temp == 0) // empty
			{
				status[x][y] = temp;
				count++;
				set_display(x,y,status[x][y],1);
				if(x > 0 && game[x-1][y] != 10 && status[x-1][y] == 12) // x-1
				{
					status[x-1][y] = game[x-1][y];
					set_display(x-1,y,status[x-1][y],0);
					count++;
					//_delay_ms(10);
				}
				if(y > 0 && game[x][y-1] != 10 && status[x][y-1] == 12) // y-1
				{
					status[x][y-1] = game[x][y-1];
					set_display(x,y-1,status[x][y-1],0);
					count++;
					//_delay_ms(10);
				}
				if(x < 7 && game[x+1][y] != 10 && status[x+1][y] == 12) // x+1
				{
					status[x+1][y] = game[x+1][y];
					set_display(x+1,y,status[x+1][y],0);
					count++;
					//_delay_ms(10);
				}
				if(y < 7 && game[x][y+1] != 10 && status[x][y+1] == 12) // y+1
				{
					status[x][y+1] = game[x][y+1];
					set_display(x,y+1,status[x][y+1],0);
					count++;
					//_delay_ms(10);
				}
				if(x > 0 && y > 0 && game[x-1][y-1] != 10 && status[x-1][y-1] == 12) // x-1  y-1
				{
					status[x-1][y-1] = game[x-1][y-1];
					set_display(x-1,y-1,status[x-1][y-1],0);
					count++;
					//_delay_ms(10);
				}
				if(x < 7 && y > 0 && game[x+1][y-1] != 10 && status[x+1][y-1] == 12) // x+1 y-1
				{
					status[x+1][y-1] = game[x+1][y-1];
					set_display(x+1,y-1,status[x+1][y-1],0);
					count++;
					//_delay_ms(10);
				}
				if(x > 0 && y < 7 && game[x-1][y+1] != 10 && status[x-1][y+1] == 12) // x-1  y+1
				{
					status[x-1][y+1] = game[x-1][y+1];
					set_display(x-1,y+1,status[x-1][y+1],0);
					count++;
					//_delay_ms(10);
				}
				if(x < 7 && y < 7 && game[x+1][y+1] != 10 && status[x+1][y+1] == 12) // x+1  y+1
				{
					status[x+1][y+1] = game[x+1][y+1];
					set_display(x+1,y+1,status[x+1][y+1],0);
					count++;
					//_delay_ms(10);
				}
			}
			else if( temp == 10) // lose
			{
				status[x][y] = temp;
				set_display(x,y,status[x][y],1);
				for(unsigned char i =0; i<8; i+=1){
					for(unsigned char j =0; j<8; j+=1)
					{
						if( game[i][j] == 10 && status[i][j] == 12)
						{
							status[i][j] = game[i][j];
							set_display(i,j,status[i][j],0);
						}
					}
				}
				glcd_game_over();
				_delay_ms(60);
				game_initialization();
			}
		}
		_delay_ms(20);
	}
	if (PINB & (1<<5)) // flag
	{
		if(status[x][y] == 12)
		{
			status[x][y] = 11;
			set_display(x,y,status[x][y],1);
			if( game[x][y] == 10)
				count++;
		}
		else if(status[x][y] == 11)
		{
			status[x][y] = 12;
			set_display(x,y,status[x][y],1);
			if( game[x][y] == 10)
				count--;
		}
		_delay_ms(20);
	}
	if(count == 64)  // win
	{
		glcd_victory();
		level = (level+1) % 5;
		_delay_ms(30);
		game_initialization();
	}
}
}


