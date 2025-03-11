#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "usage: xargs <command> [args...]\n");
        exit(1);
    }

    char *cmd = argv[1];
    char *cmd_argv[MAXARG];
    int i;
    for (i = 1; i < argc; i++) {
        cmd_argv[i - 1] = argv[i];
    }

    char buf[512];
    char *p = buf;
    int n;

    while ((n = read(0, p, 1)) > 0) {
        if (*p == '\n') {
            *p = 0;
            if (strlen(buf) == 0) continue;

            int pid = fork();
            if (pid < 0) {
                fprintf(2, "fork failed\n");
                exit(1);
            }

            if (pid == 0) {
                cmd_argv[argc - 1] = buf;
                cmd_argv[argc] = 0;
                exec(cmd, cmd_argv);
                fprintf(2, "exec %s failed\n", cmd);
                exit(1);
            } else {
                wait(0);
            }

            p = buf;
            *p = 0;
        } else {
            p++;
            if (p - buf >= sizeof(buf)) {
                fprintf(2, "xargs: input line too long\n");
                exit(1);
            }
        }
    }

    if (n < 0) {
        fprintf(2, "read failed\n");
        exit(1);
    }

    exit(0);
}