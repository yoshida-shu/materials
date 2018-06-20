#include <stdio.h>
#include <fcntl.h>

main()
{
  int fd;
  char buff[128];
  int num_c = 127;

  fd = open("test.txt", O_WRONLY);
  if (fd == -1)
    {
      //printf("error:file open\n");
      write(2,"error:file open\n",16);
      return;
    }
  while(num_c == 127){
    num_c = read(0,buff,127);
    if (num_c == -1)
      {
	//printf("error:file write\n");
	write(2,"error:file write\n",17);
      }
    else
      {
	buff[num_c] = '\0';
	write(fd, buff, num_c);
      }
  }
  close(fd);
}
