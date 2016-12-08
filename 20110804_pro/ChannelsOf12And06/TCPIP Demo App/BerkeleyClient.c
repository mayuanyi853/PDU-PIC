/*****************************************************
*���ߣ�   CCT
*ʱ�䣺   2010/12/30-
*���ܣ�   ʵ�ֶԵ�Ƭ��������ͨ�ţ���UDP����������֤��֮��ͨ��
		  TCP�������ӵ�TCP-SERVER��DELPHI��ʵ�ֱ���������
*�޸ģ�   2010/01/10 CCT ����UDP��TCP,ע��V4.51 BSD�ܶ�BUGҪע��
*�޸ģ�   2010/01/28 CCT �̵������أ�		 
*�޸ģ�	  2011/06/08 CCT ����һ�����������ڼ����DELPHIͨ�ţ�����ʾ��Һ������		 
******************************************************/
 
#include "TCPIP Stack/TCPIP.h"
#if defined(STACK_USE_BERKELEY_API)

#define BSDPORTNUM 18096                                       //CCT����UDP�˿ںţ��Ա�DELPHI�㲥
#define BSDREMOTEPORTNUM 18095
//BYTE sendRequest[9] = "connected";
BYTE passWord[]="STARTSEIPABCDEFGH$$$$$$$$$$$$END";
enum _BSDServerState								   //CCT״̬��ģ�ͣ�ʵ��ͨ�ŵ���
{      
		UDP_START_BIND,
		UDP_RECV,
		UDP_CLOSE,
		TCP_START,
		TCP_BIND,
        TCP_CONNECT,
        TCP_OPERATION,
        TCP_CLOSE
} BSDClientState = UDP_START_BIND;
extern TICK t3;	
extern BYTE TCPSendTempHumi[10];
extern BYTE relaySet[13];
extern BYTE AVs[12][4];                                         //cct12·������ѹʵʱ��������	
extern BYTE NET_Link;
extern BYTE NET_Link_MAC; //����û����������Ϊ5 --��0 ������������ʱΪ0
			
