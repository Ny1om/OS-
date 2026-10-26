#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

char *target;
char *exec_argv[MAXARG];      // exec 用的参数数组
int exec_argc = 0;            // exec 参数个数

char* fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[DIRSIZ] = '\0';
  return buf;
}

void run_exec(char *path)
{
  // 构造 argv：命令参数 + 文件路径 + 0
  char *argv[MAXARG];
  int i;
  for(i = 0; i < exec_argc; i++){
    argv[i] = exec_argv[i];
  }
  argv[i++] = path;
  argv[i] = 0;

  if(fork() == 0){
    exec(argv[0], argv);
    fprintf(2, "find: exec %s failed\n", argv[0]);
    exit(1);
  } else {
    wait(0);
  }
}

void find(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if(strcmp(fmtname(path), target) == 0){
      run_exec(path);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(strcmp(de.name, target) == 0){
        run_exec(buf);
      }
      find(buf);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;
  int exec_pos = -1;

  if(argc < 3){
    fprintf(2, "usage: find dir name [-exec cmd args...]\n");
    exit(1);
  }

  // 找 -exec 的位置
  for(i = 3; i < argc; i++){
    if(strcmp(argv[i], "-exec") == 0){
      exec_pos = i;
      break;
    }
  }

  target = argv[2];

  if(exec_pos >= 0){
    // -exec 后面的参数拷进 exec_argv
    for(i = exec_pos + 1; i < argc; i++){
      exec_argv[exec_argc++] = argv[i];
    }
    if(exec_argc == 0){
      fprintf(2, "find: -exec needs a command\n");
      exit(1);
    }
  } else {
    // 没有 -exec，默认用 echo 打印路径
    exec_argv[0] = "echo";
    exec_argc = 1;
  }

  find(argv[1]);
  exit(0);
}