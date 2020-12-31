#include "include/stc8.h"
#include "include/utility.h"

__sbit __at (0xB5) charge;//P3_5

ulong tm0,tmavg=0;
uint  cc=10;
uint time=0;
void tim0() __interrupt (1) __using (2)
{
    TR0=0;//赋初值时，关闭定时器
    //TH0=0xff;//(65536-10)/256;//赋初值定时
    //TL0=0xf7;//(65536-10)%256;//0.01ms
    TH0=0;TL0=0;//赋初值定时
    TR0=1;//打开定时器

    time++;
}

ulong calavg(uint iitimes)
{
	uint ii=0;
	ulong sum=0;
	while(1){
        charge=1;//充电
        delay_ms(40);

        //cmp_over=0;
        time=0;
        TR0=0;//关闭计数器
        TH0=0;TL0=0;//计数器初始归零
        TR0=1; //开启计数
        charge=0;//放电

        while( CMPCR1 & 0x01 );

        TR0=0; //关闭计数

        if(time==0){
            sum+=TH0*256+TL0;
			ii++;
        }

        //delay_ms(50);

		if(ii==iitimes)break;
    }
	if(ii!=0)
		return sum*100L/ii ;
	else
		return 0xffffffff;
}


void int0(void) __interrupt (0) __using (0)
{
	tmavg=calavg(40);
}

/*
//ulong alltimes;
void CMP_Isr()  __interrupt (21) __using (1)
{
    CMPCR1 &= ~0x40;                            //清中断标志
    if (CMPCR1 & 0x01)  //下降沿中断
    {
		TR0=0; //关闭计数
		
		SBUF = (time>>8);
		while(!TI);TI = 0;
		SBUF = (time&0x00ff);
		while(!TI);TI = 0;
		
		SBUF=TH0;
		while(!TI);TI = 0;
		SBUF=TL0;
		while(!TI);TI = 0;
		//alltimes = time*65536 + (TH0*256+TL0);
		cmp_over=1;
    }
    else //上升沿中断
    {
    }
}
*/

void usart_receive(void) __interrupt (4) __using (1)
{
    if(RI)//串口接收完成
    {
		cc=SBUF;
        RI = 0;//清零
    }
    if(TI)//发送数据完成
    {
        //TI = 0;//清零
    }
}


void main(void)
{
	//设置cpu时钟不分频
    //P_SW2|=0x80;
    //CLKDIV=0x00;  //是默认值的4倍，delay_ms，delay_us之类的应该除以4
    //P_SW2&=~0x80;
	
	EA = 1;//全局中断开

	// 16*(1999+1)/32000 = 1.0s //掉电唤醒定时器时钟频率32KHZ
	//WKTCL = 0xcf;
	//WKTCH = 0x87;//最高位启用唤醒定时器

	//串口
	ES=1;//串口中断开
	PCON &= 0x7c;//PD=0,IDL=0:正常工作; SMOD=0,串口方式1，2，3时，波特率正常(否则波特率加倍)
	SCON = 0x50;//设置串口方式1且允许串口接收
    TMOD = 0x21;//定时器1设定为工作方式2(为串口通讯提供脉冲(必须为定时器1)),同时定时器0设定为工作方式1(通常16位>定时)
    //TH1  = 0xfd;TL1  = 0xfd;//设置波特率9600v TH1=256-(2^SMOD)*11095200/(12*32*9600) ; SMOD=0
    TH1  = 0xdc;TL1  = TH1;//1T单片机设置波特率9600v TH1=256-(2^SMOD)*11095200/(32*9600) ; SMOD=0
    TR1=1;
	//PS=0;//串行口中断优先级定义为高优先级
	AUXR = 0x40;//定时器1不分频，否则12分频;  AUXR用于控制波特率发生器，扩展RAM，定时器（特别是分频以兼容传统51）等

	//INT0
	EX0=1;//外部中断0开
    IT0=1;//P3_2口 0电平触发，1下降沿触发


	//定时器
	//TMOD = 0x21;
	AUXR |= 0x80;//定时器0不分频，否则12分频;
	TH0=0;
    TL0=0;
    ET0=1;//开定时器0中断
    TR0=1;//启动定时器0


	//比较器
	CMPCR2 = 0x00;
	CMPCR2 &= ~0x80;                            //比较器正向输出
	//  CMPCR2 |= 0x80;                             //比较器反向输出
	//CMPCR2 &= ~0x40;                            //禁止0.1us滤波
	  CMPCR2 |= 0x40;                             //使能0.1us滤波
	CMPCR2 &= ~0x3f;                            //比较器结果直接输出
	//CMPCR2 |= 0x10;                             //比较器结果经过16个去抖时钟后输出
	CMPCR1 = 0x00;
	//CMPCR1 |= 0x30;                             //使能比较器边沿中断
	//  CMPCR1 &= ~0x20;                            //禁止比较器上升沿中断
	//  CMPCR1 |= 0x20;                             //使能比较器上升沿中断
	//  CMPCR1 &= ~0x10;                            //禁止比较器下降沿中断
	//  CMPCR1 |= 0x10;                             //使能比较器下降沿中断
	CMPCR1 &= ~0x08;                            //P3.7为CMP+输入脚
	//  CMPCR1 |= 0x08;                             //ADC输入脚为CMP+输入教
	CMPCR1 &= ~0x04;                            //内部参考电压为CMP-输入脚
	//  CMPCR1 |= 0x04;                             //P3.6为CMP-输入脚
	//  CMPCR1 &= ~0x02;                            //禁止比较器输出
	CMPCR1 |= 0x02;                             //使能比较器输出
	CMPCR1 |= 0x80;                             //使能比较器模块
	


	while(1){
		tm0=calavg(cc);
		/*
		if(time==0){
			tm0=TH0*256+TL0;
			if(tm0-tmavg>0x04)P1_7=0; else P1_7=1;
*/

		SBUF = (tm0>>24);
        while(!TI);TI = 0;
        SBUF = (tm0>>16);
        while(!TI);TI = 0;
        SBUF = (tm0>>8);
        while(!TI);TI = 0;
        SBUF = tm0;
        while(!TI);TI = 0;



		SBUF = (cc>>8);
        while(!TI);TI = 0;
        SBUF = cc;
        while(!TI);TI = 0;


		delay_ms(200);

		//PCON=0x02;
	}

}


// readme
///////////////////////////////////////////
////    微电容检测   /////////////////////
///////////////////////////////////////////
/*
原理：
根据微小电容充放电的指数规律，通过测量微电容放电到指定电压的时间来检测电容。
指定电压为单片机内置高精度基准电压，通过单片机自带的比较器来实现起停。

电路图:
本微电容检测外围电路非常简单: 1,带比较器的单片机(STC8F2K08S2-28I-TSSOP20)，
2,高阻值电阻如20M，2,电容感应片(DIY)，3,指示灯；
----------    ----------                 --------
|    P3_5|----|高阻电阻|---C-------------|感应片|
| 单片机 |    ----------   |             --------
|     CMP|------------------
|        |
----------

充电: P3_5=1;
放电: P3_5=0;  开启定时器计数直到触发比较器

评价：极简电路即实现微小电容变化的检测，对各种触控，检测，水位等都可用，缺点是
检测距离较短，数据稳定性和市场上的成品比较似乎稍弱。

使用方法（本程序）：
1,接入3v电源
2,触发中断0来启动calavg设定基准电容量
3,一旦发现电容大于基准电容量，指示灯熄灭，否则闪烁

*/
/////////////////////////////////////////////


