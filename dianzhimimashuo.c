#include<reg51.h>
#include<intrins.h>
 
#define uchar unsigned char 
#define uint  unsigned int

sbit SCL=P2^0; //时钟线接口 
sbit SDA=P2^1; //数据线接口
sbit D1=P2^2;  //开锁信号
sbit beep=P2^4;//蜂鸣器接口
sbit E=P2^5;   //液晶使能 
sbit RW=P2^6;  //读写控制
sbit RS=P2^7;  //数据命令选择 
sbit k1=P3^2;  //恢复初始密码按键

uchar idata table1[6]={0,8,0,8,0,8}; //初始化密码
uchar dd; 
uchar aa; //存放密码错误的次数，达到三次将报警
uchar bb;
uchar cc;
//延时子函数
void delay1(uint z)
{
	uint x,y;
	for(x=0;x<z;x++)
		for(y=0;y<110;y++);
}
void delay2() 
{
  ;;
}
//总线初始化子函数
void chushi()
{
	SDA=1;
	delay2();
	SCL=1;
	delay2();
}
//启动信号子函数
void start()
{
	SDA=1;
	SCL=1;
	delay2();
	SDA=0;
	delay2();
}
//停止信号子函数
void stop()
{
	SDA=0;
	delay2();
	SCL=1;
	delay2();
	SDA=1;
	delay2();
}
//应答信号子函数
void respons()
{
	uchar i=0;
	SCL=1;
	delay2();
	while(SDA==1&&i<255)//等待应答，过一段时间不应答退出循环
		i++;
	SCL=0;
	delay2();
}
//写一个字节子函数
void writebyte(uchar date)
{
	uchar i,temp;
	temp=date;
	for(i=0;i<8;i++)
	{
		temp<<=1;//temp左移一位后高位进CY
		SCL=0;
		delay2();
		SDA=CY;
		delay2();
		SCL=1;
		delay2();
	}	
	SCL=0;//应答信号中SCL = 1，所以这里要置0
	delay2();
	SDA=1;//用完要释放数据总线
	delay2();
}
//读一个字节子函数
uchar readbyte() 
{
	uchar i,k;
	SCL=0;
	delay2();
	SDA=1;
	for(i=0;i<8;i++)
	{
		SCL=1;	
		delay2();
		k=(k<<1)|SDA; //和最低位或，一位位送到K
		SCL=0;
		delay2();
	}
	delay2();
	return k;
}
//向地址写一个字节数据子函数
void write_add(uchar address,uchar date)
{
	start();
	writebyte(0xa0);//A0,A1,A2接地，AT24C02芯片地址为1010，送控制字为1010A2A1A0R/~W
	respons();
	writebyte(address);
	respons();
	writebyte(date);
	respons();
	stop();
}
//向地址读一个字节数据子函数
uchar read_add(uchar address)
{
	uchar date;
	start();
	writebyte(0xa0); //A0,A1,A2接地，AT24C02芯片地址为1010，送控制字为1010A2A1A0R/~W
	respons();
	writebyte(address);
	respons();
	start();
	writebyte(0xa1); //A0,A1,A2接地，AT24C02芯片地址为1010，送控制字为1010A2A1A0R/~W
	respons();
	date=readbyte();
	stop();
	return date;
}
//向地址写n个字节数据子函数
void write_n_add(uchar *p,uchar address,uchar n)
{
	uchar i;
	for(i=0;i<n;i++)
	{
		write_add((address+i),*(p+i));
		delay1(20);
	}
}
//向地址读n个字节数据子函数
void read_n_add(uchar *p,uchar address,uchar n)
{
	uchar i;
	for(i=0;i<n;i++)
	{
		*(p+i)=read_add(address+i);	
	}
}
//LCD1602液晶读忙检子函数
bit LCD1602_busy()
{                          
    bit a;
    RS=0;
    RW=1;
    E=1;
    _nop_();
    _nop_();
    a=(bit)(P0&0x80);
    E=0;
    return a; 
}
//LCD1602液晶写命令子函数
void write_com(uchar com) 
{
	while(LCD1602_busy());
    RW=0;
	RS=0;
    E=0;
	P0=com;
	delay1(5);
    E=1;
	delay1(5);
    E=0;
}
//LCD1602液晶写数据子函数
void write_date(uchar date)
{
	while(LCD1602_busy());
    RW=0;
  	RS=1;
    E=0;
  	P0=date;
	delay1(5);
    E=1;
	delay1(5);
    E=0;          
}
//写一个字符子函数
void write_1_char(uchar zifu)
{
	write_date(zifu);	
}
//向液晶写n个字符子函数
void write_n_char(uchar zifu[])
{
	uchar i;
	for(i=0;;i++)
	{
		write_1_char(zifu[i]);
		if(zifu[i+1]=='\0')
			break;
	}
}
//设置液晶显示位置子函数
void LCD1602_pos(uchar x,uchar y) 
{
	uchar pos;
	if(x==0)
		x=0x80;
	else if(x==1)
		x=0x80+0x40;
	pos=x+y;
	write_com(pos);
}
//LCD1602液晶初始化子函数
void LCD1602_init()
{
    E=0;
    write_com(0x38);//设置16x2显示，5x7点阵，8位数据口 
    write_com(0x0c);//设置开显示，不显示光标 
    write_com(0x06);//写一个字符后地址指针加1 
	write_com(0x01);//显示清0，数据指针清0         
}
//报警子函数
void baojing1()
{
	int i=0;
	for(i=0;i<5;i++)
	{
		beep=1;
		D1=0;
		delay1(5);
		beep=0;
		D1=1;
		delay1(5);	
	}
}
void baojing2()
{
	uchar i;
	for(i=0;i<10;i++)
	{
		baojing1();
	}
}
//矩阵按键扫描子函数
uchar keyscan()
{
	uchar temp,key;
	key=0xff;
	P1=0xfe; //将第一行线置低电平
	temp=P1;//读JPJK的当前状态到 temp
	temp&=0xf0;// temp = temp & 0xf0 按位与 
	if(temp!=0xf0)
	{
		delay1(10);//延时去抖
		temp=P1;
		temp&=0xf0;
	   	if(temp!=0xf0)//第一行有键被按下
		{
			temp=P1;//读被按下的键
			baojing1();
			switch(temp)
			{
				case 0xee: 	key=1;
							break;
				case 0xde: 	key=2;
							break;
				case 0xbe: 	key=3;
							break;
				case 0x7e: 	key=0x0a;
							break;
			}
			while(temp!=0xf0)//等待按键释放
			{
				temp=P1;
				temp&=0xf0;
			}
		}
	}
	P1=0xfd; //将第二行线置低电平
	temp=P1;//读JPJK的当前状态到 temp
	temp&=0xf0;// temp = temp & 0xf0
	if(temp!=0xf0)
	{
		delay1(10);//延时去抖
		temp=P1;
		temp&=0xf0;
	   	if(temp!=0xf0)//第二行有键被按下
		{
			temp=P1;//读被按下的键
			baojing1();
			switch(temp)
			{
				case 0xed: 	key=4;
							break;
				case 0xdd: 	key=5;
							break;
				case 0xbd: 	key=6;
							break;
				case 0x7d: 	key=0x0b;
							break;
			}
			while(temp!=0xf0)//等待按键释放
			{
				temp=P1;
				temp&=0xf0;
			}
		}
	}
	P1=0xfb; //将第 三行线置低电平
	temp=P1;//读JPJK的当前状态到 temp
	temp&=0xf0;// temp = temp & 0xf0
	if(temp!=0xf0)
	{
		delay1(10);//延时去抖
		temp=P1;
		temp&=0xf0;
	   	if(temp!=0xf0)//第三行有键被按下
		{
			temp=P1;//读被按下的键
			baojing1();
			switch(temp)
			{
				case 0xeb: 	key=7;
							break;
				case 0xdb: 	key=8;
							break;
				case 0xbb: 	key=9;
							break;
				case 0x7b: 	key=0x0c;
							break;
			}
			while(temp!=0xf0)//等待按键释放
			{
				temp=P1;
				temp&=0xf0;
			}
		}
	}
	P1=0xf7; //将第四行线置低电平
	temp=P1;//读JPJK的当前状态到 temp
	temp&=0xf0;// temp = temp & 0xf0
	if(temp!=0xf0)
	{
		delay1(10);//延时去抖
		temp=P1;
		temp&=0xf0;
	   	if(temp!=0xf0)//第四行有键被按下
		{
			temp=P1;//读被按下的键
			baojing1();
			switch(temp)
			{
				case 0xe7: 	key=0;
							break;
				case 0xd7: 	key=0;
							break;
				case 0xb7: 	key=0x0f;
							break;
				case 0x77: 	key=0x0d;
							break;
			}
			while(temp!=0xf0)//等待按键释放
			{
				temp=P1;
				temp&=0xf0;
			}
		}
	}
   	return key; //返回按下的键 
}
//比较密码子函数
bit sfj1(uchar *string1,uchar *string2)
{
	uchar i;
	for(i=0;i<6;i++)
	{
		if(string1[i]!=string2[i])
			return 0;
	}
	return 1;
}
//选择输入密码或修改密码函数
uchar step_choose()
{
	uchar key;
	key=0xff;
	write_com(0x06);//写一个字符后地址指针加 1 
    write_com(0x01);//显示清零，数据指针清零
	LCD1602_pos(0,0);
	write_n_char(" Please Input ");
	LCD1602_pos(1,0);
	write_n_char(" Press key begin  ");	
	while((key!=0x0a)&&(key!=0x0b))
		key=keyscan();	
	return key;
}
//输入密码子函数
bit input_mima(uchar * mima)
{
	uchar i,key;
	LCD1602_pos(1,0);
	for(i=0;i<7;i++)
	{
		delay1(100);
		if(i<6)
		{
			do
			{
				key=keyscan();
			}//扫描键盘
			while(key==0xff);
			if((key!=0x0f)&&(key!=0x0a)&&(key!=0x0c))//不是退格也不是确认键
			{
				write_date('*');//是数字键显示*
				mima[i]=key;
			}
			if(key==0x0f)//是退格键
			{
				if(i>0)
				{
				    LCD1602_pos(1,--i);//光标前移一位
					write_date(' ');//清空一位
					mima[i]=' ';//写空
					LCD1602_pos(1,i);
					i--;//密码计数器减一 ,因为循环后会+1，所以在这里要加1
				}
			}
			if(key==0x0c)//没完成密码输入返回错误信息
			{
				LCD1602_pos(0,0);
				return(0);
			}
		}
		if(i==6)
		{
			do
			{
				key=keyscan();
			}
			while((key!=0x0f)&&(key!=0x0c));
			if(key==0x0f)
			{
				LCD1602_pos(1,--i);
				write_date(' ');
				mima[i]=' ';
				LCD1602_pos(1,i);
				i--;
			}
			if(key==0x0c)//密码位数正确
			{
				return(1);//返回1正确信号
			}
		}
	}	
}
//密码处理子函数
void sfj2() 
{
	uchar key,i;
	uchar idata table2[6]={' ',' ',' ',' ',' ',' '};//存放密码缓冲区
	uchar idata table3[6]={' ',' ',' ',' ',' ',' '};
	uchar idata table4[6]={' ',' ',' ',' ',' ',' '};
	key=step_choose();
	if(key==0x0a)//begin被按下,接收输入密码，处理
	{
		read_n_add(table2,0x00,6);
		write_com(0x06);//写一个字符后地址指针加 1 
		write_com(0x01);//显示清零，数据指针清零
		write_com(0x0f);//显示光标
		LCD1602_pos(0,0);
		write_n_char(" Please Input ");		
		if(input_mima(table3)) //处理输入密码
		{
			if(sfj1(table3,table1) || sfj1(table3,table2)) //密码正确
			{
				LCD1602_pos(0,0);
				write_com(0x0c);
	        	write_com(0x06);//写一个字符后地址指针加 1 
	         	write_com(0x01);//显示清零，数据指针清零
				write_n_char(" password right ");
				aa=0; //清除密码错误次数
				D1=0; //开锁
				beep=1; //响一下
				delay1(400);
				beep=0;
				TR0=1;
				cc=1;
				while(key!=0x0d&&cc) //lock没按下一直开
				{			
					key=keyscan();
				}
				TR0=0;
				D1=1;//lock按下了关锁
			}
			else //密码 不正确
			{
				LCD1602_pos(0,0);
				write_com(0x0c); //关光标
				write_com(0x06);//写一个字符后地址指针加 1 
				write_com(0x01);//显示清零，数据指针清零
				write_n_char(" password wrong");
				delay1(400);
				aa++;
				if(aa==4)
				{
					aa=0;
			    	i=20; //密码不正确报警
					while(i--)
					 	baojing2();
				}
			}
		}
		else //密码没有输入正确或完成
		{
			LCD1602_pos(0,0);
			write_com(0x0c); //关光标
	   		write_com(0x06);//写一个字符后地址指针加 1 
		   	write_com(0x01);//显示清零，数据指针清零
			write_n_char(" password wrong");
			delay1(400);
			aa++;
			if(aa==4)
			{
				aa=0;
		    	i=20; //密码不正确报警
				while(i--)
				 	baojing2();
			}
		}
	}
	if(key==0x0b)//set被按下,修改密码
	{
		read_n_add(table2,0x00,6);
		write_com(0x06);//写一个字符后地址指针加 1 
		write_com(0x01);//显示清零，数据指针清零
		write_com(0x0f);//显示光标
		LCD1602_pos(0,0);
		write_n_char(" input password");		
		write_com(0x0f);//显示光标	
		if(input_mima(table3)) //处理输入密码
		{
			if(sfj1(table3,table1) || sfj1(table3,table2)) //密码正确
			{
				LCD1602_pos(0,0);
				write_com(0x0c);
	      		write_com(0x06);//写一个字符后地址指针加 1 
	      		write_com(0x01);//显示清零，数据指针清零
				write_n_char("password right ");
				aa=0; //清除密码错误次数
				delay1(500);
				dd=1;
				while(dd) //下面开始修改密码
				{
					write_com(0x06);//写一个字符后地址指针加 1 
					write_com(0x01);//显示清零，数据指针清零
					write_com(0x0f);//显示光标
					LCD1602_pos(0,0);
					write_n_char("In new password");
					delay1(500);
					if(input_mima(table3)) //处理输入密码
					{
						LCD1602_pos(0,0);
						write_com(0x0c);
			         	write_com(0x06);//写一个字符后地址指针加 1 
			         	write_com(0x01);//显示清零，数据指针清零
						write_n_char("Input new pass"); //确定新密码
						LCD1602_pos(1,0);
						write_n_char("word again");
						delay1(800);
						LCD1602_pos(0,0);
						write_com(0x0f);//显示光标
			         	write_com(0x06);//写一个字符后地址指针加 1 
			         	write_com(0x01);//显示清零，数据指针清零
						write_n_char("In new password");						
						if(input_mima(table4)) //处理输入密码
						{
							if(sfj1(table3,table4)) //密码正确
							{
								LCD1602_pos(0,0);
								write_com(0x0c);
						      	write_com(0x06);//写一个字符后地址指针加 1 
						      	write_com(0x01);//显示清零，数据指针清零
								write_n_char("password has");
								LCD1602_pos(1,0);
								write_n_char("change already");
								write_n_add(table4,0x00,6); //把修改的密码存进24C02
								delay1(1000);
								dd=0;
							}
							else //密码 不正确
							{
								LCD1602_pos(0,0);
								write_com(0x0c); //关光标
								write_com(0x06);//写一个字符后地址指针加 1 
								write_com(0x01);//显示清零，数据指针清零
								write_n_char("password wrong");
								delay1(600);
								aa++;
								if(aa==4)
								{
									aa=0;
								   	i=20; //3次输入密码不正确报警
									while(i--)
										baojing2();
								}
								dd=0;
							}
						}
						else //密码没有输入正确或完成
						{
							LCD1602_pos(0,0);
							write_com(0x0c); //关光标
						   	write_com(0x06);//写一个字符后地址指针加 1 
							write_com(0x01);//显示清零，数据指针清零
							write_n_char(" password wrong");
							delay1(600);
							aa++;
							if(aa==4)
							{
								aa=0;
							    i=20; //3次输入密码不正确报警
								while(i--)
									baojing2();
							}
						}	
					}
					else //密码没有输入正确或完成
					{
						LCD1602_pos(0,0);
						write_com(0x0c); //关光标
				   		write_com(0x06);//写一个字符后地址指针加 1 
					   	write_com(0x01);//显示清零，数据指针清零
						write_n_char("password wrong");
						delay1(600);
						dd=0;
						aa++;
						if(aa==4)
						{
							aa=0;
					    	i=20; //3次输入密码不正确报警
							while(i--)
							 	baojing2();
						}
					}									
				}
			}
			else //密码 不正确
			{
				LCD1602_pos(0,0);
				write_com(0x0c); //关光标
				write_com(0x06);//写一个字符后地址指针加 1 
				write_com(0x01);//显示清零，数据指针清零
				write_n_char("password wrong");
				delay1(600);
				aa++;
				if(aa==4)
				{
					aa=0;
			    	i=20; //3次输入密码不正确报警
					while(i--)
					 	baojing2();
				}
			}
		}
		else //密码没有输入正确或完成
		{
			LCD1602_pos(0,0);
			write_com(0x0c); //关光标
	   		write_com(0x06);//写一个字符后地址指针加 1 
		   	write_com(0x01);//显示清零，数据指针清零
			write_n_char("password wrong");
			delay1(600);
			aa++;
			if(aa==4)
			{
				aa=0;
		    	i=20; //3次输入密码不正确报警
				while(i--)
				 	baojing2();
			}
		}		
	}
}
//主函数
void main()
{
	LCD1602_init();//调用液晶初始化子函数
	chushi();//调用24C02总线初始化子函数
	beep=0;
	D1=1; //关锁
	TMOD=0x01;//选择定时器0方式1
	EA=1;//打开总中断
	ET0=1;//打开定时器0中断
	EX0=1; //打开外部中断0
	IT0=1;//下降沿触发
	TR0=0;//关闭定时器
	TH0=0x4c;//50ms装初值
	TL0=0x00;
	while(1)
	{
	  	sfj2();//调用密码处理子函数 
	}
}
//外部中断0子函数
void wb0() interrupt 0
{
	delay1(2000);
	if(!k1)
	{
		LCD1602_pos(0,0);
		write_com(0x0c); //关光标
	   	write_com(0x06);//写一个字符后地址指针加 1 
		write_com(0x01);//显示清零，数据指针清零
		chushi();//24C02总线初始化
		write_n_add(table1,0x00,6);
		write_n_char("password renew");
		LCD1602_pos(1,0);
		write_n_char("already");
		delay1(1000);
		LCD1602_pos(0,0);
		write_com(0x0c); //关光标
	   	write_com(0x06);//写一个字符后地址指针加 1 
		write_com(0x01);//显示清零，数据指针清零
		write_n_char(" Please Input ");
	  LCD1602_pos(1,0);
	  write_n_char(" Press key begin  ");
	}
}
//定时器0中断子函数
void t0() interrupt 1 
{
	TH0=0x4c;//50ms定时
	TL0=0x00;
	bb++;
	if(bb==200)//10秒时间到
	{
		bb=0;
		cc=0;
	}
}
				