# MCU-DIY
Some really good featured products with MCU (Single Chip Microcomputer). For example, electric conductivity monitor, capacity monitor, water level monitor ...

///////////////////////////////////////////  
//// 脉冲法测量电导率 /////////////////////  
///////////////////////////////////////////  

原理：  
利用电阻分压测量溶液电阻，为了降低溶液极化效应的影响，这里采用短时脉冲电压来测量。  
电导率(EC)sigma定义为电阻率rho的导数, 在具体的电导池中，可以将和电阻成反比：  
    sigma = K/R  
系数K为电导池常数。根据国家标准，使用KCl的水溶液来作为校正液可确定K值。这样就可通过  
测量电阻来确定电导率sigma。由于sigma受温度影响较大，通常根据和温度的关系，把测量结果  
转换到25摄氏度时的电导率。可以看到在程序中有相应转换公式。  
  
电路图:(排版问题，需下载查看)  
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

  
  
  
  
  
  
  
///////////////////////////////////////////  
////      微电容检测   /////////////////////  
///////////////////////////////////////////  
  
原理：  
根据微小电容充放电的指数规律，通过测量微电容放电到指定电压的时间来检测电容。  
指定电压为单片机内置高精度基准电压，通过单片机自带的比较器来实现起停。  
  
电路图:(排版问题，需下载查看)   
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
  
  
  
  
  
  
  
  
///////////////////////////////////////////  
////     探头式水位检测 /////////////////////  
///////////////////////////////////////////  
  
原理：  
    采用按键检测的办法来检测探头电极是否在水中。  
    探头长时间在溶液中，如果采用直流电检测，复杂的电化学效应会导致电极失效，采用正反  
脉冲来检测水位能防止电极氧化电镀效应等，即使一般的电极也能长时间使用，另外也非常省电，  
即使一节纽扣电池也能使用很久。  
  
电路图:(排版问题，需下载查看)   
本检测方案电路和单片机程序都非常的简易，只需要三个原件：探头(DIY)，单片机(STC8F2K08S2-28I-TSSOP20)，  
指示灯  
  
探测电路:  
----------               ------  
|    P3_6|---------------|探头|===   
|        |               ---|--  
|    P3_7|------------------|  
| 单片机 |  
|        |  
|    P1_6|----\  
|        |     *LED指示灯  
|    P1_7|----/  
----------  
  
单片机正反脉冲做完按键检测之后，根据情况来闪烁指示灯，单片机其他时间都处于掉电模式。  
  
评价：  
    效果很好！  
  
使用方法（本程序）：  
1,接上电源  
2,探头放入水中  
3,一旦探头因缺水导致绝缘，指示灯闪烁  
  
