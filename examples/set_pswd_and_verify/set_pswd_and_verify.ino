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
*/
#include "ATSerial.h"
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
    #define debug  SerialUSB
    #define uart Serial
    //FingerPrint_KCT202<HardwareSerial,HardwareSerial> kct202;
    FingerPrint_KCT202<Uart, Serial_> kct202;
#elif defined(ARDUINO_ARCH_ESP32)
    #define debug Serial
    #define uart Serial2
    FingerPrint_KCT202<HardwareSerial,HardwareSerial> kct202;
    //FingerPrint_KCT202<Uart, Serial_> kct202;

#else
    #define debug  Serial
    SoftwareSerial uart(2, 3);
    FingerPrint_KCT202<SoftwareSerial, HardwareSerial> kct202;
#endif


void setup(void) {
    debug.begin(115200);
    kct202.begin(uart, debug);
    kct202.configModule(KCT202_CFG_SL, 3);
}

Protocol_oprt oprt;
uint8_t err_code = 0;
uint8_t param[40];
uint32_t param_len;


void loop() {

    kct202.controlBLN(KCT202_LED_BREATH, KCT202_LED_B);
    kct202.setPassWord(0x01020304);
    kct202.getCommonResponAndparse(err_code, param, param_len);
    delay(2000);

    //Verify the wrong password.
    kct202.verifyPassWord(0x01010101);
    kct202.getCommonResponAndparse(err_code, param, param_len);
    if (err_code) {
        debug.println("**************************************************************");
        debug.println("verify failed");
        debug.println("**************************************************************");
        kct202.controlBLN(KCT202_LED_BLINK, KCT202_LED_R);
    }
    delay(2000);

    //Verify the correct password
    kct202.verifyPassWord(0x01020304);
    kct202.getCommonResponAndparse(err_code, param, param_len);
    if (!err_code) {
        debug.println("**************************************************************");
        debug.println("verify succeed");
        debug.println("**************************************************************");
        kct202.controlBLN(KCT202_LED_ON, KCT202_LED_G);
    }
    delay(500);

    while (1);
}





