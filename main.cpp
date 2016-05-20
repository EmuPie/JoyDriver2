#include <iostream>
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#define SPICLK 18
#define SPIMISO 23
#define SPIMOSI 24
#define SPICS 25

namespace JoyDriver {

    void loop();

    int fd, wSyn;
    struct input_event keyEv, synEv;

    bool running = true;

    void tick() {
        digitalWrite(SPICLK, 1);
        digitalWrite(SPICLK, 0);
    }

    int readAdc(int adcnum) {
        if (adcnum > 3 || adcnum < 0) {
            printf("Called non existent channel!");
            return -1;
        }

        int adcout = 0;

        digitalWrite(SPICS, 1);

        digitalWrite(SPICLK, LOW);
        digitalWrite(SPICS, LOW);

        __u8 commandout = 0;

        if (adcnum == 0) {
            commandout = 12;
        } else if (adcnum == 1) {
            commandout = 14;
        } else if (adcnum == 2) {
            commandout = 13;
        } else if (adcnum == 3) {
            commandout = 15;
        }

        commandout <<= 4;
        for (int i = 0; i < 4; ++i) {
            if (commandout & 0x80) {
                digitalWrite(SPIMOSI, HIGH);
            } else {
                digitalWrite(SPIMOSI, LOW);
            }
            commandout <<= 1;
            tick();
        }

        for (int j = 0; j < 12; ++j) {
            tick();
            adcout <<= 1;
            if (digitalRead(SPIMISO) == 1) {
                adcout |= 0x1;
            }
        }

        adcout >>= 1;
        return adcout;
    }

    void pressKey(__u16 key) {
        keyEv.code = key;
        keyEv.value = 1;
        write(fd, &keyEv, sizeof(keyEv));
        wSyn = 1;
    }

    void releaseKey(__u16 key) {
        keyEv.code = key;
        keyEv.value = 0;
        write(fd, &keyEv, sizeof(keyEv));
        wSyn = 1;
    }

#define X1 0
#define Y1 1
#define X2 2
#define Y2 3


    void loop() {

        bool isW = false, isA = false, isD = false, isS = false, isUp = false, isDown = false, isLeft = false, isRight = false;


        if ((fd = open("/dev/input/event0", O_WRONLY | O_NONBLOCK)) <
            0) /*std::cerr << "Could not open /dev/input/event0" << std::endl*/;

        memset(&keyEv, 0, sizeof(keyEv));
        keyEv.type = EV_KEY;
        memset(&synEv, 0, sizeof(synEv));
        synEv.type = EV_SYN;
        synEv.code = SYN_REPORT;
        synEv.value = 0;

        while (running) {

            int adc = readAdc(X1);

            if (adc < 900) {
                if (!isA) {
                    pressKey(KEY_A);
                    isA = true;
                }
            } else {
                if (isA) {
                    releaseKey(KEY_A);
                    isA = false;
                }
            }

            if (adc > 1148) {
                if (!isD) {
                    pressKey(KEY_D);
                    isD = true;
                }
            } else {
                if (isD) {
                    releaseKey(KEY_D);
                    isD = false;
                }
            }

            adc = readAdc(Y1);

            if (adc < 900) {
                if (!isS) {
                    pressKey(KEY_S);
                    isS = true;
                }
            } else {
                if (isS) {
                    releaseKey(KEY_S);
                    isS = false;
                }
            }

            if (adc > 1148) {
                if (!isW) {
                    pressKey(KEY_W);
                    isW = true;
                }
            } else {
                if (isW) {
                    releaseKey(KEY_W);
                    isW = false;
                }
            }

            adc = readAdc(X2);

            if (adc < 900) {
                if (!isLeft) {
                    pressKey(KEY_LEFT);
                    isLeft = true;
                }
            } else {
                if (isLeft) {
                    releaseKey(KEY_LEFT);
                    isLeft = false;
                }
            }

            if (adc > 1148) {
                if (!isRight) {
                    pressKey(KEY_RIGHT);
                    isRight = true;
                }
            } else {
                if (isRight) {
                    releaseKey(KEY_RIGHT);
                    isRight = false;
                }
            }

            adc = readAdc(Y2);

            if (adc < 900) {
                if (!isDown) {
                    pressKey(KEY_DOWN);
                    isDown = true;
                }
            } else {
                if (isDown) {
                    releaseKey(KEY_DOWN);
                    isDown = false;
                }
            }

            if (adc > 1148) {
                if (!isUp) {
                    pressKey(KEY_UP);
                    isUp = true;
                }
            } else {
                if (isUp) {
                    releaseKey(KEY_UP);
                    isUp = false;
                }
            }

            if (wSyn) write(fd, &synEv, sizeof(synEv));
            wSyn = 0;
        }
    }
}

int main() {
    //std::cout << "Driver Starting!" << std::endl;
    printf("Driver starting!");
    wiringPiSetupGpio();

    pinMode(SPICLK, OUTPUT);
    pinMode(SPIMOSI, OUTPUT);
    pinMode(SPIMISO, INPUT);
    pinMode(SPICS, OUTPUT);

    JoyDriver::loop();

    return 0;
}