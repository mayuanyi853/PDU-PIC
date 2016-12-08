
#define THIS_IS_STACK_APPLICATION

// Include all headers for any enabled TCPIP Stack functions
#include "TCPIP Stack/TCPIP.h"

// Include functions specific to this stack application
#include "MainDemo.h"

// Declare AppConfig structure and some other supporting stack variables
DS1302_VAL DS1302REG;				//added by beyond //Mayee；DS1302时钟结构体变量

#if defined(STACK_USE_BUZZER)    //CCT这是一个定时器3的相关应用，在这里没有用到，先保留下来以后用
BOOL b_RF315_finish;
BOOL b_RF315Key_Press;
BOOL b_TMeasure;
BOOL b_Flash500ms;
BYTE T_100ms_Acc;
BYTE T_1s_Acc;
BYTE T_3s_Acc;
BOOL b_3s;
#endif

//BOOL b_NewMessage=0;			//没被引用 ？
//BOOL b_JK;					//没被引用 ？
TICK t3;						//typedef DWORD TICK;  \Microchip\Include\TCPIP Stack\Tick.h:59: Double Word 的缩写双字节
//BYTE Temp1[8]={0};

APP_CONFIG AppConfig;			 //自动获取IP地址
BYTE AN0String[8];
BYTE myDHCPBindCount = 0xFF;   //什么变量？
#if !defined(STACK_USE_DHCP_CLIENT)
	#define DHCPBindCount	(1)
#endif
BYTE TCPSendTempHumi[10];       //CCT用于发送TCP的温湿度
BYTE PostIpVar[3][16];          //CCT ip 地址定义
BYTE UserPwdVar[3][16];         //CCT 用户名密码
BYTE webOperate;				//CCT WEB操作是否成功
BYTE AVs[12][4];                //cct12路电流电压实时数据数组
BYTE relaySet[13]={'0','0','0','0','0','0','0','0','0','0','0','0',2};      //cct当前下发继电器状态
BYTE oldRelay[12]={'0','0','0','0','0','0','0','0','0','0','0','0'};        //CCT保存的继电器状态
///玩具区//////////////////////////////////////////////////////////////////////////////总电流，总电压
BYTE testV[6]={'0','0','0','0','0','0'}; 			//格式{'2','3','0','.','4',0x00} 电压
BYTE shareA[6]={'0','0','0','0','0','0'};			//格式{' ','0','.','0','0',0x00} 电流
BYTE DataArray1[3]={'0','0','0'};					// mayee 终于让我猜到它是干什么的了，通道状态数组[0]:有负载的通道数[1]:导通通道数[2]:关闭通道数，王震只做了12路的，没做6路的，

/////////// 与服务器链接计数器，无连接时递减至0
BYTE NET_Link = 30; //无web中心连接是--至0
BYTE NET_Link_MAC = 0x00; //网络没物理连接是为5 --至0 ，有物理连接时为0
BYTE NET_MAC_TEMP = 0x01;

extern BOOL b_RS485_Success;

 
// Private helper functions.
// These may or may not be present in all applications.
static void InitAppConfig(void);	//初始化网络
static void InitializeBoard(void);	//初始化板子函数
static void ProcessIO(void);     	//什么函数
void initE2promData(void);

#if defined(__18CXX)
	#pragma interruptlow LowISR  //将函数LowISR声明为低优先级的中断服务程序。
	void LowISR(void)
	{
 		RS485_RC_ISR();
	}	
	#pragma interruptlow HighISR  //将函数HighISR声明为低优先级的中断服务程序。
	void HighISR(void)
	{
	    TickUpdate();				//??

		#if defined(STACK_USE_BUZZER)
		Timer3_ISR();             //???
		#endif
	}
	
	#pragma code lowVector=0x18
	void LowVector(void){_asm goto LowISR _endasm}
	#pragma code highVector=0x8
	void HighVector(void){_asm goto HighISR _endasm}
	#pragma code // Return to default code section
#endif


// Main application entry point.

