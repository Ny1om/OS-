#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  // 申请 4 页内存，提高拿到 secret 页面的概率
  char *buf = sbrk(4096 * 4);
  if(buf == (char*)-1){
    printf("sbrk failed\n");
    exit(1);
  }

  // 扫描所有页面，打印所有长度 >= 3 的字母数字串
  for(int i = 0; i < 4096 * 4; i++){
    if((buf[i] >= '0' && buf[i] <= '9') ||
       (buf[i] >= 'A' && buf[i] <= 'Z') ||
       (buf[i] >= 'a' && buf[i] <= 'z')){
      int j = 0;
      while(i + j < 4096 * 4 &&
            ((buf[i+j] >= '0' && buf[i+j] <= '9') ||
             (buf[i+j] >= 'A' && buf[i+j] <= 'Z') ||
             (buf[i+j] >= 'a' && buf[i+j] <= 'z'))){
        j++;
      }
      if(j >= 3){
        for(int k = 0; k < j; k++)
          printf("%c", buf[i+k]);
        printf("\n");
      }
      i += j;
    }
  }
  exit(0);
}