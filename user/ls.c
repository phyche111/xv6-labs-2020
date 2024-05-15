#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

//找到路径中的文件
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // 13-15找到/后的第一个字符
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;
  //当检测到文件是普通文件时，输出该文件的名称、类型、inode号和大小。
  case T_DIR://表示文件类型是目录
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);//strcpy函数用于将字符串从一个位置（源）复制到另一个位置（目标）。这里，它将path变量的内容复制到buf数组中。
    p = buf+strlen(buf);
    *p++ = '/';//*p++ = '/'先将p指向的位置设置为'/'，然后将p指针向前移动一个字符位置。这样，p现在指向斜杠之后的位置，准备接受下一个文件或目录名的复制。
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;
  //如果没有参数，列出当前目录下的所有文件
  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
// 简单示例
// ```
// /home/user/
// │
// ├── file1.txt
// ├── file2.txt
// └── documents/
//     ├── doc1.pdf
//     └── doc2.pdf
// ```
// 这里，“/home/user” 是一个目录，包含两个文件（`file1.txt`, `file2.txt`）和一个子目录（`documents`），子目录中有两个PDF文件（`doc1.pdf`, `doc2.pdf`）。
// ### 1. 程序启动
// 假设我们的程序命名为`myls`，它被调用来列出`/home/user`目录的内容：
// ```bash
// ./myls /home/user
// ```
// ### 2. 解析命令行参数
// `main`函数会解析命令行参数，确定需要列出哪些目录的内容。在本例中，参数是`/home/user`。
// ### 3. 执行`ls`函数
// `ls("/home/user")`函数被调用。
// #### 打开目录
// - 使用`open("/home/user", 0)`尝试以只读方式打开目录。
// - 如果成功，函数返回一个文件描述符，否则打印错误消息并退出。
// #### 读取目录内容
// - 使用`fstat`检查`/home/user`是不是目录。
// - 使用`read`函数在一个循环中逐个读取目录条目。对于每个有效的目录条目（即`de.inum`不为0）：
//   - 使用`memmove`和`memset`将条目名称追加到路径后，形成新的路径，如`/home/user/file1.txt`。
//   - 使用`stat`获取每个条目的详细信息（类型、inode号、大小）。
//   - 使用`printf`打印每个文件或目录的格式化名称和详细信息。
// 对于上述文件系统结构，输出可能会类似于：
// ```
// file1.txt <type> <inode> <size>
// file2.txt <type> <inode> <size>
// documents/ <type> <inode> <size>
// ```
// 其中`<type>`、`<inode>`和`<size>`将被替换为实际的文件类型、inode编号和文件大小。
// #### 子目录处理
// 如果`ls`函数在处理过程中遇到目录（如`documents`），则可以递归调用`ls`函数来处理该目录，不过在此简化版本中，它只列出顶级目录的内容。
// ### 4. 关闭文件描述符
// 处理完所有条目后，使用`close(fd)`关闭文件描述符。
// ### 5. 程序退出
// 执行完毕后，程序通过`exit(0)`正常退出。
// 这就是完整代码的基本执行流程，展示了如何逐步读取和处理一个目录下的文件和子目录条目的信息。在真实的实现中，还会考虑错误处理、资源管理和可能的递归目录访问等更复杂的情况。