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

#define BUF_SIZE 1024 //
#define RQ_SIZE 256 //C_MAXに同じ
#define PORT_NO 60000
#define SV_IP "172.16.2.6"

/***ここから**************************************************************/

#define MAX_P 10000 //レコード数
#define MAX_F 70    //フィールドサイズ
#define MAX_L 1024  //不明
#define MAX_C 252   //RQ_SIZE-4
#define FIELD 5     //フィールド数

char ter[] = "%END";

typedef struct{
  int year;
  int month;
  int date;
}born;

typedef struct{
    int id;
    char name[RQ_SIZE];
    born birth;
    char addr[RQ_SIZE];
    char *note;
    int flg;
} profile;

int write_p(char *buffer,profile *pb,int cr_soc);
int subst(char *str, char c1, char c2,int i);/*c1->c2*/
int func_b(born *p,char *buffer,int cr_soc);
int split(char *buf,char *law[],int field);
int com_p(char com[],profile data[],int i,int cr_soc);
int com_r(char com[],FILE *fp,profile data[],int *i,int cr_soc);
int com_w(char com[],FILE *fp,profile data[],int i,int cr_soc);
int com_f(char com[],profile data[],int i,int cr_soc);
int com_s(profile data[],int i,char com[],int cr_soc);
int id_sort(profile data[],int scp_r,int scp_l);
int name_sort(profile data[],int scp_r,int scp_l);
int birth_sort(profile data[],int scp_r,int scp_l);
int addr_sort(profile data[],int scp_r,int scp_l);
int note_sort(profile data[],int scp_r,int scp_l);
int btoi(profile data);
int com_e(char com[],profile data[],int i,int cr_soc);
int flg_print(profile data[],int i,int cr_soc);
int com_a(profile data[],int *i,int cr_soc);
int com_partf(profile data[],int i);
int fcomp(profile *data,char buffer[],char *str,int i); 

/***ここまで名簿*************************************************************/

int send_message(char *str,int str_lng,int cr_soc){
  /*　送信　sigは最後のみ0　ほかは1*/
  int i;
  if(send(cr_soc,str,BUF_SIZE,0)==-1){
      perror("sv-send");
      return -1;
  }
  return 0;
}
int recv_message(char *r_buf,int cr_soc){
  /* 受信　*/ 
  int i = 0;
  while(1){
    int rcv = recv(cr_soc, r_buf, RQ_SIZE, 0);
    printf("rcv:%d,buf:%s\n",rcv,r_buf);
    if(rcv == -1){
    //if(recv(cr_soc, r_buf, RQ_SIZE, 0)==-1){
      perror("sv-recv");
      return -1;
    }
    if(rcv == 0){
      r_buf[0]='\0';
      return 0;
    }
    while(r_buf[i]!='\0' && i<(RQ_SIZE-4)){
      if(strcmp(&r_buf[i],ter)==0){
	r_buf[i] = '\0';
	return 0;
      }
      putchar(r_buf[i]);
      i++;
    }
  }
}


