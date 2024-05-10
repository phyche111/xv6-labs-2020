// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"

// int main(void) {
//   int p[2];
//   char buf[2];
//   char* parmsg = "c";
//   char* chimsg = "d";
//   pipe(p);
//   if (fork() == 0) {
//     if (read(p[0], buf, 1) != 1) {
//       fprintf(2, "can not read from parent\n");
//       exit(1);
//     }
//     printf("child receive\n", buf[0]);
//     close(p[0]);
//     printf("%d:receive ping\n", getpid());
//     if (write(p[1], chimsg, 1) != 1) {
//       fprintf(2, "can not write to parent|n");
//     }
//     close(p[1]);
//     exit(0);
//   } else {
//     if (write(p[1], parmsg, 1) != 1) {
//       fprintf(2, "can not write to child|n");
//       exit(1);
//     }
//     wait(0);
//     if (read(p[0], buf, 1) != 1) {
//       fprintf(2, "Can't read from child!");
//       exit(1);
//     }
//     printf("parent receive: %c\n", buf[0]);
//     close(p[0]);
//     printf("%d: received pong\n", getpid());
//     exit(0);
//   }
// }

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    int p[2];
    char buf[2];
    char *parmsg = "a";
    char *chimsg = "b";
    pipe(p);

    if (fork() == 0){
        if (read(p[0], buf, 1) != 1){
            fprintf(2, "Can't read from parent!\n");
            exit(1);
        }
        printf("child receive: %c\n", buf[0]);
        close(p[0]);

        printf("%d: received ping\n", getpid());
        if (write(p[1], chimsg, 1) != 1){
            fprintf(2, "Can't write to parent!");
        }
        close(p[1]);
        
        exit(0);
    }else{
        if (write(p[1], parmsg, 1) != 1){
            fprintf(2, "Can't write to child!\n");
            exit(1);
        }
        close(p[1]);

        wait(0);

        if (read(p[0], buf, 1) != 1){
            fprintf(2, "Can't read from child!");
            exit(1);
        }
        printf("parent receive: %c\n", buf[0]);
        close(p[0]);
        printf("%d: received pong\n", getpid());

        exit(0);
    }
}