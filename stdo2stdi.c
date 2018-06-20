#include <stdio.h>
#include <fcntl.h>

main()
{
  char buff[128];
  int num_c = 127;
  while(num_c == 127){
    num_c = read(0,buff,127);
    buff[num_c] = '\0';
    write(1,buff,num_c);
  }
}
