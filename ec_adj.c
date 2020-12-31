#include "include/stc8.h"
#include "include/utility.h"
//测试工作频率为11.0592MHz

//溶液电导率温度补偿：k25 = kt / ( 1 + a*(t-25) )    ; 一般 a =0.02， 酸0.016,碱0.019,盐0.024
//具体测量发现：先正向再反向测量get_ratio，与先反向再正向测量get_ratio_r，结果是不同的！对于电导率大的溶液get_ratio测量的结果比get_ratio_r大！推测可能是因为第一次加电压与第二次加电压对应的溶液极化状态不同或者电极原因，第一次加电压对应的电阻相对小，如果认为这个更小的电阻才体现溶液电导率，那么 Rx/R = n * (1-p)/(1+n) ; p为get_ratio测量的Rx/R，n为get_ratio_r测量的Rx/R。  这么做的缺点，好像结果稳定性稍差

#define     ADCTIM      (*(unsigned char volatile __xdata *)0xfea8)

ulong Res1=0;
ulong Res2=0;
//int Vi=0;
ulong ratio=0;
ulong ratiotmp=0xffffffff;
ulong ratio_p=0;
ulong ratio_n=0;
ulong ec=0;
uint temp=250;//1位小数
//uchar ff=1;
//uint rff=0;
uchar ii=0;

void get_ratio(uint tm)
{
	ratiotmp=0xffffffff;

	ADC_CONTR = 0x80;                           //使能ADC模块


	//正向
	P3_6=0;
	P3_7=1;
	delay_us(tm);

	ADC_CONTR |= 0x40;                      //启动AD转换
	_nop_();
	_nop_();
	while (!(ADC_CONTR & 0x20));            //查询ADC完成标志
	ADC_CONTR &= ~0x20;                     //清完成标志
	//ADC_CONTR &= ~0x40;                     //关闭AD转换
	
	Res1=(ADC_RES<<8)|ADC_RESL;
	//Vi = (int)(5000L*Res/1023L);   //单位mV

	P3_7=0;


	//反向
	P3_7=0;
	P3_6=1;
	delay_us(tm);

	ADC_CONTR |= 0x40;                      //启动AD转换
	_nop_();
	_nop_();
	while (!(ADC_CONTR & 0x20));            //查询ADC完成标志
	ADC_CONTR &= ~0x20;                     //清完成标志
	//ADC_CONTR &= ~0x40;                     //关闭AD转换

	Res2=(ADC_RES<<8)|ADC_RESL;
	//Vi = (int)(5000L*Res/1023L);   //单位mV
	//if(Res)ratiotmp = (ulong)((ADC_RES<<8)|ADC_RESL)*1000L/Res;
	if(Res1)ratiotmp = (Res2*10000L)/Res1;
	//ratiotmp=Res;
	//ratiotmp=(ratiotmp<<16)|((ADC_RES<<8)|ADC_RESL);

	P3_6=0;


	ADC_CONTR &= ~0x80;                     //关闭ADC模块

	//return ratiotmp;
}


void get_ratio_r(uint tm)
{
	ratiotmp=0xffffffff;

	ADC_CONTR = 0x80;                           //使能ADC模块



	//反向
	P3_7=0;
	P3_6=1;
	delay_us(tm);

	ADC_CONTR |= 0x40;                      //启动AD转换
	_nop_();
	_nop_();
	while (!(ADC_CONTR & 0x20));            //查询ADC完成标志
	ADC_CONTR &= ~0x20;                     //清完成标志
	//ADC_CONTR &= ~0x40;                     //关闭AD转换

	Res2=(ADC_RES<<8)|ADC_RESL;
	//Vi = (int)(5000L*Res/1023L);   //单位mV
	//if(Res)ratiotmp = (ulong)((ADC_RES<<8)|ADC_RESL)*1000L/Res;
	//ratiotmp=Res;
	//ratiotmp=(ratiotmp<<16)|((ADC_RES<<8)|ADC_RESL);

	P3_6=0;


	//正向
	P3_6=0;
	P3_7=1;
	delay_us(tm);

	ADC_CONTR |= 0x40;                      //启动AD转换
	_nop_();
	_nop_();
	while (!(ADC_CONTR & 0x20));            //查询ADC完成标志
	ADC_CONTR &= ~0x20;                     //清完成标志
	//ADC_CONTR &= ~0x40;                     //关闭AD转换
	
	Res1=(ADC_RES<<8)|ADC_RESL;
	//Vi = (int)(5000L*Res/1023L);   //单位mV
	if(Res1)ratiotmp = (Res2*10000L)/Res1;

	P3_7=0;




	ADC_CONTR &= ~0x80;                     //关闭ADC模块

	//return ratiotmp;
}

