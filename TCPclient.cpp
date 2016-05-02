#include <winsock2.h>
#include <stdio.h>

#define	BUF_SIZE	512

void ErrorDisplay( char *str )
{
	printf("<ERROR> %s!!!\n", str );
	exit(-1);
}
 
// ����� ���� ������ ���� �Լ�
int recvn( SOCKET s, char *buf, int len, int flags )
{
	int 		received;
	char 	*ptr = buf;
	int 		left = len;

	while( left > 0 )
	{
		received = recv(s, ptr, left, flags);
		if(received == SOCKET_ERROR) 
			return SOCKET_ERROR;
		else if(received == 0) 
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}


int main( int argc, char* argv[] )
{
int		retval;		//recv(), send() �����ϸ� return ��: ������ ����Ÿ �� 

	// argv[0] : TCPClient
	// argv[1] : IP �ּ�
	// argv[2] : Port ��ȣ
	if( argc != 3 )	
	{
		printf("<input format>TCPClient <IP> <Port>\n");
		exit(0);
	}

	// ���� DLL: open()
	WSADATA	wsa;
	retval = WSAStartup(  MAKEWORD(2, 2), &wsa );
	if( retval != 0 )	return -1;

	// ���� �ϳ� �����...
	SOCKET	ClientSocket;
	ClientSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if( ClientSocket == INVALID_SOCKET )
	{
		ErrorDisplay("socket() error(INVALID_SOCKET)");
	}

	// connect() �Լ��� �̿��ؼ� ������ ���� �õ�...
	SOCKADDR_IN	ServerAddr;
	ZeroMemory( &ServerAddr, sizeof(ServerAddr)  );
	ServerAddr.sin_family			= AF_INET;
	ServerAddr.sin_port			= htons( atoi(argv[2]) );		// ���� ��Ʈ ��ȣ...
	ServerAddr.sin_addr.s_addr		= inet_addr( argv[1] );		// ���� IP �ּ�...
	
	retval = connect( ClientSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr) );
	if( retval == SOCKET_ERROR )
	{
		ErrorDisplay("connect() error(SOCKET_ERROR)");
	}

	// ����Ÿ�� ��, �����ϱ�...
	char		Buf[BUF_SIZE+1];
	//int		iLen;

	while( 1 )
	{
		// ����Ÿ�� �Է��ϰ�, �� ����Ÿ�� ������ �����ϱ�...
		// ���� û��...
		ZeroMemory( Buf, sizeof(Buf) );
		// ����ڷκ��� ����Ÿ �Է�...
		printf("\n[���� ����Ÿ] ");
		if( fgets( Buf, BUF_SIZE+1, stdin ) == NULL )	break;
		
		retval = send( ClientSocket, Buf, strlen(Buf), 0 );
		if( retval == SOCKET_ERROR )
		{
			printf("<ERROR> send()(SOCKET_ERROR)!!!\n");
			break;
		}
		printf("[TCP Ŭ���̾�Ʈ] %d ����Ʈ�� ���½��ϴ�.\n", retval);

		// �����κ��� ����Ÿ�� �����ϱ�...
		retval = recvn( ClientSocket, Buf, retval, 0 );
		if( retval == SOCKET_ERROR )
		{
			printf("<ERROR> recvn()(SOCKET_ERROR)!!!\n");
			break;
		}else if( retval == 0 )	break;

		// ���� ����Ÿ�� ����ϱ�...
		Buf[retval]= '\0';
		printf("[TCP Ŭ���̾�Ʈ] %d ����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		printf("[���� ����Ÿ] %s \n", Buf);
	}

	// ���� close...
	closesocket( ClientSocket );
	
	// ���� DLL: close()
	WSACleanup();

	return 0;
}