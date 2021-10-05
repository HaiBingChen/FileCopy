//
// Created by user on 2021/10/2.
//
#include <unistd.h>
#include "stdio.h"
#include "Sockect.h"

int main(int argc, const char **argv) {
    printf("File Copy Server start\n");

    StartSockectServer(1234);

    while (1) {
        pause();
    }
}