void main(void)
{
    static TICK t ; 	//typedef DWORD TICK; 双字节
    static TICK t1 ;
    static TICK t2 ;
    BYTE relaySet_x;
    // Initialize application specific hardware
    InitializeBoard();
	
	SPK_IO=0;
//	BuildDateSave();					//保存编译日期，并记录编译次数
	LCDInit();							//LCD初始化
	DelayMs(100);						//延时
#if defined(POWER_SUPPLY_12)
	memset(LCDText,' ',64);
	strcpypgm2ram((char*)LCDText, "北京网玉金石科技""有限公司        ""TEL:010-82871433");//CCT启动第一屏显示
	LCDUpdate(3);
#elif defined(POWER_SUPPLY_6) || defined(POWER_SUPPLY_12_1602)
	memset(LCDText,' ',32);
	strcpypgm2ram((char*)LCDText, "www.wanstone.com""TEL:010-82871433");//CCT启动第一屏显示
	LCDUpdate(2);
#endif


	// Initialize stack-related hardware components that may be 
	// required by the UART configuration routines
    TickInit();
	#if defined(STACK_USE_MPFS) || defined(STACK_USE_MPFS2)
	MPFSInit();
	#endif

	// Initialize Stack and application related NV variables into AppConfig.
	InitAppConfig();					//设置IP地址

   
	// Initialize core stack layers (MAC, ARP, TCP, UDP) and
	// application modules (HTTP, SNMP, etc.)
    StackInit();


	// Initialize any application-specific modules or functions/
	// For this demo application, this only includes the
	// UART 2 TCP Bridge
	#if defined(STACK_USE_UART2TCP_BRIDGE)
	UART2TCPBridgeInit();
	#endif
	

	RS485_Init();
    initE2promData();
///////////////////////////////////////////////////////////////////////////////////CCT设备存储单元初始化
	memset(relaySet,'0',13);                           //CCT读继电器当前状态
	XEEBeginRead(0x5400);							
	for(relaySet_x=0;relaySet_x<12;relaySet_x++)
		relaySet[relaySet_x]=XEERead();
	XEEEndRead();
    relaySet[12]=2;   
 
	memset(TCPSendTempHumi,0,sizeof(TCPSendTempHumi));
    memset(AVs,0,sizeof(AVs));

////////////////////////////////////////////////////////////////////////////////////////////////////////
	t = TickGet();
	t1 = TickGet();
    t2 = TickGet();
	t3 = TickGet();
                                                        //CCT初始化已经完成，从这里开始进行正常循环，尽量把自己写的函数放在程序的末尾
                                                        //   大的程序应用尽量用状态器形式，简化为无延时程序
	while(1)
    {
  		if(TickGet() - t2 >= 3*TICK_SECOND)
		{
            t2 = TickGet();
			TmTsk();									//CCT把时间处理放在这里，是为了分屏显示数据。
			RelayOnOffCount();	
	}

	 	if(TickGet() - t1 >= 1*TICK_SECOND)
		{ 
			t1 = TickGet();           
			RS485_SendRequest();						//CCT这行主要是调用485连接的两个12C5608.
			if((relaySet[12]==1)||(relaySet[12]==2))    //CCT检查是否有从DELPHI7发来的新继电器开关数据，=1代表没有处理完，=2代表收到新的信息
				RelayControl();			                //CCT处理开关继电器
		}
	
	    if(TickGet() - t >= 1*TICK_SECOND)
        {
            t = TickGet();                              
            LED_RUN_IO ^= 1;                            //CCT系统正常运行指示灯
			GetTPHI();                                  //CCT截取温湿度
			if(NET_Link_MAC >0)	
				NET_Link_MAC--;
			if(NET_Link_MAC == 0x02)
				AnnounceIP();
			if(NET_Link_MAC == 0x01)
				AnnounceIP();
			if(NET_Link > 0)
				NET_Link--;
        }
                                                        //CCT单片机一周期600-700uS
		 if(b_RS485_Success == 1)
		{
			RS485_Function();                           //CCT处理485返回的数据
			ApSum();									//CCT计算电流总合
		}

        if(BUTTON0_IO == 0u)
		{
			SPK_IO=0;               	                //CCT关闭凤鸣器
			AppConfig.MyIPAddr.Val=	AppConfig.DefaultIPAddr.Val;
			AppConfig.MyMask.Val  = AppConfig.DefaultMask.Val;
			SaveAppConfig();
		//	XEEBeginWrite(0x3000);
			XEEBeginWrite(0x3800);//mayee 20140218 12路web和6路web差0.6k页面冲突
    	//	XEEWrite(0x00);
			XEEWrite('a');
			XEEWrite('d');
			XEEWrite('m');
			XEEWrite('i');
			XEEWrite('n');
			XEEWrite(0x00);
   			XEEEndWrite();
	    }


		
        // This task performs normal stack task including checking
        // for incoming packet, type of packet and calling
        // appropriate stack entity to process it.
        StackTask();
        
        // This tasks invokes each of the core stack application tasks
        StackApplications();

		// Process application specific tasks here.
		// For this demo app, this will include the Generic TCP 
		// client and servers, and the SNMP, Ping, and SNMP Trap
		// demos.  Following that, we will process any IO from
		// the inputs on the board itself.
		// Any custom modules or processing you need to do should
		// go here.
		#if defined(STACK_USE_GENERIC_TCP_CLIENT_EXAMPLE)
		GenericTCPClient();
		#endif
		
		#if defined(STACK_USE_GENERIC_TCP_SERVER_EXAMPLE)
		GenericTCPServer();
		#endif
		
		#if defined(STACK_USE_SMTP_CLIENT)
		SMTPDemo();
		#endif
		
		#if defined(STACK_USE_ICMP_CLIENT)
		PingCheck();							     	//cct调用PING函数	
		#endif
		
		#if defined(STACK_USE_SNMP_SERVER) && !defined(SNMP_TRAP_DISABLED)
		SNMPTrapDemo();
		#endif
		
		#if defined(STACK_USE_BERKELEY_API)
//      	BerkeleyCommunication();                         //CCT 外围网络通信，这个函数写的比较NB，特别有套路，不管是从理论，还是从实践能写出这样代码的我相信不是一班人，他是14班的														 
		#endif

	//	ProcessIO();

        // If the DHCP lease has changed recently, write the new
        // IP address to the LCD display, UART, and Announce service
		NET_MAC_TEMP = MACIsLinked();
		if(NET_MAC_TEMP == 0x00)	//mayee网络插拔测试MACIsLinked()函数头拔出后确实可以返回0x00 20130713
			NET_Link_MAC = 0x7;
		if(DHCPBindCount != myDHCPBindCount)
		{
			myDHCPBindCount = DHCPBindCount;

			#if defined(STACK_USE_UART)
				putrsUART((ROM char*)"\r\nNew IP Address: ");
			#endif

	//		DisplayIPValue(AppConfig.MyIPAddr);

			#if defined(STACK_USE_ANNOUNCE)
				AnnounceIP();
			#endif
		}

	}
}

