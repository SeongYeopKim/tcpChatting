#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#define BUF_SIZE	1024
int main(void)
{
	WSADATA			wsa;
	SOCKET			firstHandle, clntHandle;
	SOCKADDR_IN		servAdr,clntAdr; //32비트주소 주소 구조체 타입.
	fd_set			reads,cpyReads;	 //SELECT set
	TIMEVAL			timeout; //타임아웃 시간 설정
	int				fdNum,index,adrSize;
	char			buf[BUF_SIZE];
	int				strLen;
	//1.소켓 초기화
	WSAStartup( MAKEWORD(2,2), &wsa );

	//2.소켓 생성 - 서버
	firstHandle = socket( PF_INET, SOCK_STREAM, 0); //TCP 소켓 생성 //이 소켓의 용도는 connection 만 받는다.f
	
	//3.주소 구조체 설정
	memset( &servAdr, 0, sizeof(servAdr) );
	servAdr.sin_family	= AF_INET;
	servAdr.sin_port	= htons(9000);
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);

	//4. bind 실행.
	bind(firstHandle, (SOCKADDR*)&servAdr, sizeof(servAdr) );

	//5. listen 실행
	listen( firstHandle, 5);

	//6. SELECT 사용 시작
	 FD_ZERO( &reads );
	 FD_SET( firstHandle, &reads );

	 while(1)
	 {	
		 timeout.tv_sec		= 5;
		 timeout.tv_usec	= 5000;
		 cpyReads = reads;	//reads 를 넘겨주지 않고 복사본을 넘겨줌.
		 fdNum = select(0, &cpyReads, 0, 0, &timeout );  //timeout은 무한정 기다리지말고 timeout까지만 기다리고 빠져나오라는 것.
		 if( fdNum == 0 ) //0은 이벤트 발생을 안한것(데이터가 들어온것도 없고 ,요청도 없고) //timeout 때문에 return 된 경우.
		 {
			 continue; //while loop처음으로 가서 실행. select 반복실행.
		 }

		 // 이벤트 발생한 경우
		 // 내가 설정한 Handle에 대해서 이벤트 발생 여부 확인
		 
		 for( index=0; index<reads.fd_count; index++)
		 {
			 if(FD_ISSET( reads.fd_array[index], &cpyReads ) ) //이상태에서 fd_array에는 firsthandle이 들어가있다(아직코드덜짰을때임), 최종적으론 clntHandle도 들어가있을것 cpyReads에 1로 설정해서 확인해!라고 보낸것. FD_ISSET이 확인해서 되어있으면->연결요청이온것
			 {
				 if( reads.fd_array[index] == firstHandle )
				 {
				 //연결 요청이 온 상태 ->accept 하기
				 adrSize = sizeof(clntAdr);
				clntHandle = accept(firstHandle, (SOCKADDR*)&clntAdr, &adrSize );
				FD_SET( clntHandle, &reads );
				 }
				 else
				 {
					 //데이터가 수신된 상태
					 strLen = recv( reads.fd_array[index], buf, BUF_SIZE-1, 0 );
					 if( strLen == 0) //connection close 된상태
					 {
						 //연결이 종료된 경우
						 FD_CLR( reads.fd_array[index], &reads );
						 closesocket( cpyReads.fd_array[index] );
						 printf("> 클라이언트 소켓 종료\n");
					 }
					 else
					 {
						 //데이터 수신된 경우.
						 buf[strLen] = '\0';
						 printf("> Msg : %s \n",buf);
						 send( reads.fd_array[index], buf, strLen, 0 ); //서버가 다시 에코하는 기능?
					 }

				 }
			 }
		 }
	 }
				 
	closesocket(firstHandle);
	WSACleanup();
	return 0;

}