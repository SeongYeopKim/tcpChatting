#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#define BUF_SIZE	1024
int main(void)
{
	WSADATA			wsa;
	SOCKET			firstHandle, clntHandle;
	SOCKADDR_IN		servAdr,clntAdr; //32��Ʈ�ּ� �ּ� ����ü Ÿ��.
	fd_set			reads,cpyReads;	 //SELECT set
	TIMEVAL			timeout; //Ÿ�Ӿƿ� �ð� ����
	int				fdNum,index,adrSize;
	char			buf[BUF_SIZE];
	int				strLen;
	//1.���� �ʱ�ȭ
	WSAStartup( MAKEWORD(2,2), &wsa );

	//2.���� ���� - ����
	firstHandle = socket( PF_INET, SOCK_STREAM, 0); //TCP ���� ���� //�� ������ �뵵�� connection �� �޴´�.f
	
	//3.�ּ� ����ü ����
	memset( &servAdr, 0, sizeof(servAdr) );
	servAdr.sin_family	= AF_INET;
	servAdr.sin_port	= htons(9000);
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);

	//4. bind ����.
	bind(firstHandle, (SOCKADDR*)&servAdr, sizeof(servAdr) );

	//5. listen ����
	listen( firstHandle, 5);

	//6. SELECT ��� ����
	 FD_ZERO( &reads );
	 FD_SET( firstHandle, &reads );

	 while(1)
	 {	
		 timeout.tv_sec		= 5;
		 timeout.tv_usec	= 5000;
		 cpyReads = reads;	//reads �� �Ѱ����� �ʰ� ���纻�� �Ѱ���.
		 fdNum = select(0, &cpyReads, 0, 0, &timeout );  //timeout�� ������ ��ٸ������� timeout������ ��ٸ��� ����������� ��.
		 if( fdNum == 0 ) //0�� �̺�Ʈ �߻��� ���Ѱ�(�����Ͱ� ���°͵� ���� ,��û�� ����) //timeout ������ return �� ���.
		 {
			 continue; //while loopó������ ���� ����. select �ݺ�����.
		 }

		 // �̺�Ʈ �߻��� ���
		 // ���� ������ Handle�� ���ؼ� �̺�Ʈ �߻� ���� Ȯ��
		 
		 for( index=0; index<reads.fd_count; index++)
		 {
			 if(FD_ISSET( reads.fd_array[index], &cpyReads ) ) //�̻��¿��� fd_array���� firsthandle�� ���ִ�(�����ڵ��®������), ���������� clntHandle�� �������� cpyReads�� 1�� �����ؼ� Ȯ����!��� ������. FD_ISSET�� Ȯ���ؼ� �Ǿ�������->�����û�̿°�
			 {
				 if( reads.fd_array[index] == firstHandle )
				 {
				 //���� ��û�� �� ���� ->accept �ϱ�
				 adrSize = sizeof(clntAdr);
				clntHandle = accept(firstHandle, (SOCKADDR*)&clntAdr, &adrSize );
				FD_SET( clntHandle, &reads );
				 }
				 else
				 {
					 //�����Ͱ� ���ŵ� ����
					 strLen = recv( reads.fd_array[index], buf, BUF_SIZE-1, 0 );
					 if( strLen == 0) //connection close �Ȼ���
					 {
						 //������ ����� ���
						 FD_CLR( reads.fd_array[index], &reads );
						 closesocket( cpyReads.fd_array[index] );
						 printf("> Ŭ���̾�Ʈ ���� ����\n");
					 }
					 else
					 {
						 //������ ���ŵ� ���.
						 buf[strLen] = '\0';
						 printf("> Msg : %s \n",buf);
						 send( reads.fd_array[index], buf, strLen, 0 ); //������ �ٽ� �����ϴ� ���?
					 }

				 }
			 }
		 }
	 }
				 
	closesocket(firstHandle);
	WSACleanup();
	return 0;

}