/**************************************************************************************
*		              DS1302时钟实验												  *
实现现象：下载程序后，数码管显示时钟数据
注意事项：																				  
***************************************************************************************/

#include "reg52.h"			 //此文件中定义了单片机的一些特殊功能寄存器
#include "stdio.h"
#include"ds1302.h"	

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

u8 numb = 0;
u8 nsum = 0; //设定值个数计数器
u8 *jieshou;
u8 b[21];	



struct DingShi
{
	u8 sec;
	u8 min;
	u8 hour;
	u8 KG;
} DS[9],*zuhe,*zhixing;


sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;
sbit beep=P1^5;
sbit led1 = P2^0;	 


char num=0;
u8 DisplayData[8];
u8 code smgduan[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
*******************************************************************************/
void delay(u16 i)
{
	while(i--);	
}


/*******************************************************************************
* 函 数 名         : datapros()
* 函数功能		   : 时间读取处理转换函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void datapros() 	 
{
   	Ds1302ReadTime();
	DisplayData[0] = smgduan[TIME[2]/16];				//时
	DisplayData[1] = smgduan[TIME[2]&0x0f];				 
	DisplayData[2] = 0x40;
	DisplayData[3] = smgduan[TIME[1]/16];				//分
	DisplayData[4] = smgduan[TIME[1]&0x0f];	
	DisplayData[5] = 0x40;
	DisplayData[6] = smgduan[TIME[0]/16];				//秒
	DisplayData[7] = smgduan[TIME[0]&0x0f];
}


/*******************************************************************************
* 函数名         :DigDisplay()
* 函数功能		 :数码管显示函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void DigDisplay()
{
	u8 i;

	for(i=0;i<8;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;//显示第0位
			case(1):
				LSA=1;LSB=0;LSC=0; break;//显示第1位
			case(2):
				LSA=0;LSB=1;LSC=0; break;//显示第2位
			case(3):
				LSA=1;LSB=1;LSC=0; break;//显示第3位
			case(4):
				LSA=0;LSB=0;LSC=1; break;//显示第4位
			case(5):
				LSA=1;LSB=0;LSC=1; break;//显示第5位
			case(6):
				LSA=0;LSB=1;LSC=1; break;//显示第6位
			case(7):
				LSA=1;LSB=1;LSC=1; break;//显示第7位	
		}
		P0=DisplayData[7-i];//发送数据
		delay(100); //间隔一段时间扫描	
		P0=0x00;//消隐
	}		
}
/*******************************************************************************
* 函数名         :USartsInit()
* 函数功能		 :串口中断初始化
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void Usarts()
{
	TMOD |= 0x20; //8位自动重装，GATE=0通过TCON的TR1开启定时器1TF1标志定时中断申请
	TH1 = 0xfd;
	TL1 = 0xfd; //用定时器的溢出脉冲设定波特率
	TR1 = 1;//打开定时器1
	SCON = 0x50;//工作方式10位异步收发，REN允许串口接收数据
	EA = 1;
	ES = 1;
	ET1 = 0;
}
/*******************************************************************************
* 函数名         :paixu()
* 函数功能		 :排序
* 输入           : q
* 输出         	 : 无
*******************************************************************************/	
void paixu(u8 q)	
{
    
	u8 i ;
	u8 h,m,s,k;
	

	h = DS[nsum-1].hour;
    m = DS[nsum-1].min;
    s = DS[nsum-1].sec;
    k = DS[nsum-1].KG;
		
    for(i=q;i<nsum-1;i++)
	{
		   DS[nsum-i].hour = DS[nsum-i-1].hour;
		   DS[nsum-i].min = DS[nsum-i-1].min;
		   DS[nsum-i].sec = DS[nsum-i-1].sec;
		   DS[nsum-i].KG = DS[nsum-i-1].KG;	    
	} 
	DS[i].hour = h;
    DS[i].min = m;
	DS[i].sec = s;
    DS[i].KG = k;
}

/*******************************************************************************
* 函数名         :charu()
* 函数功能		 :插入 ：先比较原来设定值再选择插入值的位置再调用排序函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void charu()
{
	u8 i;
    for(i=0;i<nsum-1;i++)
	{
		if (DS[nsum-1].hour < DS[i].hour)
		    paixu(i);
	        break;
		if (DS[nsum-1].hour > DS[i].hour)
		        continue;
		if  ((DS[nsum-1].hour == DS[i].hour) & (DS[nsum-1].min < DS[i].min))
		     paixu(i);
	         break;
	 	if  ((DS[nsum-1].hour == DS[i].hour) & (DS[nsum-1].min > DS[i].min))
		          continue;
		if	 ((DS[nsum-1].hour == DS[i].hour) & (DS[nsum-1].min == DS[i].min) & (DS[nsum-1].sec < DS[i].sec))
			  paixu(i);
	          break;
		if	 ((DS[nsum-1].hour == DS[i].hour) & (DS[nsum-1].min == DS[i].min) & (DS[nsum-1].sec > DS[i].sec))
			      continue;
		if	 ((DS[nsum-1].hour == DS[i].hour) & (DS[nsum-1].min == DS[i].min) & (DS[nsum-1].sec == DS[i].sec))
	         {
			  printf("与前值设定重复");
			  break;
			 }		  	  		 
	}
}
/*******************************************************************************
* 函数名         :pointinit()
* 函数功能		 :执行指针初始化
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void pointinit()
{
  u8 i;
  for(i=0;i<nsum;i++)
  {
  	if (TIME[2] < (*zhixing).hour)
	    break;
  	if (TIME[2] > (*zhixing).hour)
	    zhixing++;
		continue;
	if	((TIME[2] == (*zhixing).hour) & (TIME[1]<(*zhixing).min) )
	     break;
	if  ((TIME[2] == (*zhixing).hour) & (TIME[1]>(*zhixing).min) )
		 zhixing++;
		 continue;
	if ((TIME[2] == (*zhixing).hour) & (TIME[1]==(*zhixing).min) & (TIME[0]<(*zhixing).sec) )	  
		break;
	if ((TIME[2] == (*zhixing).hour) & (TIME[1]==(*zhixing).min) & (TIME[0]>(*zhixing).sec) )
		 zhixing++;		   
  }
  if (i == nsum)
     zhixing = DS;//经过检测现在系统时间大于设定时间没有可执行的任务
}
/*******************************************************************************
* 函数名         :delet()
* 函数功能		 :删除一个设定值
* 输入           : i(删除第几行的设定值）
* 输出         	 : 无
*******************************************************************************/
void delet(u8 q)
{
   	u8 i;

   for(i=q;i<nsum-1;i++)
	{
		   DS[i].hour = DS[i+1].hour;
		   DS[i].min = DS[i+1].min;
		   DS[i].sec = DS[i+1].sec;
		   DS[i].KG = DS[i+1].KG;	    
	} 
	 
	nsum--;
	zuhe--;
	pointinit();
		
}

void main()
{
	 Ds1302Init();

	 jieshou = b;
	 zuhe = DS;
	 zhixing = DS;

	if (nsum>1)
	pointinit();//上电执行指初始化

	while(1)
	{
	   datapros();	 //数据处理函数
	   DigDisplay();//数码管显示函数
	   Usarts();    //串口中断初始化
	   if ((TIME[2] == (*zhixing).hour) & (TIME[1]==(*zhixing).min) & (TIME[0]==(*zhixing).sec) )
		   led1 = (*zhixing).KG;
		   zhixing++;
		if (zhixing == (DS+nsum))//执行完最后一个设定值了
		zhixing = DS;
           
		  
	}			
}
/*******************************************************************************
* 函数名         :USart()
* 函数功能		 :串口中断
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void Usart() interrupt 4
{
	  
	  
		//break;//退出接收数据的程序  删除一个zuhe--
	  *jieshou = SBUF;	  //PN指向定时动作结构体数组DS[]  jieshou指向普通数组b[]
	  
	  RI = 0;
	  numb++;

     if(numb%4==0)
	  {
	    if (zuhe == (DS+8))
	     printf("定时设定值已经存满请删除一个再设定");
	    (*zuhe).hour = *(jieshou-3);
		(*zuhe).min =  *(jieshou-2);
		(*zuhe).sec =  *(jieshou-1);
		(*zuhe).KG  =  *jieshou;

		zuhe++;	
		nsum++;
		   
		if (nsum >= 2)
		   {
		     charu();
		     pointinit();
			}
	  } 
	  
	  if(jieshou == b[19])	 
	     jieshou = b;		//指针到了数组尾部后再移到数组头部
	  else
	     jieshou++;

	if (zuhe == (DS+8))
	     printf("定时设定值已经存满请删除一个再设定");
	 //while(!TI);
	 //TI = 0;
}
