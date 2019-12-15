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
    printf( "�û���Ϊ��%s\n",userName );

    //��������
    string temp = string( userName );
    string s = "��ã�TCP�ͻ��� " + temp + "\n";
    const char * sendData = s.c_str();
    send( tmp_sClient,sendData,strlen(sendData),0 );

    //��������
    char revData[255];
    int ret = recv( tmp_sClient,revData,255,0 );
    while( ret > 0 ){
        revData[ret] = 0x00;
        if( !strcmp(revData,"end") ){
            closesocket( tmp_sClient );   //���յ���ϢΪendʱ�Źرյ�ǰsocket
            printf( "�ͻ��� %s ���˳����ӣ�\n",userName );
            break;
        }
        printf( "%s\n",revData );
        ret = recv( tmp_sClient,revData,255,0 );
    }
}

int main(){
    //��ʼ��WSA
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if( WSAStartup( sockVersion, &wsaData ) != 0 )
        return 0;

    //�����׽���
    SOCKET slisten = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( slisten == INVALID_SOCKET ){
        printf("socket error !");
        return 0;
    }

    //��IP�Ͷ˿�
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if( bind( slisten, ( LPSOCKADDR )&sin, sizeof( sin ) ) == SOCKET_ERROR )
        printf( "bind error !" );

    //��ʼ����
    if( listen( slisten, 5 ) == SOCKET_ERROR ){
        printf("listen error !");
        return 0;
    }

    HANDLE hThread;

    //ѭ����������
    while( true ){
        printf( "�ȴ�����...\n" );
        SOCKET sClient;
        sockaddr_in remoteAddr;
        int nAddrlen = sizeof( remoteAddr );
        char revData[255];
        sClient = accept( slisten, ( SOCKADDR * )&remoteAddr, &nAddrlen );
        if( sClient == INVALID_SOCKET ){
            printf( "accept error !" );
            continue;
        }
        printf( "���ܵ�һ�����ӣ�%s, %d\n",inet_ntoa( remoteAddr.sin_addr ),ntohs( remoteAddr.sin_port ) );
        unsigned int threadid;
        hThread = (HANDLE)_beginthreadex( NULL,0,Accept,(void*)sClient,0,&threadid );
        CloseHandle( hThread );
    }

    closesocket( slisten );
    WSACleanup();
    return 0;
}
