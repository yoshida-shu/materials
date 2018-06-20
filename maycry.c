#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>

#define BUF_SIZE 1024
#define RQ_SIZE 256 //C_MAXに同じ
#define PORT_NO 60000
#define SV_IP "172.16.2.6"

#define MAX_P 10000 //レコード数
#define MAX_F 70    //フィールドサイズ
#define MAX_L 1024  //不明
#define MAX_C 252   //RQ_SIZE-4
#define FIELD 5     //フィールド数

char ter[]  = "%END";

int send_message(char *str,int str_lng,int cr_soc){
  /*　送信　*/
  int i;
  char message[RQ_SIZE] = {0};
  sprintf(message,"%s%s",str,ter);
  if(send(cr_soc,message,RQ_SIZE,0)==-1){
      perror("cr-send");
      return -1;
  }
  return 0;
}
int recv_message(char *r_buf,int cr_soc){
  /* 受信　*/
  int i;
  char *ter = "%END";
  while(1){
    //printf("recv:ready\n");
    if(recv(cr_soc, r_buf, BUF_SIZE, 0)==-1){
      perror("cr-recv");
      return -1;
    }
    //printf("%s",r_buf);
    i = 0;
    printf("\n>>");
    while(r_buf[i]!='\0' && i<(BUF_SIZE-4)){
      if(strcmp(&r_buf[i],ter)==0){
	return 0;
      }
      putchar(r_buf[i]);
      i++;
    }
    //printf("\ncontinue\n");
    //r_buf[0]='\0';
  }
}

int main() {
  /* IP アドレス、ポート番号、ソケット */
  char dest[]="172.16.2.6";
  unsigned short port = PORT_NO;
  int dst_soc;

  /* sockaddr_in 構造体 */
  struct sockaddr_in dst_addr;

  /* 各種パラメータ */
  int status;
  int numsnt;
  int i,err,num_c;
  char r_buf[BUF_SIZE],s_buf[MAX_C];
/* sockaddr_in 構造体のセット */
  memset(&dst_addr, 0, sizeof(dst_addr));
  dst_addr.sin_port = htons(port);
  dst_addr.sin_family = AF_INET;
  dst_addr.sin_addr.s_addr = inet_addr(dest);
 
  /* ソケット生成 */
  dst_soc = socket(AF_INET, SOCK_STREAM, 0);

  /* 接続 */
  printf("cr-Trying to connect to %s: \n", dest);
  err = connect(dst_soc, (struct sockaddr *) &dst_addr, sizeof(dst_addr));
  printf("cr-connect:%d\n",err);

  while(1){
    //printf("recv waiting\n");
    if(recv_message(r_buf,dst_soc)==-1){
      //fprintf(stderr,"cr-recv faild\n");
      printf("cr-recv faild\n");
      return 0;
    }
    memset(s_buf,'\0',MAX_C);
    fgets(s_buf,MAX_C,stdin);
    //read(stdin,s_buf,MAX_C);
    //printf("message:%s\n",s_buf);
    if(s_buf[0]=='%'){
      if(s_buf[1]=='Q'){
	send_message(s_buf,MAX_C,dst_soc);
	close(dst_soc);
	exit(0);
      }
    }
    //if(s_buf[0]=='\n'){s_buf[0]='\0';}
    if(send_message(s_buf,MAX_C,dst_soc)==-1){
      //fprintf(stderr,"cr-send faild\n");
      printf("cr-send faild\n");
      return 0;
    }
  }
}
  