void usart_receive(void) __interrupt (4) __using (1)
{
    if(RI)//串口接收完成
    {
        temp=SBUF;
        RI = 0;//清零
    }
    if(TI)//发送数据完成
    {
        //TI = 0;//清零
    }
}


void main()
{
	//设置cpu时钟不分频
    P_SW2|=0x80;
    CLKDIV=0x00;  //是默认值的4倍，delay_ms，delay_us之类的应该除以4 ?
    P_SW2&=~0x80;

	//stc8h系列默认高阻态，最好开始都重置成通常的准双向口便于编程
	P0M0 = 0x00;
	P0M1 = 0x00;
	P1M0 = 0x00;
	P1M1 = 0x00;
	P2M0 = 0x00;
	P2M1 = 0x00;
	P3M0 = 0x00;
	P3M1 = 0x00;
	P4M0 = 0x00;
	P4M1 = 0x00;
	P5M0 = 0x00;
	P5M1 = 0x00;
	P1M0 = 0x00;


	EA = 1;//全局中断开

    //串口
    ES=1;//串口中断开
    PCON &= 0x7c;//PD=0,IDL=0:正常工作; SMOD=0,串口方式1，2，3时，波特率正常(否则波特率加倍)
    SCON = 0x50;//设置串口方式1且允许串口接收
    TMOD = 0x21;//定时器1设定为工作方式2(为串口通讯提供脉冲(必须为定时器1)),同时定时器0设定为工作方式1(通常16位>定时)
    //TH1  = 0xfd;TL1  = 0xfd;//设置波特率9600v TH1=256-(2^SMOD)*11059200/(12*32*9600) ; SMOD=0
    TH1  = 0xdc;TL1  = TH1;//1T单片机设置波特率9600v TH1=256-(2^SMOD)*11059200/(32*9600) ; SMOD=0
    TR1=1;
    //PS=0;//串行口中断优先级定义为高优先级
    AUXR = 0x40;//定时器1不分频，否则12分频;  AUXR用于控制波特率发生器，扩展RAM，定时器（特别是分频以兼容传统51>）等

	//ADC
	P1M1 = 0x01; P1M0 = 0x00;                   //设置P1_0高阻模式 (作为ADC口,同时ADC_CONTR后四位=0)
	P3M1 = 0x00; P3M0 = 0xc0;                   //P3_6,P3_7强推挽输出

	P_SW2|=0x80;
	ADCTIM=0x0a;
	P_SW2 &= 0x7f;
	ADCCFG = 0x20;                              //设置ADC转换频率; 结果右对齐


	//ff=0;
	while (1)
	{
		//动态脉冲法: 找达到稳定状态的最短时间(如15us)，这时可忽略电极间电容影响,极化效应一般还很弱，然后进一步逐步降低电压再找稳定状态，将进一步减少极化效应的影响
		//估计是因为AD转换的时间和电极间电容充放电时间差不多，导致在本实验中看不到电极间电容的影响！
		//因此我们直接取等待稳定时间为0, 按照原理delay_us(tm)应该直接注释掉，CPU不分频，直接取最大24M
		//即存在问题：脉冲时间越小电阻越小，直到电容效应比较显著，但是一直没有看到电容效应，一直都是极化效应的貌似指数函数 (尽管和买的95元的韩国产EC计测量结果几乎完全一致)
		//rff=ff;
		//get_ratio(rff); 
		//ff++;
		//if(ff==100)ff=0;
		//get_ratio(0); 
		
		get_ratio(0);
		ratio_p=ratiotmp;
		delay_ms(120);
		get_ratio_r(0);
		ratio_n=ratiotmp;

		ratio = ratio_n * (10000L-ratio_p)/(10000L-ratio_n);

		//转化为电导率 ec = formfactor / ( ratio*0.1*(1.0 + 补偿因子*(temperature-25) )  ) ; 用其他EC计获得formfactor
		ec = 65091131.0622 / ( ratio*0.1*(1.0 + 0.02*(temp/10.0 - 25.0) )  ); //多乘以100来保留2位小数

		SBUF = (temp>>8);
        while(!TI);TI = 0;
        SBUF = temp;
        while(!TI);TI = 0;

		SBUF = (ec>>24);
        while(!TI);TI = 0;
		SBUF = (ec>>16);
        while(!TI);TI = 0;
		SBUF = (ec>>8);
        while(!TI);TI = 0;
        SBUF = ec;
        while(!TI);TI = 0;


		delay_ms(100);
	}

/*
    while (1)
    {
        ADC_CONTR = 0x80;                           //使能ADC模块
        ADC_CONTR |= 0x40;                      //启动AD转换
        _nop_();
        _nop_();
        while (!(ADC_CONTR & 0x20));            //查询ADC完成标志
        ADC_CONTR &= ~0x20;                     //清完成标志
        ADC_CONTR &= ~0x40;                     //关闭AD转换
        ADC_CONTR &= ~0x80;                     //关闭ADC模块

        Res=(ADC_RES<<8)|ADC_RESL;

        Vi = (int)(5000L*Res/1023L);   //单位mV


        SBUF = (Vi>>8);
        while(!TI);TI = 0;
        //SBUF = (Vi&0x00ff);
        SBUF = Vi;
        while(!TI);TI = 0;

        delay_ms(500);
    }
	*/

}



