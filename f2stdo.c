#include <stdio.h>
#include <fcntl.h>

main()
{
  int     fd;
  char    buff[128];
  int     num_c = 127;
  
  fd = open("test.txt", O_RDONLY);
  if (fd == -1)
    {
      //printf("error:file open\n");
      write(2,"error:file open\n",16);
      return;
    }
  while(num_c == 127){
    num_c = read(fd, buff, 127);
    if (num_c == -1)
      {
      write(2,"error:file read\n",16);
      //printf("error:file read\n");
      }
    else
    {
      buff[num_c] = '\0';
      write(1,buff,num_c);
      //printf("read:%d - %s\n", num_c, buff);
    }
  }
  close(fd);
}