// Writes an IP address to the LCD display and the UART as available
void DisplayIPValue(IP_ADDR IPVal)
{
    BYTE IPDigit[4];
	BYTE i;
#ifdef USE_LCD
	BYTE j;
	BYTE LCDPos=16;
#endif

	for(i = 0; i < sizeof(IP_ADDR); i++)
	{
	    uitoa((WORD)IPVal.v[i], IPDigit);

		#if defined(STACK_USE_UART)
			putsUART(IPDigit);
		#endif

		#ifdef USE_LCD
			for(j = 0; j < strlen((char*)IPDigit); j++)
			{
				LCDText[LCDPos++] = IPDigit[j];
			}
			if(i == sizeof(IP_ADDR)-1)
				break;
			LCDText[LCDPos++] = '.';
		#else
			if(i == sizeof(IP_ADDR)-1)
				break;
		#endif

		#if defined(STACK_USE_UART)
			while(BusyUART());
			WriteUART('.');
		#endif
	}

}

// Processes A/D data from the potentiometer
static void ProcessIO(void)///////////////////////////////////////////////////CCT	目前这块没有什么用，程序中没有应用到A/D
{

    // AN0 should already be set up as an analog input
    ADCON0bits.GO = 1;

    // Wait until A/D conversion is done
    while(ADCON0bits.GO);

    // Convert 10-bit value into ASCII string
    uitoa(*((WORD*)(&ADRESL)), AN0String);

}



/*********************************************************************
 * Function:        void InitializeBoard(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initialize board specific hardware.
 *
 * Note:            None
 ********************************************************************/