int main(){
  FILE *fp;
  int i=0,reset;
  profile data[MAX_P];
  int l;
  char fname[MAX_C],str[MAX_L];
  char com[MAX_C];
  char c;

  /* ポート番号、ソケット */
  unsigned short port = PORT_NO;
  int sr_soc;  // 自分
  int cr_soc;  // 相手
  
  /* sockaddr_in 構造体 */
  struct sockaddr_in sr_addr;
  struct sockaddr_in cr_addr;
  int cr_addr_size = sizeof(cr_addr);
  
  //make_socket(sr_soc, cr_soc, port, cr_addr_size);
  
  /* 各種パラメータ */
  int err;
 
    /******ここから********************************************************/
    /* sockaddr_in 構造体のセット */
    memset(&sr_addr, 0, sizeof(sr_addr));
    sr_addr.sin_port = htons(port);
    sr_addr.sin_family = AF_INET;
    sr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    /* ソケットの生成 */
    sr_soc = socket(AF_INET, SOCK_STREAM, 0);

    /* ポート再利用 */
    int yes = 1;

    if (setsockopt(sr_soc, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes)) < 0)
    {
        perror("ERROR on setsockopt");
        exit(1);
    }
    
    /* ソケットのバインド */
    if(bind(sr_soc, (struct sockaddr *) &sr_addr, sizeof(sr_addr))==-1){
      perror("bind");
    }
    while(1){
    /* 接続の許可 */
    err = listen(sr_soc, 5);
    printf("sv-listen:%d\n",err);
    
    /* 接続の受付け */
    printf("Waiting for connection ...\n");
    cr_soc = accept(sr_soc, (struct sockaddr *) &cr_addr, &cr_addr_size);
    printf("sv-Connected from %s\n", inet_ntoa(cr_addr.sin_addr));
    
    /*******ここまで接続の確立***************************************************/  
    
    /*以下名簿本体*/
    
    /**print各種はすべてsendへ置き換え**/
    int hoge;
    for(reset=0;reset!=1;){
      c = '\0';
      com[0]='\0';
      memset(com,'\0',MAX_C);
      memset(str,'\0',MAX_L);
      //printf("command:");
      sprintf(str,"command:%s",ter);
      send_message(str,sizeof(str),cr_soc);
      printf("/****rec1****/");
      printf("Waiting for com-recieving\n");
      recv_message(&com[0],cr_soc);
      //fgets(com,sizeof(com),stdin);/*****recvへ*****/
      if(com[0]=='%'){c=com[1];}
      //printf("com:%s,c:%s",&com[0],c);
      printf("com:%s,c:%s",&com[0],&com[1]);
      switch(c){
      case 'C':
	//printf("data:%d\n",i);
	sprintf(str,"data:%d\n",i);
	send_message(str,sizeof(str),cr_soc);
	break;
      case 'P':
	com_p(com,data,i,cr_soc);
	//send_message(ter,sizeof(ter),cr_soc);
	break;
      case 'Q':
	//exit(0);              /*******終了処理へ***********/
	close(cr_soc);
	reset=1;
	break;
      case 'R':
	com_r(com,fp,data,&i,cr_soc);
	break;
      case 'W':
	com_w(com,fp,data,i,cr_soc);
	break;
      case 'F':
	com_f(com,data,i,cr_soc);
	break;
      case 'S':
	com_s(data,i,com,cr_soc);
	break;
      case 'D':
	com_d(data,&i,com);
	break;
      case 'E':
	hoge = com_e(com,data,i,cr_soc);
	printf("hoge:%d\n",hoge);
	break;
      case 'A':
	com_a(data,&i,cr_soc);
	break;
      default:
	//printf("invarid\n");
	//printf("Add,Clear,Delete,Read,Write,Sort,Find,...");
	sprintf(str,"invarid\n");printf("default\n");
	send_message(str,sizeof(str),cr_soc);
	break;
      }
    }
   }
}
int write_p(char *buffer,profile *pp,int cr_soc){
  char *law[FIELD];
  if(subst(buffer,',','\0',FIELD)!=(FIELD-1))return 1;/*format error*/
  split(buffer,law,FIELD);
  pp->id = atoi(law[0]);
  strncpy(pp->name, law[1], MAX_F);
  pp->name[strlen(law[1])] = '\0';
  if(func_b(&(pp->birth),law[2],cr_soc))return 1;
  strncpy(pp->addr, law[3], MAX_F);
  pp->addr[strlen(law[3])] = '\0';
  pp->note = (char *)malloc(sizeof(char) * (strlen(law[4])+1));
  strncpy(pp->note, law[4],(strlen(law[4])+1));
  pp->note[strlen(law[5])] = '\0';
  return 0;
  
}

int subst(char *str, char c1, char c2,int i){
  int n = 0;
  while (*str && n<i) {
    if (*str == c1) {
      *str = c2;
      n++;
    }
    str++;
  }
  //printf("'%c'counter:%d\n",c1,n);
  return n;
}

int split(char *buf,char *law[],int field){
  int n;
  law[0]=buf;
  for(n=1;n<field;n++){
    while(*buf){buf++;}
    law[n]=++buf;
  }
}

int func_b(born *p,char *buffer,int cr_soc){
  char *law[3],str[MAX_C];
  born t;
  if(subst(buffer,'-','\0',3)!=2){
    //printf("field:birth ");
    sprintf(str,"field:birth %s",ter);
    send_message(str,sizeof(str),cr_soc);
    return 1;
  }
  else{
    split(buffer,law,3);
    t.year=atoi(law[0]);
    t.month=atoi(law[1]);
    t.date=atoi(law[2]);
    *p=t;
    return 0;
  }
}