// readme
///////////////////////////////////////////
//// 脉冲法测量电导率 /////////////////////
///////////////////////////////////////////
/*
原理：
利用电阻分压测量溶液电阻，为了降低溶液极化效应的影响，这里采用短时脉冲电压来测量。
电导率(EC)sigma定义为电阻率rho的导数, 在具体的电导池中，可以将和电阻成反比：
    sigma = K/R
系数K为电导池常数。根据国家标准，使用KCl的水溶液来作为校正液可确定K值。这样就可通过
测量电阻来确定电导率sigma。由于sigma受温度影响较大，通常根据和温度的关系，把测量结果
转换到25摄氏度时的电导率。可以看到在程序中有相应转换公式。


电路图:
本电导率测量方案电路非常简单，一个高精度电阻1K（温飘一般可忽略），一个带AD的单片机
即可(STC8H1K08-36I-TSSOP20)；
输出暂时用串口(接收程序ec.py)。

分压电路:
            -------          -------
 P3_6 ------|R=1K |----------| Rx  |-------- P3_7
            -------     |    -------
 P1_0(ADC)--------------|

P3_6，P3_7 需要设置推挽输出；ADC端口P1_0需要设置高阻输入
正向电压输出: P3_6=0; P3_7=U, 这时P1_0测得P1_0=U1
反向电压输出: P3_6=U; P3_7=0, 这时P1_0测得P1_0=U2
得到不依赖电压U的电阻测量结果：
   U2/U1 = Rx/R   =>  Rx = R * (U2/U1)


----------    ----------                 ------
|    P3_6|----|分压电阻|---C-------------|探头|=== [溶液(Rx)]
| 单片机 |    ----------   |             ---|--
|P1_0(AD)|------------------                |
|        |                                  |
|    P3_7|-----------------------------------
----------

探头需要加装开口的塑料壳子，这样形成局部稳定的电导池让测量更稳定

评价：
效果非常好！看起来误差完全可控制在10%以内！后面准备用来做水培的EC实时检测；

使用方法（本程序）：
1,接上电源(电源电压任意，只要能带动单片机)
2,需要串口接电脑，用ec.py来查看EC。
关于输出，如果做成EC计那就需要加显示模块，如果需要在线监控，那就需要加装无线模块

*/
/////////////////////////////////////////////
