#include "chardev.h"

#include <fcntl.h>      /* open */
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <stdlib.h>
#include <stdio.h>


ioctl_set_msg(int file_desc, char *message) {
    int ret_val;

    ret_val = ioctl(file_desc, IOCTL_SET_MSG, message);

    if (ret_val < 0) {
        printf("Ошибка при вызове ioctl_set_msg: %d\n", ret_val);
        exit(-1);
    }
}

ioctl_get_msg(int file_desc) {
    int ret_val;
    char message[100];
    ret_val = ioctl(file_desc, IOCTL_GET_MSG, message);

   

    if (ret_val < 0) {
        printf("Ошибка при вызове ioctl_get_msg: %d\n", ret_val);
        exit(-1);
    }

    printf("Получено сообщение (get_msg): %s\n", message);
}

void main(int argc, char* argv[]) {
    int file_desc, ret_val;
    char *msg = "Это сообщение передается через ioctl\n";

    // printf("%x\n",  IOCTL_SET_MSG);  

    file_desc = open(DEVICE_FILE_NAME, 0);
    if (file_desc < 0) {
        printf("Невозможно открыть файл устройства: %s\n", DEVICE_FILE_NAME);
        exit(-1);
    }

    if (argc == 1) {
        ioctl_get_msg(file_desc);
    }
    else {
        ioctl_set_msg(file_desc, argv[1]);
    }

    close(file_desc);
}