int com_p(char com[],profile data[],int i,int cr_soc){
  int n,l;
  char str[MAX_L],ss[MAX_C];
  if(com[3]=='-'){
    n=atoi(&com[4]);
    //printf("P_-%d\n",n);
    sprintf(ss,"P_-%d\n",n);
    send_message(ss,sizeof(ss),cr_soc);
    for(l=--i;l>=0;l--){
      /*printf("\nDATA[%d]\nID:%d\nNAME:%s\nDATE:%d-%d-%d\nADDR:%s\nNOTE:%s\n",
	     l,data[l].id,data[l].name,data[l].birth.year,data[l].birth.month,
	     data[l].birth.date,data[l].addr,data[l].note); */
      sprintf(str,"\nDATA[%d]\nID:%d\nNAME:%s\nDATE:%d-%d-%d\nADDR:%s\nNOTE:%s\n",l,data[l].id,data[l].name,data[l].birth.year,data[l].birth.month,data[l].birth.date,data[l].addr,data[l].note);
      send_message(str,sizeof(str),cr_soc);
      n--;
      if(!n){
	//send_message(ter,sizeof(ter),cr_soc);
	return 0;
      }
    }
  }
  else if(atoi(&com[3])){
    n=atoi(&com[3]);
    //printf("P_%d\n",n);
    sprintf(ss,"P_-%d\n",n);
    //send_message(ss,sizeof(ss),cr_soc);
    for(l=0;l<i;l++){
      /*printf("\nDATA[%d]\nID:%d\nNAME:%s\nDATE:%d-%d-%d\nADDR:%s\nNOTE:%s\n",
	     l,data[l].id,data[l].name,data[l].birth.year,data[l].birth.month,
	     data[l].birth.date,data[l].addr,data[l].note); */
      sprintf(str,"\nDATA[%d]\nID:%d\nNAME:%s\nDATE:%d-%d-%d\nADDR:%s\nNOTE:%s\n",l,data[l].id,data[l].name,data[l].birth.year,data[l].birth.month,
	     data[l].birth.date,data[l].addr,data[l].note);
      send_message(str,sizeof(str),cr_soc);
      n--;
      if(!n){
	//send_message(ter,sizeof(ter),cr_soc);
	return 0;
      }
    }
  }
  else{
    //printf("P_all\n");
    sprintf(ss,"P_all\n");
    send_message(ss,sizeof(ss),cr_soc);
    for(l=0;l<i;l++){
      /* printf("\nDATA[%d]\nID:%d\nNAME:%s\nDATE:%d-%d-%d\nADDR:%s\nNOTE:%s\n",
	     l,data[l].id,data[l].name,data[l].birth.year,data[l].birth.month,
	     data[l].birth.date,data[l].addr,data[l].note); */
      sprintf(str,"\nDATA[%d]\nID:%d\nNAME:%s\nDATE:%d-%d-%d\nADDR:%s\nNOTE:%s\n",l,data[l].id,data[l].name,data[l].birth.year,data[l].birth.month,data[l].birth.date,data[l].addr,data[l].note);
      send_message(str,sizeof(str),cr_soc);
    }
  }
  //send_message(ter,sizeof(ter),cr_soc);
  return 0;
}

int com_r(char com[],FILE *fp,profile data[],int *i,int cr_soc) {
  int l;
  char fname[MAX_C],str[MAX_C];
  sprintf(fname,"%s",&com[3]);
  subst(fname,'\n','\0',1);
  if ((fp = fopen(fname, "r")) == NULL) {
    //fprintf(stderr, "faild to open %s\n", fname);
    sprintf(str,"faild to open %s\n",fname);
    send_message(str,sizeof(str),cr_soc);
    return 1;
  }
  //printf("%s is opend\n",fname);
  sprintf(str,"%s is opend\n",fname);
  send_message(str,sizeof(str),cr_soc);
  for(l=*i;l<=MAX_P;){
    char buffer[MAX_L];
    if(fgets(buffer,sizeof(buffer),fp)==NULL)break;
    if(write_p(buffer,&data[l],cr_soc)){
      //printf("format error in the record:%d\n",l+1);
      sprintf(str,"format error in the record:%d\n",l+1);
      send_message(str,sizeof(str),cr_soc);
    }
    else{
      l++;
      if(l==MAX_P){
	//printf("%d records\n",l);
	sprintf(str,"%d records\n",l);
	send_message(str,sizeof(str),cr_soc);
      }
    }
  }
  *i=l;
  //printf("END\n");
  sprintf(str,"END\n");
  send_message(str,sizeof(str),cr_soc);
  fclose(fp);
  return 0;
}

