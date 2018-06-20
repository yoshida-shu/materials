#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>

#define BUF_SIZE 128
#define PORT_NO 60000

int
main() {
  /* ポート番号、ソケット */
  unsigned short port = PORT_NO;
  int sr_soc;  // 自分
  int cr_soc;  // 相手

  /* sockaddr_in 構造体 */
  struct sockaddr_in sr_addr;
  struct sockaddr_in cr_addr;
  int cr_addr_size = sizeof(cr_addr);

  /* 各種パラメータ */
  int numrcv;
  int err,num_c;
  char r_buf[BUF_SIZE],s_buf[BUF_SIZE];
  char *send_message;
  send_message = &s_buf[0];
  /************************************************************/
  /* sockaddr_in 構造体のセット */
  memset(&sr_addr, 0, sizeof(sr_addr));
  sr_addr.sin_port = htons(port);
  sr_addr.sin_family = AF_INET;
  sr_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* ソケットの生成 */
  sr_soc = socket(AF_INET, SOCK_STREAM, 0);

  /* ソケットのバインド */
  if(bind(sr_soc, (struct sockaddr *) &sr_addr, sizeof(sr_addr))==-1){
    perror("bind");
  }

  /* 接続の許可 */
  err = listen(sr_soc, 5);
  printf("sv-listen:%d\n",err);

  /* 接続の受付け */
  printf("Waiting for connection ...\n");
  cr_soc = accept(sr_soc, (struct sockaddr *) &cr_addr, &cr_addr_size);
  printf("sv-Connected from %s\n", inet_ntoa(cr_addr.sin_addr));

  /* 受信 */
    numrcv = recv(cr_soc, r_buf, BUF_SIZE, 0); 
    printf("sv-received: %s\n", r_buf);

  sprintf(send_message,"ppp%sqqq",&r_buf[0]);
  printf("sv-hi\n");
  /*　送信　*/
  num_c = send(cr_soc,send_message,sizeof(send_message),0);
  printf("sv-send message:%d\n",num_c);   
  if(num_c == -1){perror("send");}
  
  close(cr_soc);

}
