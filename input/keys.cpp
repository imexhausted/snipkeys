#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <iostream>
#include "Config.hpp"

#define EVENT_DEVICE "/dev/input/event8" // Change this to match your input event device

void send_key_event(int fd, int code, int value) {
    struct input_event event;
    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_KEY;
    event.code = code;
    event.value = value;
    write(fd, &event, sizeof(event));
}

int main() {
    int fd = open(EVENT_DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open input event device");
        return 1;
    }

    Config config("./conf/settings.config", "./conf/keys.config");

    // Simulate key press event

    // int a = 0;
    // while (a < 100){
    //     send_key_event(fd, KEY_A, 1); // Press the key
    //     usleep(100000); // Wait for 100ms
    //     send_key_event(fd, KEY_A, 0); // Release the key
    //     a++;
    // }

    struct input_event event;
    while (1) {
        ssize_t bytes = read(fd, &event, sizeof(event));
        if (bytes < sizeof(event)) {
            printf("Error");
            // perror("Failed to read input event");
            close(fd);
            return 1;
        }

        if (event.value == 1) {
            //printf("Key pressed: %d\n", event.code);
            std::cout << event.type << std::endl;
            std::cout << config.getKey(event.code) << std::endl; 
        }
    }
    close(fd);
    return 0;
}