int com_w(char com[],FILE *fp,profile data[],int i,int cr_soc){
  int l;
  char fname[MAX_C];
  char str[MAX_C];
  //printf("file name:");
  sprintf(str,"file name:");
  send_message(str,sizeof(str),cr_soc);
  sprintf(fname,"%s",&com[3]);
  subst(fname,'\n','\0',1);
  if((fp=fopen(fname,"w"))==NULL){
    //fprintf(stderr,"faild to open %s\n",fname);
    sprintf(str,"faild to open %s\n",fname);
    send_message(str,sizeof(str),cr_soc);
    return 1;
  }
  printf("%s is opend\n",fname);
  sprintf(str,"%s is opend\n",fname);
  send_message(str,sizeof(str),cr_soc);
  for(l=0;l<i;l++){
    fprintf(fp,"%d,%s,%d-%d-%d,%s,%s\n",data[l].id,data[l].name,
	    data[l].birth.year,data[l].birth.month,
	    data[l].birth.date,data[l].addr,data[l].note);
    /*sprintf(str,"%d,%s,%d-%d-%d,%s,%s\n",data[l].id,data[l].name,
	    data[l].birth.year,data[l].birth.month,
	    data[l].birth.date,data[l].addr,data[l].note);
	    send_message(str,sizeof(str),cr_soc);*/
  }
  //printf("END\n");
  sprintf(str,"finished\n",i);
  send_message(str,sizeof(str),cr_soc);
  fclose(fp);
  return 0;
} 

int com_f(char com[],profile data[],int i,int cr_soc){
  int l;
  char *str,buffer[MAX_C];
  sprintf(buffer,"%s",&(com[3]));
  subst(buffer,'\n','\0',1);
  for(l=0;l<i;l++){
    char birth[15],id[50];
    sprintf(birth,"%d-%d-%d",data[l].birth.year,data[l].birth.month,
	    data[l].birth.date);
    sprintf(id,"%d",data[l].id);
    if(strcmp(buffer,id)==0||
       strcmp(buffer,birth)==0||
       strcmp(buffer,data[l].name)==0||
       strcmp(buffer,data[l].addr)==0||
       strcmp(buffer,data[l].note)==0){
      /*printf("DATA[%d]\nID:%d\nNAME:%s\nDATE:%d-%d-%d\nADDR:%s\nNOTE:%s\n",
	     l,data[l].id,data[l].name,data[l].birth.year,data[l].birth.month,
	     data[l].birth.date,data[l].addr,data[l].note);*/
      data[l].flg=1;
    }
    else{
      data[l].flg=0;
    }
  }
  flg_print(data,i,cr_soc);
}

int com_s(profile data[],int i,char com[],int cr_soc){
  int n,s=i-1;
  char str[MAX_C];
  n=atoi(&com[3]);
  switch(n){
  case 1:
    id_sort(data,s,0);
    break;
  case 2:
    name_sort(data,s,0);
    break;
  case 3:
    birth_sort(data,s,0);
    break;
  case 4:
    addr_sort(data,s,0);
    break;
  case 5:
    note_sort(data,s,0);
    break;
  default:
    //printf("select a field\n");
    sprintf(str,"select a field\n");
    send_message(str,sizeof(str),cr_soc);
    break;
  }
}

int id_sort(profile data[],int scp_r,int scp_l){
  int r,l,std=(data[scp_r].id + data[scp_l].id)/2;
  r=scp_r;
  l=scp_l;
  profile tmp;
    for(;;){
      while(data[r].id>std){r--;}
      while(data[l].id<std){l++;}
    	if(r<=l){break;}
    	tmp=data[l];
    	data[l]=data[r];
    	data[r]=tmp;
    	r--;
    	l++;
    }
   if(scp_l<l-1){id_sort(data,l-1,scp_l);}
   if(r+1<scp_r){id_sort(data,scp_r,r+1);}
}

int name_sort(profile data[],int scp_r,int scp_l){
  int r,l;
  r=scp_r;
  l=scp_l;
  char *std;
  std=data[(scp_r + scp_l)/2].name;
  profile tmp;
    for(;;){
     while(strcmp(data[r].name,std)>0){r--;}
      while(strcmp(std,data[l].name)>0){l++;}
    	if(r<=l){break;}
    	tmp=data[l];
    	data[l]=data[r];
    	data[r]=tmp;
    	r--;
    	l++;
    }
    if(scp_l<l-1){name_sort(data,l-1,scp_l);}
    if(r+1<scp_r){name_sort(data,scp_r,r+1);}
}

