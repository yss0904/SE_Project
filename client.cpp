#include<iostream>
#include<WINSOCK2.H>
#include<STDIO.H>
#include<cstring>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

int main(){
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if( WSAStartup( sockVersion, &data ) != 0 )
		return 0;
	while( true ) {
		SOCKET sclient = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if( sclient == INVALID_SOCKET ){
			printf( "invalid socket!" );
			return 0;
		}

		sockaddr_in serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(8888);
		serAddr.sin_addr.S_un.S_addr = inet_addr( "127.0.0.1" );
		if( connect( sclient, ( sockaddr * )&serAddr, sizeof( serAddr ) ) == SOCKET_ERROR ){  //连接失败
			printf( "connect error !" );
			closesocket( sclient );
			return 0;
		}

		string userName;
		cout << "userName: ";
		cin >> userName;
		const char * c_name = userName.c_str();
        send( sclient,c_name,strlen(c_name),0 );

		//接收消息
		char recData[255];
		int ret = recv( sclient, recData, 255, 0 );
		if( ret > 0 ){
			recData[ret] = 0x00;
			printf( recData );
		}

		//发送消息
		string data;
		const char * sendData;
		while( cin >> data ){
            sendData = data.c_str();   //string转const char*
            send( sclient, sendData, strlen( sendData ), 0 );
            if( data == "end" ){
                printf( "退出连接！\n" );
                closesocket( sclient );
                break;
            }
		}
		break;
	}

	WSACleanup();
	return 0;
}
