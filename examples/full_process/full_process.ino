/*

    Copyright (c) 2019 Seeed Technology Co., Ltd.
    Website    : www.seeed.cc
    Author     : downey
    Create Time: Jun 2019
    Change Log :

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/#include "ATSerial.h"
#include "Protocol.h"
#include "KCT202.h"


#if defined(ARDUINO_ARCH_AVR)
    #define debug  Serial
    SoftwareSerial uart(2, 3);
    FingerPrint_KCT202<SoftwareSerial, HardwareSerial> kct202;

#elif defined(ARDUINO_ARCH_SAM)
    #define debug  SerialUSB
    #define uart Serial
    //FingerPrint_KCT202<HardwareSerial,HardwareSerial> kct202;
    //FingerPrint_KCT202<HardwareSerial,Serial_> kct202;
    FingerPrint_KCT202<Uart, Serial_> kct202;
#elif defined(ARDUINO_ARCH_SAMD)
    #if defined(SEEED_XIAO_M0)
        #define debug  Serial
        #define uart Serial1
    #else
        #define debug  SerialUSB
        #define uart Serial
    #endif

    //FingerPrint_KCT202<HardwareSerial,HardwareSerial> kct202;
    FingerPrint_KCT202<Uart, Serial_> kct202;
#elif defined(ARDUINO_ARCH_ESP32)
    #if defined(CONFIG_IDF_TARGET_ESP32C3) ||  defined(CONFIG_IDF_TARGET_ESP32C6)
    #define debug Serial
    #define uart Serial1
    FingerPrint_KCT202<HardwareSerial,HWCDC> kct202;
    //FingerPrint_KCT202<Uart, Serial_> kct202;
    #elif defined(CONFIG_IDF_TARGET_ESP32S3)
    #define debug Serial
    #define uart Serial2
    FingerPrint_KCT202<HardwareSerial,HWCDC> kct202;
    #else
    #define debug Serial
    #define uart Serial2
    FingerPrint_KCT202<HardwareSerial,HardwareSerial> kct202;
    //FingerPrint_KCT202<Uart, Serial_> kct202;
    #endif

#elif defined(NRF52840_XXAA)
    #define debug  Serial
    SoftwareSerial uart(D6, D7);
    FingerPrint_KCT202<SoftwareSerial, Adafruit_USBD_CDC> kct202;
#else
    #define debug  Serial
    SoftwareSerial uart(2, 3);
    FingerPrint_KCT202<SoftwareSerial, HardwareSerial> kct202;
#endif



Protocol_oprt oprt;
uint8_t err_code = 0;
uint8_t param[10];
uint32_t param_len;

void setup(void) {
    debug.begin(115200);
    kct202.begin(uart, debug);
    kct202.configModule(KCT202_CFG_SL, 3);
}


void loop(void) {
    kct202.controlBLN(KCT202_LED_BREATH, KCT202_LED_B);
    uint16_t finger_num = 0;
    //Specify the finger print ID 0x01,collect four times finger-print。
    //If there is finger-print which ID is 0x01 already,Then override it.
    kct202.autoRegisterFingerPrint(1, 4, LED_OFF_AFTER_GET_GRAGH |
                                   PRETREATMENT_GRAGH | NOT_RET_FOR_EVERY_STEP | OVERRIDE_CURR_FINGER_PRINT);
    debug.println(" ");
    debug.println("Usage:");
    debug.println("    Please put your finger on the touchpad, from green light which on the touchpad lights on till it lights off.");
    debug.println("    Repeat for 4 times.");
    debug.println("    If the red light lights on, it indicates that an error occurred.");
    debug.println(" ");
    debug.println(" ");
    debug.println(" ");
    if (0 == kct202.getRegisterResponAndparse()) {
        debug.println("Register ok!");
        kct202.controlBLN(KCT202_LED_ON, KCT202_LED_G);
    }
    else {
        kct202.controlBLN(KCT202_LED_BLINK, KCT202_LED_R);
    }
    delay(1000);
    kct202.controlBLN(KCT202_LED_BREATH, KCT202_LED_B | KCT202_LED_G);
    kct202.autoVerifyFingerPrint(CHECK_ALL_FINGER_TEMP,
                                 LED_OFF_AFTER_GET_GRAGH | PRETREATMENT_GRAGH | NOT_RET_FOR_EVERY_STEP);
    debug.println(" ");
    debug.println("Please put your finger on the touchpad, from green light which on the touchpad lights on till it lights off.");
    debug.println("To verify your finger print.");
    debug.println(" ");
    debug.println(" ");
    debug.println(" ");

    if (0 == kct202.getVerifyResponAndparse(finger_num)) {
        debug.println("Verify ok!");
        debug.print("Your finger temp id = ");
        debug.println(finger_num, HEX);
        kct202.controlBLN(KCT202_LED_ON, KCT202_LED_G);
    }
    else {
        kct202.controlBLN(KCT202_LED_BLINK, KCT202_LED_R);
    }

    delay(1000);
    kct202.cleanAllFingerPrint();

    if (0 == kct202.getCommonResponAndparse(err_code, param, param_len)) {
        debug.println("Delete ok!");
    }

    while (1);
}


