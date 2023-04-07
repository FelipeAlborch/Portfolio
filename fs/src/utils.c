#include <utils.h>
#include <stdio.h>
#include <unistd.h>

void print_cwd() {
    printf("PWD = %s\n", getcwd(NULL, 0));
}