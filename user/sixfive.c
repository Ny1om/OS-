#include "kernel/types.h" 
#include "kernel/stat.h" 
#include "user/user.h" 

char *seps = " -\r\t\n./,"; 
void sixfive(int fd) 
{ 
char ch; 
int cur = 0, have = 0,pre=1;
while(read(fd, &ch, 1) == 1){ 
    if(ch >= '0' && ch <= '9'&&pre==1)
    {
        have=1,pre=1;
        cur=cur*10+(ch-'0');
    }
    else if(strchr(seps,ch))
    {
        if(have)
        {
            if(cur%5==0||cur%6==0)
            {
                printf("%d\n",cur);
            }
            cur=0,have=0;
        }
        pre=1;
    }
    else
    {
        cur=0,have=0,pre=0;
    }

    // TODO: 三种情况分别处理 
} 
if(have){
    if(cur % 5 == 0 || cur % 6 == 0){
      printf("%d\n", cur);
    }
  }
// TODO: 别忘了文件末尾这个隐式分隔符 
} 
int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "usage: sixfive files...\n");
    exit(1);
  }

  for(int i = 1; i < argc; i++){
    int fd = open(argv[i], 0);   // 0 表示只读
    if(fd < 0){
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      exit(1);
    }
    sixfive(fd);
    close(fd);
  }

  exit(0);
}