int addr_sort(profile data[],int scp_r,int scp_l){
  int r,l;
  r=scp_r;
  l=scp_l;
  char *std;
  std=data[(scp_r + scp_l)/2].addr;
  profile tmp;
    for(;;){
      while(strcmp(data[r].addr,std)>0){r--;}
      while(strcmp(std,data[l].addr)>0){l++;}
    	if(r<=l){break;}
    	tmp=data[l];
    	data[l]=data[r];
    	data[r]=tmp;
    	r--;
    	l++;
    }
    if(scp_l<l-1){addr_sort(data,l-1,scp_l);}
    if(r+1<scp_r){addr_sort(data,scp_r,r+1);}
}

int note_sort(profile data[],int scp_r,int scp_l){
  int r,l;
  r=scp_r;
  l=scp_l;
  char *std;
  std=data[(scp_r + scp_l)/2].note;
  profile tmp;
    for(;;){
      while(strcmp(data[r].note,std)>0){r--;}
      while(strcmp(std,data[l].note)>0){l++;}
    	if(r<=l){break;}
    	tmp=data[l];
    	data[l]=data[r];
    	data[r]=tmp;
    	r--;
    	l++;
    }
    if(scp_l<l-1){note_sort(data,l-1,scp_l);}
    if(r+1<scp_r){note_sort(data,scp_r,r+1);}
}

int birth_sort(profile data[],int scp_r,int scp_l){
  int r,l;
  r=scp_r;
  l=scp_l;
  int std;
  std=btoi(data[(scp_r + scp_l)/2]);
  profile tmp;
    for(;;){
      while(btoi(data[r])>std){r--;}
      while(std>btoi(data[l])){l++;}
    	if(r<=l){break;}
    	tmp=data[l];
    	data[l]=data[r];
    	data[r]=tmp;
    	r--;
    	l++;
    }
    if(scp_l<l-1){birth_sort(data,l-1,scp_l);}
    if(r+1<scp_r){birth_sort(data,scp_r,r+1);}
}

int btoi(profile data){
  return (data.birth.year)*10000+(data.birth.month)*100+(data.birth.date);
}

int com_e(char com[],profile data[],int i,int cr_soc){
  int n,l;
  char buffer[RQ_SIZE],str[MAX_L];
  l=atoi(&com[3]);
  if(l>=i || 0>l){
    //printf("data[%c] is inexistent\n",com[3]);
    sprintf(str,"data[%c] is inexistent\n",com[3]);
    send_message(str,sizeof(str),cr_soc);
    return 0;
  }
  /*printf("\nDATA[%d]\nID:%d\nNAME:%s\nDATE:%d-%d-%d\nADDR:%s\nNOTE:%s\n",
	 l,data[l].id,data[l].name,data[l].birth.year,data[l].birth.month,
	 data[l].birth.date,data[l].addr,data[l].note);*/
  sprintf(str,"\nDATA[%d]\nID:%d\nNAME:%s\nDATE:%d-%d-%d\nADDR:%s\nNOTE:%s\n",
	 l,data[l].id,data[l].name,data[l].birth.year,data[l].birth.month,
	 data[l].birth.date,data[l].addr,data[l].note);
  send_message(str,sizeof(str),cr_soc);
  //printf("field:");
  sprintf(str,"field:%s",ter);
  send_message(str,sizeof(str),cr_soc);
  //fgets(buffer,sizeof(buffer),stdin);
  recv_message(buffer,cr_soc);
  if(buffer[0]=='\0'){n=0;}
  else {n=atoi(&buffer[0]);}
  switch(n){
  case 1:
    //printf("id:");
    sprintf(str,"id:%s",ter);
    send_message(str,sizeof(str),cr_soc);
    //fgets(buffer,sizeof(buffer),stdin);atoi(buffer);
    recv_message(buffer,cr_soc);
    if(buffer[0]=='\0'){printf("not a number\n");}
    else {data[l].id=atoi(buffer);}
    break;
  case 2:
    printf("NAME:hhhh");
    sprintf(str,"NAME:%s",ter);
    send_message(str,sizeof(str),cr_soc);
    //fgets(buffer,sizeof(buffer),stdin);
    recv_message(buffer,cr_soc);
    sprintf(data[l].name,"%s",buffer);
    subst(data[l].name,'\n','\0',1);
    printf("NAME:iiii:%s\n",buffer);
    break;  
  case 3:
    //printf("DATE:");
    sprintf(str,"DATE:%s",ter);
    send_message(str,sizeof(str),cr_soc);
    //fgets(buffer,sizeof(buffer),stdin);
    recv_message(buffer,cr_soc);
    if(subst(buffer,'-','\0',3)!=2){
      //printf("format error\n");
      sprintf(str,"format error\n");
      send_message(str,sizeof(str),cr_soc);
    }
    char *law[3];
    split(buffer,law,3);
    data[l].birth.year=atoi(law[0]);
    data[l].birth.month=atoi(law[1]);
    data[l].birth.date=atoi(law[2]);
    break;
  case 4:
    //printf("ADDR%s\n",data[l].addr);
    sprintf(str,"ADDR%s\n",data[l].addr);
    send_message(str,sizeof(str),cr_soc);
    send_message(ter,sizeof(ter),cr_soc);
    //fgets(buffer,sizeof(buffer),stdin);
    printf("b_if\n");
    if(recv_message(buffer,cr_soc)==-1){printf("recv faild\n");return 0;}
    sprintf(data[l].addr,"%s",buffer);
    subst(data[l].addr,'\n','\0',1);
    printf("b_break\n");
    break;
  case 5:
    //printf("NOTE%s\n",data[l].note);
    sprintf(str,"NOTE%s\n%s",data[l].note,ter);
    send_message(str,sizeof(str),cr_soc);
    //fgets(buffer,sizeof(buffer),stdin);
    recv_message(buffer,cr_soc);
    sprintf(data[l].note,"%s",buffer);
    subst(data[l].note,'\n','\0',1);
    break;
  case 6:
    //printf("FLG:%d\n",data[l].flg);
    sprintf(str,"FLG:%d\n%s",data[l].flg,ter);
    send_message(str,sizeof(str),cr_soc);
    //fgets(buffer,sizeof(buffer),stdin);
    recv_message(buffer,cr_soc);
    data[l].flg=atoi(buffer);
    break;
  default:
    //printf("select a field\n");
    sprintf(str,"select a field\n");
    send_message(str,sizeof(str),cr_soc);
    break;  
  }
  printf("b_return\n");
  return 0;
}