static void InitializeBoard(void)
{	
	                                        //CCT已更正
	RELAY1_TRIS = 0;
	RELAY2_TRIS = 0;
	RELAY3_TRIS = 0;
	RELAY4_TRIS = 0;
	RELAY5_TRIS = 0;
	RELAY6_TRIS = 0;
	RELAY7_TRIS = 0;
	RELAY8_TRIS = 0;
	RELAY9_TRIS = 0;
	RELAYA_TRIS = 0;
	RELAYB_TRIS = 0;
	RELAYC_TRIS = 0;

	RELAY1_IO = 0;	//继电器输出端口
	RELAY2_IO = 0;
	RELAY3_IO = 0;
	RELAY4_IO = 0;
	RELAY5_IO = 0;
	RELAY6_IO = 0;
	RELAY7_IO = 0;
	RELAY8_IO = 0;
	RELAY9_IO = 0;
	RELAYA_IO = 0;
	RELAYB_IO = 0;
	RELAYC_IO = 0;

	SPK_IO = 0;
	SPK_TRIS = 0;
	LED_RUN_IO = 0;	//RUN LED
	LED_RUN_TRIS = 0;

	BUTTON0_TRIS = 1;
	BUTTON0_IO = 1;	//按键输入端口



#if defined(__18CXX)                                     //CCT这段以前好多预编译指令，都删去了，因为我们只用PIC18F97J60 与 MCC18 .如果想应用于其它的芯片和编译器
	// Enable 4x/5x PLL on PIC18F87J10, PIC18F97J60, etc.//可以重新再选择一套程序，。这套PIC程序几乎涵盖了MICROCHIP所有的比较高级的芯片的使用，从8BIT-16BIT-32BIT
    OSCTUNE = 0x40;

	// Set up analog features of PORTA

	// PICDEM.net 2 board has POT on AN2, Temp Sensor on AN3

	//	ADCON0 = 0x09;		// ADON, Channel 2
		ADCON0 = 0x00;      //cct AD禁止

		ADCON1 = 0x0B;		// Vdd/Vss is +/-REF, AN0, AN1, AN2, AN3 are analog
		ADCON2 = 0xBE;			// Right justify, 20TAD ACQ time, Fosc/64 (~21.0kHz)

    // Enable internal PORTB pull-ups
    INTCON2bits.RBPU = 0;

	// Configure USART
    TXSTA = 0x00;
    RCSTA = 0x00;
/*
	// See if we can use the high baud rate setting                       //CCT,这个地方没有用，，没有用到过USART1.这块是波特的事,不好算，也没有必然去算，让机器自己选吧。
	#if ((GetPeripheralClock()+2*BAUD_RATE)/BAUD_RATE/4 - 1) <= 255
		SPBRG = (GetPeripheralClock()+2*BAUD_RATE)/BAUD_RATE/4 - 1;			//CCT用这是这行，在485与TCP桥中也有选择，建意最后一起删Y的
		TXSTAbits.BRGH = 1;
	#else	// Use the low baud rate setting
		SPBRG = (GetPeripheralClock()+8*BAUD_RATE)/BAUD_RATE/16 - 1;		//CCT这行没有用。是其它单位机要的波特率
	#endif
*/

	// Enable Interrupts
	RCONbits.IPEN = 1;		// Enable interrupt priorities
    INTCONbits.GIEH = 1;    
    INTCONbits.GIEL = 1;    
/*
    // Do a calibration A/D conversion
		ADCON0bits.ADCAL = 1;
	    ADCON0bits.GO = 1;
		while(ADCON0bits.GO);
		ADCON0bits.ADCAL = 0;
*/
#endif


#if defined(SPIRAM_CS_TRIS)     //cct这段本打算删的，=。@*_*@
	SPIRAMInit();
#endif

	PerDevice_Init();			//cct它"重要"，它出始化了好几个模块。在MYAPP.C中。
}

/*********************************************************************
 * Function:        void InitAppConfig(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
// Uncomment these two pragmas for production MAC address 
// serialization if using C18. The MACROM=0x1FFF0 statement causes 
// the MAC address to be located at aboslute program memory address 
// 0x1FFF0 for easy auto-increment without recompiling the stack for 
// each device made.  Note, other compilers/linkers use a different 
// means of allocating variables at an absolute address.  Check your 
// compiler documentation for the right method.
//#pragma romdata MACROM=0x1FFF0

//#pragma romdata
///static BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
// CCT mayee 例子程序中有ROM ，此处导致网络在第一次初始化时会出问题，导致无法过大型交换 20130708
static	ROM BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
static void InitAppConfig(void)
{
    BYTE c;
    BYTE *p;
   
   
	p = (BYTE*)&AppConfig;

    XEEBeginRead(0x0000);
    c = XEERead();
    XEEEndRead();
  	if(c == 0x6e)
    {
        XEEBeginRead(0x0001);
        for ( c = 0; c < sizeof(AppConfig); c++ )
            *p++ = XEERead();
        XEEEndRead();
		return;
    }
	else
	{
		AppConfig.Flags.bIsDHCPEnabled = TRUE;
		AppConfig.Flags.bInConfigMode = TRUE;
		memcpypgm2ram((void*)&AppConfig.MyMACAddr, (ROM void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));
											//CCT按照优先级，如下赋值是先左移，之后或運算
		AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
		AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
		AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
		AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
		AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
		AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
		AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;

	// Load the default NetBIOS Host Name
		memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
		FormatNetBIOSName(AppConfig.NetBIOSName);

 	   SaveAppConfig();
	}

}

void SaveAppConfig(void)                           
{
    BYTE c;
    BYTE *p;

    p = (BYTE*)&AppConfig;
    XEEBeginWrite(0x0000);
    XEEWrite(0x6e);
    for ( c = 0; c < sizeof(AppConfig); c++ )
    {
        XEEWrite(*p++);
    }
    XEEEndWrite();    
}

void initE2promData(void)
{
    BYTE c,x; 
   
	XEEBeginRead(0x4abf);
    c = XEERead();
    XEEEndRead();

  	if(c != 0x32)
    {
		XEEBeginWrite(0x5400);
		for(x=0;x<12;x++)
		{
			XEEWrite('0');
		}			    
		XEEEndWrite();

		XEEBeginWrite(0x4abf);
		XEEWrite(0x32);			    
		XEEEndWrite();
    }
}