//////////////////////////////////////////////////////////////////////////////////CCT ʵ��TCP��UDP�����ͨ�š�����ȵȡ�
void BerkeleyCommunication(void)
{
	
    static SOCKET bsdSocket = INVALID_SOCKET;
//	static SOCKET bsdUDPSocket = INVALID_SOCKET;
//	static BOO
	static BYTE TCP_ConnectSum = 0;
    static struct sockaddr_in addr,udpr,udprBroadadd;
	static BYTE connectFlag = 0;
    char recvBuffer[32];									   //CCT����TCP��UDP��������
	char sendBuffer[32];									   //CCT����TCP��UDP��������
  //  char sendUdp[2];
	int i,x,y,command;
    int addrlen = sizeof(struct sockaddr_in);                  
	UDP_SOCKET_INFO *p;                                        //CCT��UDP��C�����ã��ó�����TCP��SERVER��ַ
   	BYTE testPwd[8];										   //CCT������ȡ
    BYTE flag1=0;											   //CCTӦ���ڸ��ֱ�־�ص�
	static TICK TIMEOUT1 = 0;

    switch(BSDClientState)
    {
		case UDP_START_BIND:
			bsdSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//		bsdUDPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            addr.sin_port = BSDPORTNUM;
		//		addr.sin_port = 0;
            addr.sin_addr.S_un.S_addr = IP_ADDR_ANY;
            if( bind(bsdSocket, (struct sockaddr*)&addr, addrlen) == SOCKET_ERROR )           
           	    break;
            BSDClientState = UDP_RECV;
			break;
		case UDP_RECV:
			if(TickGet() - t3 >= 5*TICK_SECOND)
			{
				t3=TickGet();

				strcpypgm2ram((BYTE*)sendBuffer,"STARTRENOCONNECT$$$$$$$$$$$$$END");

				sendto(bsdSocket, (BYTE*)sendBuffer, sizeof(sendBuffer), 0,0,0);

				break;
			}

			memset(recvBuffer,'\0',sizeof(recvBuffer));
			i = recvfrom(bsdSocket, (char*)&recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr*)&addr, &addrlen);
            if(i <= 0)
			{
				break;
			}
			else if(i>0)
			{
				
				if(memcmp(passWord,recvBuffer,sizeof(recvBuffer))==0)
				{

					p = &UDPSocketInfo[activeUDPSocket];
					udpr.sin_addr.S_un.S_addr = p->remoteNode.IPAddr.Val;
           			udpr.sin_port = (p->remotePort)+10;
					BSDClientState = UDP_CLOSE;
				}
			}
			break;
		case  UDP_CLOSE:
			closesocket(bsdSocket);
			BSDClientState = TCP_START;		
			break;

        case TCP_START:
            if((bsdSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR )
                return;         	         
            BSDClientState = TCP_BIND;
            break;

		case TCP_BIND:
			addr.sin_port = 0;
			addr.sin_addr.S_un.S_addr = IP_ADDR_ANY;
		    addrlen = sizeof(struct sockaddr_in);
			if(bind(bsdSocket, (struct sockaddr*)&addr, addrlen) == SOCKET_ERROR )
				return;
			BSDClientState = TCP_CONNECT;
			break;

        case TCP_CONNECT:
			TCP_ConnectSum++;
			for(x=0;x<=20;x++)
			{
          		  if(connect( bsdSocket, (struct sockaddr*)&udpr, addrlen) ==0)
				  {
						BSDClientState = TCP_OPERATION;
						TCP_ConnectSum = 0;
						connectFlag = 0;
       	    			break;
				   }	   
			}
			if(TCP_ConnectSum>=100)
			{
				BSDClientState = TCP_CLOSE;
				TCP_ConnectSum = 0;
			}	
            break;   
 
        case TCP_OPERATION:

            i=recv(bsdSocket, recvBuffer, sizeof(recvBuffer), 0);
			if( i ==  SOCKET_ERROR)
			{
				BSDClientState =TCP_CLOSE;
				break;		
			}else if(i == SOCKET_DISCONNECTED)
			{
				BSDClientState =TCP_CLOSE;
				break;
			}else if((i == sizeof(recvBuffer))&&(recvBuffer[0]=='S')&&(recvBuffer[1]=='T')&&(recvBuffer[4]=='T')&&(recvBuffer[5]=='S')&&(recvBuffer[6]=='E')&&(recvBuffer[28]=='$')&&(recvBuffer[29]=='E')&&(recvBuffer[31]=='D'))
			{
				connectFlag = 0;
				command = (recvBuffer[7]-'0')*10+(recvBuffer[8]-'0');
				memset(sendBuffer,'\0',32);
				switch(command)
				{
					case 1:								///////////////////////////cct ������֤
						memset(testPwd,'\0',8);
						flag1 = 0;
						XEEBeginRead(0x4000);							
						for(x=0;x<8;x++)
						{
							testPwd[x]=XEERead();
							if(testPwd[x]==recvBuffer[9+x])
								continue;
							else
								flag1 = 1;	
						}
						XEEEndRead();

						if(flag1 == 1)
						{
							 strcpypgm2ram((BYTE*)sendBuffer,"STARTRE01NO$$$$$$$$$$$$$$$$$$END");
							 send(bsdSocket, (BYTE*)sendBuffer, 32, 0);
							 break;
						}
						else
						{
							  flag1 = 0;
						//	  TIMEOUT1 = TickGet();	
							  break;
						}
						break;													
					case 2:								//////////////////////////////cct ������֤
						if(flag1 == 0)              //&&(TickGet() - TIMEOUT1 <= TICK_SECOND*2)
						{
							XEEBeginWrite(0x4000);
							for(x=0;x<8;x++)
							{
								XEEWrite(recvBuffer[9+x]);
							}			    
			    			XEEEndWrite();
						    strcpypgm2ram((BYTE*)sendBuffer,"STARTRE01OK$$$$$$$$$$$$$$$$$$END");
							send(bsdSocket, (BYTE*)sendBuffer, 32, 0);							

						}													
							break;						

					case 3:
							strcpypgm2ram((BYTE*)sendBuffer,"STARTRE01OK$heiheiheiheihei&$END");
							send(bsdSocket, (BYTE*)sendBuffer, 32, 0);	
							break;
						break;	
					case 4:
						break;
					case 7:                             ////////////cct �̵�������
						for(x=0;x<12;x++)
							relaySet[x]= recvBuffer[9+x];
                        relaySet[12]=2;
                        strcpypgm2ram((BYTE*)sendBuffer, "STARTRE13");
                       for(x=0;x<12;x++)
                            sendBuffer[9+x] = relaySet[x];
						strcpypgm2ram(((char*)sendBuffer)+21,"$$$$$$$$END");
                        send(bsdSocket, (BYTE*)sendBuffer, 32, 0);
						break;
                    case 8:                           ///////////////CCT ����ʪʪ��
						strcpypgm2ram((BYTE*)sendBuffer, "STARTRE08");
						for(x=0;x<10;x++)
							sendBuffer[9+x] = TCPSendTempHumi[x];
						strcpypgm2ram(((char*)sendBuffer)+19,"$$$$$$$$$$END");
						send(bsdSocket, (BYTE*)sendBuffer, 32, 0);
                        break;
                    case 10:                           //////////////cct ����1-4��ѹ������
                        strcpypgm2ram((BYTE*)sendBuffer, "STARTRE10");
                        for(x=0;x<4;x++)
                        {
                           for(y=0;y<4;y++)
					       {
                              sendBuffer[x*4+y+9]=AVs[x][y];
                           }
                        }
						strcpypgm2ram(((char*)sendBuffer)+25,"$$$$END");
                        send(bsdSocket, (BYTE*)sendBuffer, 32, 0);
                        break; 
                    case 11:                           //////////////cct ����5-8��ѹ������
                        strcpypgm2ram((BYTE*)sendBuffer, "STARTRE11");
                        for(x=0;x<4;x++)
                        {
                           for(y=0;y<4;y++)
					       {
                              sendBuffer[x*4+y+9]=AVs[x+4][y];
                           }
                        }
						strcpypgm2ram(((char*)sendBuffer)+25,"$$$$END");
                        send(bsdSocket, (BYTE*)sendBuffer, 32, 0);
                        break; 
                    case 12:                           //////////////cct ����9-12��ѹ������
                        strcpypgm2ram((BYTE*)sendBuffer, "STARTRE12");
                        for(x=0;x<4;x++)
                        {
                           for(y=0;y<4;y++)
					       {
                              sendBuffer[x*4+y+9]=AVs[x+8][y];
                           }
                        }
						strcpypgm2ram(((char*)sendBuffer)+25,"$$$$END");
                        send(bsdSocket, (BYTE*)sendBuffer, 32, 0);
                        break;
                   case 13:                           //////////////cct ���������̵�����ǰ״̬
                        strcpypgm2ram((BYTE*)sendBuffer, "STARTRE13");
                       for(x=0;x<12;x++)
                            sendBuffer[9+x] = relaySet[x];
						strcpypgm2ram(((char*)sendBuffer)+21,"$$$$$$$$END");
                        send(bsdSocket, (BYTE*)sendBuffer, 32, 0);
                        break;
                              
				}


					BSDClientState =TCP_OPERATION;
					break;
			}else if(i != sizeof(recvBuffer))
			{
				if(TickGet() - t3 >= 7*TICK_SECOND)
				{
					t3=TickGet();
							//On success, send returns number of bytes sent. In case of error, returns SOCKET_ERROR. a zero indicates no data send.
					strcpy(sendBuffer, AppConfig.NetBIOSName);
				    strcpypgm2ram(((char*)sendBuffer)+13,"connected");
					i=send(bsdSocket, (BYTE*)sendBuffer, 32, 0);
			//		i=send(bsdSocket,(BYTE*)sendRequest,sizeof(sendRequest),0);
					if((i == SOCKET_ERROR)||(i == 0))
						connectFlag++;
					else if(i == sizeof(sendBuffer))
						connectFlag = 0;
					if(connectFlag>=6)
						BSDClientState = TCP_CLOSE;				
				}
				break;  
			}

      
        case TCP_CLOSE:
            closesocket(bsdSocket);
			BSDClientState = UDP_START_BIND;
			break;         
        default:
            return;
    }
}

void ConnectToLCD(void)
{
#if defined(POWER_SUPPLY_12)
	if(BSDClientState == TCP_OPERATION)
	{
		strcpypgm2ram(((char*)LCDText)+16, "����ͨ��:   ����");///////////////
		return;
	}
	else
	{
		strcpypgm2ram(((char*)LCDText)+16, "����ͨ��:   �쳣");///////////////
		return;
	}
#elif defined(POWER_SUPPLY_6)||defined(POWER_SUPPLY_12_1602)
/*	if(BSDClientState == TCP_OPERATION)
	{
		strcpypgm2ram(((char*)LCDText)+16, "network   :OK");///////////////
		return;
	}
	else
	{
		strcpypgm2ram(((char*)LCDText)+16, "network   :ERROR");///////////////
		return;
	}
*/
	if(NET_Link > 0 && NET_Link_MAC==0)
	{
		strcpypgm2ram(((char*)LCDText)+16, "network   :OK");///////////////
		return;
	}
	else
	{
		strcpypgm2ram(((char*)LCDText)+16, "network   :ERROR");///////////////
		return;
	}
#endif  //#if defined(POWER_SUPPLY)
}

#endif