int com_d(profile data[],int *i,char com[]){
  int l,n=atoi(&com[3]);
  if(n==0 && com[3]!='0'){
    *i=0;
    return 0;
  }
  else{
    for(l=n;l+1<*i;l++){
      data[l]=data[l+1];
    }
    *i=*i-1;
    return 1;
  }

}

int com_a(profile data[],int *i,int cr_soc){
    int l;
    char str[MAX_C];
  for(l=*i;l<=MAX_P;){
    //printf("ID,NAME,DATE,ADDR,NOTE\n");
    sprintf(str,"ID,NAME,DATE,ADDR,NOTE\n");
    send_message(str,sizeof(str),cr_soc);
    send_message(ter,sizeof(ter),cr_soc);
    char buffer[MAX_L];
    //fgets(buffer,sizeof(buffer),stdin);
    recv_message(buffer,cr_soc);
    if(buffer[1]=='\0'){
      //printf("END\n");
      sprintf(str,"END\n",i);
      send_message(str,sizeof(str),cr_soc);
      break;
    }
    if(write_p(buffer,&data[l],cr_soc)){
      //printf("format error in the record:DATA[%d]\n",l);
      sprintf(str,"format error in the record:DATA[%d]\n",l);
      send_message(str,sizeof(str),cr_soc);
    }
    else{
      l++;
      if(l==MAX_P){
	//printf("%d records\n",l);
	sprintf(str,"%d records\n",l);
	send_message(str,sizeof(str),cr_soc);
      }
    }
  }
  *i=l;
  return 0;
}

int flg_print(profile data[],int i,int cr_soc){
  int l;
  char str[MAX_L];
  for(l=0;l<i;l++){
    if(data[l].flg>0){
      /*printf("\nDATA[%d]\nID:%d\nNAME:%s\nDATE:%d-%d-%d\nADDR:%s\nNOTE:%s\n",
	     l,data[l].id,data[l].name,data[l].birth.year,data[l].birth.month,
	     data[l].birth.date,data[l].addr,data[l].note);*/
      sprintf(str,"\nDATA[%d]\nID:%d\nNAME:%s\nDATE:%d-%d-%d\nADDR:%s\nNOTE:%s\n",l,data[l].id,data[l].name,data[l].birth.year,data[l].birth.month,data[l].birth.date,data[l].addr,data[l].note);
      send_message(str,sizeof(str),cr_soc);
    }
  }
}

