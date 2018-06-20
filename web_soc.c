#include<stdio.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<signal.h>
#include <errno.h>
#include<string.h>

#define PORT_NO 80

main(){

  signal(SIGPIPE,SIG_IGN);

  char send_message[128+5],r_buffer[2048];
  int fd_s,i,num_c,err;
  struct sockaddr_in sa;
  struct hostent *hp;

  hp=NULL;
  char file_name[] = "index.html";
  char host_name[]="www.okayama-u.ac.jp";

  while(hp==NULL){
  sprintf(send_message,"GET /%s\n\r",&file_name[0]);

  hp = gethostbyname(&host_name[0]);
                
  if(hp==NULL){herror("gethost");}
  }
  
  printf("hp_addr:%x\n",hp->h_addr);

  printf("get IP\n");
  sa.sin_family = hp->h_addrtype;
  sa.sin_port = htons(PORT_NO);
  bzero(&sa.sin_addr,sizeof(sa.sin_addr));
  memcpy(&sa.sin_addr,hp->h_addr, hp->h_length);
  
  printf("s_addr:%x\n",&sa.sin_addr.s_addr);

  fd_s = socket(AF_INET,SOCK_STREAM,0);             printf("make socket\n");
  err = connect(fd_s,(struct sockaddr*)&sa,sizeof(sa));
  printf("make connect:%d\n",err);
  if(err==-1){perror("connect");}

  printf("%s",send_message);

  num_c = send(fd_s,send_message,sizeof(send_message),0);
  printf("send message:%d\n",num_c);   
  if(num_c == -1){perror("send");}

  num_c = recv(fd_s,r_buffer,sizeof(r_buffer),0);   
  printf("recv message:%d err %d\n",num_c,errno); 
  if(num_c == -1){perror("recv");}
  write(1,r_buffer,num_c);
  close(fd_s);
}
