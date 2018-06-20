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

  char host_name[128],file_name[128],send_message[128+5],r_buffer[2048];
  int fd_s,i,num_c,err;
  struct sockaddr_in sa;
  struct hostent *hp;
  struct in_addr inaddr;

  hp=NULL;
  
  while(hp==NULL){
  write(1,"hostname:",9);
  num_c = read(0,host_name,126);
  host_name[num_c] = '\0';
  host_name[num_c+1] = '\0';
  write(1,"filename:",9);
  num_c = read(0,file_name,127);
  file_name[num_c] = '\0';
  sprintf(send_message,"GET /%s\n\r",&file_name[0]);

  printf("%s\n",&host_name);

  char hhost_name[]="www.okayama-u.ac.jp";
  //char ip[]="150.46.242.229";
  hp = gethostbyname(&hhost_name[0]);
  
  //hp = gethostbyname(&host_name[0]);                
  if(hp==NULL){herror("gethost");}
  }
  
  //inaddr.s_addr = *(unsigned int *)(hp->h_addr_list);
  //printf("hp_addr:%s\n",inet_ntoa(inaddr));

  printf("get IP\n");
  sa.sin_family = hp->h_addrtype;
  sa.sin_port = htons(PORT_NO);
  bzero(&sa.sin_addr,sizeof(sa.sin_addr));
  memcpy(&sa.sin_addr.s_addr,hp->h_addr, hp->h_length);
  //sa.sin_addr.s_addr = *(unsigned int *)hp->h_addr;
  //inet_aton("150.46.242.229",&sa.sin_addr);
  //printf("s_addr:%s\n",inet_ntoa(sa.sin_addr));

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
