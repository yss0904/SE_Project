#include <iostream>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>
#include <sstream>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

unsigned __stdcall Accept( void* param ){
    SOCKET tmp_sClient = (SOCKET)param;
    char userName[100];
    recv( tmp_sClient,userName,sizeof( userName ),0 );
    printf( "用户名为：%s\n",userName );

    //发送数据
    string temp = string( userName );
    string s = "你好，TCP客户端 " + temp + "\n";
    const char * sendData = s.c_str();
    send( tmp_sClient,sendData,strlen(sendData),0 );

    //接收数据
    char revData[255];
    int ret = recv( tmp_sClient,revData,255,0 );
    while( ret > 0 ){
        revData[ret] = 0x00;
        if( !strcmp(revData,"end") ){
            closesocket( tmp_sClient );   //接收的消息为end时才关闭当前socket
            printf( "客户端 %s 已退出连接！\n",userName );
            break;
        }
        printf( "%s\n",revData );
        ret = recv( tmp_sClient,revData,255,0 );
    }
}

int main(){
    //初始化WSA
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if( WSAStartup( sockVersion, &wsaData ) != 0 )
        return 0;

    //创建套接字
    SOCKET slisten = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( slisten == INVALID_SOCKET ){
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if( bind( slisten, ( LPSOCKADDR )&sin, sizeof( sin ) ) == SOCKET_ERROR )
        printf( "bind error !" );

    //开始监听
    if( listen( slisten, 5 ) == SOCKET_ERROR ){
        printf("listen error !");
        return 0;
    }

    HANDLE hThread;

    //循环接收数据
    while( true ){
        printf( "等待连接...\n" );
        SOCKET sClient;
        sockaddr_in remoteAddr;
        int nAddrlen = sizeof( remoteAddr );
        char revData[255];
        sClient = accept( slisten, ( SOCKADDR * )&remoteAddr, &nAddrlen );
        if( sClient == INVALID_SOCKET ){
            printf( "accept error !" );
            continue;
        }
        printf( "接受到一个连接：%s, %d\n",inet_ntoa( remoteAddr.sin_addr ),ntohs( remoteAddr.sin_port ) );
        unsigned int threadid;
        hThread = (HANDLE)_beginthreadex( NULL,0,Accept,(void*)sClient,0,&threadid );
        CloseHandle( hThread );
    }

    closesocket( slisten );
    WSACleanup();
    return 0;
}
