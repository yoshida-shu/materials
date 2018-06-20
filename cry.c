#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include<string.h>

#define PORT_NO 60000
#define BUF_SIZE 128

int
main() {
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
  char *toSendText = "This is a test";
  char r_buf[BUF_SIZE];

  /************************************************************/
  /* 相手先アドレスの入力 */
  //printf("Connect to ? : (name or IP address) ");
  //scanf("%s", dest);

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

  /* 送信 */
  //send(dst_soc, toSendText, strlen(toSendText)+1, 0);
    //sleep(1); 1s dilay
    num_c = send(dst_soc,toSendText,sizeof(toSendText),0);
    printf("cr-send message:%d\n",num_c);   
    if(num_c == -1){perror("send");}
    /*　受信　*/

    num_c = recv(dst_soc,r_buf,sizeof(r_buf)-1,0);
    r_buf[127]='\0';
    printf("cr-recv message:%d err %d\n",num_c,errno); 
    if(num_c == -1){perror("recv");}
    printf("cr-recv:%s\n",r_buf);

  /* ソケット終了 */
  close(dst_soc);
}






sprintf(str,"data:%d\n",i);
send_message(str,sizeof(str),cr_soc);


recv_message(&com[0],cr_soc);
