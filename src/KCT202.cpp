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
#include "KCT202.h"



/** Protocol format:
 * *********************************************************************************************
    CMD pack:
     header     chip_address    pack_id     length     directive   paras    checkSum
       2B           4B            1B         2B          1B        ...        2B
     0xFE01  FFFFFFFF(default)   0x01         N          x         ...        xx
 * ********************************************************************************************
 * */


/** The parameters of auto-register fingerprint template protocol.
    Note: The param contain directive field.
    directive : fixed 0x31,indicates auto-register fingerprint template
    finger-ID ：The u16 type unique ID for fingerprint.
    record_cnt : how many times you should put on your finger, to register one template.
    param : control bytes(u16),get more detail by referring  to manual.
 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::generateAutoRegisterParamData(uint8_t directive, uint16_t finger_ID,
        uint8_t record_cnt, uint16_t param, uint8_t* result_data) {
    if (NULL == result_data) {
        return -1;
    }
    result_data[0] = directive;
    result_data[1] = finger_ID >> 8;
    result_data[2] = finger_ID;
    result_data[3] = record_cnt;
    result_data[4] = param >> 8;
    result_data[5] = param;
    return 0;
}


/** The parameters of auto-verify fingerprint template protocol.
    Note: The param contain directive field.
    directive : fixed 0x32,indicates auto-verify fingerprint template
    finger-ID ：The u16 type unique ID for fingerprint.
    param : control bytes(u16),get more detail by referring  to manual.
 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::generateAutoVerifyParamData(uint8_t directive, uint16_t finger_ID, uint16_t param,
        uint8_t* result_data) {
    if (NULL == result_data) {
        return -1;
    }
    result_data[0] = directive;
    result_data[1] = 0x00;
    result_data[2] = finger_ID >> 8;
    result_data[3] = finger_ID;
    result_data[4] = param >> 8;
    result_data[5] = param;
    return 0;
}


/** The auto-register fingerprint template protocol.
    finger-ID ：The u16 type unique ID for fingerprint.
    record_cnt : how many times you should put on your finger, to register one template.
    param : control bytes(u16),get more detail by referring  to manual.
 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::autoRegisterFingerPrint(uint16_t finger_ID, uint8_t record_cnt, uint16_t param) {
    /*Protocol class*/
    Protocol pro;
    /*Protocol operations*/
    Protocol_oprt pro_oprt;

    uint8_t pro_data[30] = {0};

    uint32_t pro_len = 0;
    /*directive_code + record_cnt + finger_ID + param*/
    uint8_t data[6] = {0};
    generateAutoRegisterParamData(AUTO_REGISTER_FINGER_TEMP, finger_ID, record_cnt, param, data);

    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);

    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    //this->printHexArray(pro_data,pro_len);
    this->binWrite(pro_data, pro_len);
    return 0;
}


/** The auto-verify fingerprint template protocol.
    finger-ID ：The u16 type unique ID for fingerprint.
    param : control bytes(u16),get more detail by referring  to manual.
 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::autoVerifyFingerPrint(uint16_t finger_ID, uint16_t param) {
    Protocol pro;
    Protocol_oprt pro_oprt;
    uint8_t pro_data[30] = {0};
    uint32_t pro_len = 0;
    /*directive_code + security_level(fixd 0x00) + finger_ID + param*/
    uint8_t data[6] = {0};
    generateAutoVerifyParamData(AUTO_VERIFY_FINGER_TEMP, finger_ID, param, data);
    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);
    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    this->printHexArray(pro_data, pro_len);
    this->binWrite(pro_data, pro_len);
    return 0;
}


/** The parameters of delete fingerprint template protocol.
    Note: The param contain directive field.
    directive : fixed 0x0c,indicates delete fingerprint template num.
    finger-ID ：The u16 type unique ID for fingerprint,delete from finger-id to (finger-id+del_cnt).
    del_cnt : how many fingerprint templates to be deleted.
 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::deleteParamData(uint8_t directive, uint16_t finger_ID, uint16_t del_cnt,
        uint8_t* result_data) {
    if (NULL == result_data) {
        return -1;
    }
    result_data[0] = directive;
    result_data[1] = finger_ID >> 8;
    result_data[2] = finger_ID;
    result_data[3] = del_cnt >> 8;
    result_data[4] = del_cnt;
    return 0;
}


/** The delete fingerprint template protocol.
    finger-ID ：The u16 type unique ID for fingerprint,delete from finger-id to (finger-id+del_cnt).
    del_cnt : how many fingerprint templates to be deleted.
 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::DeleteFingerPrint(uint16_t finger_ID, uint8_t del_cnt) {
    Protocol pro;
    Protocol_oprt pro_oprt;
    uint8_t pro_data[30] = {0};
    uint32_t pro_len = 0;
    /*directive_code + finger ID + del_cnt*/
    uint8_t data[5] = {0};
    deleteParamData(AUTO_DELETE_FINGER_TEMP, finger_ID, del_cnt, data);
    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);
    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    this->printHexArray(pro_data, pro_len);
    this->binWrite(pro_data, pro_len);
    return 0;
}


/** Clean the templates library,delete all registered fingerprint.
 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::cleanAllFingerPrint() {
    Protocol pro;
    Protocol_oprt pro_oprt;
    uint8_t pro_data[30] = {0};
    uint32_t pro_len = 0;
    /*directive_code*/
    uint8_t data[1] = {CLEAN_ALL_FINGER_TEMP};
    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);
    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    this->printHexArray(pro_data, pro_len);
    this->binWrite(pro_data, pro_len);
    return 0;
}

/** enter sleep mode.

 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::enterSleepModeFingerPrint() {
    Protocol pro;
    Protocol_oprt pro_oprt;
    uint8_t pro_data[30] = {0};
    uint32_t pro_len = 0;
    /*directive_code*/
    uint8_t data[1] = {ENTER_SLEEP_MODE};
    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);
    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    this->printHexArray(pro_data, pro_len);
    this->binWrite(pro_data, pro_len);
    return 0;
}



template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::readValidTempCountFingerPrint() {
    Protocol pro;
    Protocol_oprt pro_oprt;
    uint8_t pro_data[30] = {0};
    uint32_t pro_len = 0;
    /*directive_code*/
    uint8_t data[1] = {READ_VALID_TEMP_CNT};
    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);
    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    //this->printHexArray(pro_data,pro_len);
    this->binWrite(pro_data, pro_len);
    return 0;
}


/** Read index table of registered fingerprint template library.
    The max fingerprint templates num is 1024.
    but .... for this version,onlu support to read the page 0(0~255,total four pages,0~3).

 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::readIndexTableFingerPrint(uint8_t page) {
    Protocol pro;
    Protocol_oprt pro_oprt;
    uint8_t pro_data[30] = {0};
    uint32_t pro_len = 0;
    /*directive_code*/
    uint8_t data[2] = {READ_INDEX_TABLE_TEMP};
    data[1] = page;
    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);
    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    //this->printHexArray(pro_data,pro_len);
    this->binWrite(pro_data, pro_len);
    return 0;
}


/** Invoke this function after readIndexTableFingerPrint() and getCommonResponAndparse(),parse the data which recieved from getCommonResponAndparse().
    data : The data that sensor return back.
    data_len :
    page : The page which read.

 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::getFingerTempID(uint8_t* data, uint32_t data_len, uint8_t page) {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 8; j++) {
            if (data[i] >> j & 0x01) {
                this->getDebugSerial()->print("ID ");
                this->getDebugSerial()->print(page * 256 + i * 8 + j);
                this->getDebugSerial()->println(" is registered");
            }
        }
    }
    return 0;
}


/** The parameters of set password protocol.
    Note: The param contain directive field.
    directive : fixed 0x12,indicates set pswd protocol.
    pswd ：u32 type password.
 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::generateSetPswdParamData(uint8_t directive, uint32_t pswd, uint8_t* result_data) {
    if (NULL == result_data) {
        return -1;
    }
    result_data[0] = directive;
    result_data[1] = pswd >> 24;
    result_data[2] = pswd >> 16;
    result_data[3] = pswd >> 8;
    result_data[4] = pswd;
    return 0;
}

/** The set password protocol.
    pswd ：u32 type password.
 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::setPassWord(uint32_t pswd) {
    Protocol pro;
    Protocol_oprt pro_oprt;
    uint8_t pro_data[30] = {0};
    uint32_t pro_len = 0;
    /*directive_code*/
    uint8_t data[5] = {0};
    generateSetPswdParamData(SET_PASSWORD, pswd, data);
    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);
    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    this->printHexArray(pro_data, pro_len);
    this->binWrite(pro_data, pro_len);
    return 0;
}


/** The verify password protocol.
    pswd ：u32 type password.
 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::verifyPassWord(uint32_t pswd) {
    Protocol pro;
    Protocol_oprt pro_oprt;
    uint8_t pro_data[30] = {0};
    uint32_t pro_len = 0;
    /*directive_code*/
    uint8_t data[5] = {0};
    generateSetPswdParamData(VERIFY_PASSWORD, pswd, data);
    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);
    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    this->printHexArray(pro_data, pro_len);
    this->binWrite(pro_data, pro_len);
    return 0;
}


/** Cancel the last directive,in most case, we can send a new directive to override the last directive.
    but for other directive,like autoregister, it should collect four fingerprint,if we have put on finger twice and then want to cancel it,we must use this directive.
    pswd ：u32 type password.
 **/
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::cancelAction() {
    Protocol pro;
    Protocol_oprt pro_oprt;
    uint8_t pro_data[30] = {0};
    uint32_t pro_len = 0;
    /*directive_code*/
    uint8_t data[1] = {CANCEL_ACTION};
    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);
    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    this->printHexArray(pro_data, pro_len);
    this->binWrite(pro_data, pro_len);
    return 0;
}


/** We send cmd with serial, and then the sensor response.
    So we have to wait for response after send a cmd.This function uses a block way to wait for response. The function is waitForDataTillRespon();
    Alternatively,you can use a unblock(query in a loop) way to do this,it up to you.
    This function to waiting for the register directive's response and parse the data , print the result.
 * */
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::getRegisterResponAndparse() {
    uint8_t err_code = 0;
    uint8_t ret_param[10];
    uint32_t ret_param_len = 0;
    uint8_t data[50] = {0};
    uint32_t data_len = 0;
    if (false == this->waitForDataTillRespon(data, data_len)) {
        return -1;
    }
    if (Protocol_oprt::checkRecvPack(data, data_len, err_code, ret_param, ret_param_len) < 0) {
        return -1;
    }
    if (err_code != 0x00) {
        if (0x22 ==  err_code) {
            this->getDebugSerial()->println("***********************************************************");
            this->getDebugSerial()->println("This finger-print ID has been registered already.");
            this->getDebugSerial()->println("***********************************************************");
        } else {
            this->getDebugSerial()->print("Register failed,error code = 0x");
            this->getDebugSerial()->print(err_code, HEX);
            this->getDebugSerial()->println(" ");

            this->getDebugSerial()->print("Ret param = ");
            this->printHexArray(ret_param, ret_param_len);
            this->getDebugSerial()->println(" ");

            this->getDebugSerial()->println("Please check the error code through datasheet!");
        }
        return -1;
    }
    return 0;
}



template <class T, class T1>
uint16_t FingerPrint_KCT202<T, T1>::getValidTempCntFromRsp(uint8_t* param) {
    return Protocol_oprt::mergeU8ToU16(param[0], param[1]);
}


/** We send cmd with serial, and then the sensor response.
    So we have to wait for response after send a cmd.This function uses a block way to wait for response. The function is waitForDataTillRespon();
    Alternatively,you can use a unblock(query in a loop) way to do this,it up to you.
    This function to waiting for the verify directive's response and parse the data , print the result.
    finger_num : The finger id that matched.
 * */
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::getVerifyResponAndparse(uint16_t& finger_num) {
    uint8_t err_code = 0;
    uint8_t ret_param[10];
    uint32_t ret_param_len = 0;
    uint8_t data[50] = {0};
    uint32_t data_len = 0;
    if (false == this->waitForDataTillRespon(data, data_len)) {
        return -1;
    }
    if (Protocol_oprt::checkRecvPack(data, data_len, err_code, ret_param, ret_param_len) < 0) {
        return -1;
    }
    if (err_code != 0x00) {
        if (0x09 == err_code) {
            this->getDebugSerial()->println("***********************************************************");
            this->getDebugSerial()->println(" Can't find your fingerprint in templates library.");
            this->getDebugSerial()->println("***********************************************************");
        } else {
            this->getDebugSerial()->print("Verify failed,error code = 0x");
            this->getDebugSerial()->print(err_code, HEX);
            this->getDebugSerial()->println(" ");

            this->getDebugSerial()->print("Ret param = ");
            this->printHexArray(ret_param, ret_param_len);
            this->getDebugSerial()->println(" ");

            this->getDebugSerial()->println("Please check the error code through datasheet!");

        }
        return -1;
    }
    finger_num  |= (uint16_t)ret_param[1] << 8;
    finger_num  |=  ret_param[2];
    return 0;
}


/** We send cmd with serial, and then the sensor response.
    So we have to wait for response after send a cmd.This function uses a block way to wait for response. The function is waitForDataTillRespon();
    Alternatively,you can use a unblock(query in a loop) way to do this,it up to you.
    This function to waiting for the common(except autoregister& autoverify) directive's response and parse the data , print the result.
    error_code : The status code that returned.
    param : The status param that returned,get more detail by referring manual for the speciefed directive.
    param_len :
 * */
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::getCommonResponAndparse(uint8_t& error_code, uint8_t* param, uint32_t& param_len) {
    uint8_t err_code = 0;
    uint8_t data[50] = {0};
    uint32_t data_len = 0;
    if (false == this->waitForDataTillRespon(data, data_len)) {
        return -1;
    }
    if (Protocol_oprt::checkRecvPack(data, data_len, err_code, param, param_len) < 0) {
        return -1;
    }
    error_code = err_code;
    if (0 == err_code) {
        this->getDebugSerial()->println("Operation succed.");
        return 0;
    } else {
        this->getDebugSerial()->print("error code = 0x");
        this->getDebugSerial()->print(err_code, HEX);
        this->getDebugSerial()->println(" ");

        this->getDebugSerial()->print("Ret param = ");
        this->printHexArray(param, param_len);
        this->getDebugSerial()->println(" ");

        this->getDebugSerial()->println("Please check the error code through datasheet!");

        return -1;
    }
    return -1;
}

/** Send serial cmd to control LED, and then waiting for response.
    state : The state of led, referring to manual
    color : The color of led, using "|" to get composite color
 * */
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::controlBLN(uint8_t state, uint8_t color)
{
    Protocol pro;
    Protocol_oprt pro_oprt;
    uint8_t pro_data[20] = {0};
    uint32_t pro_len = 0;
    /*directive_code + funtion code + start color + end color + cycle times */
    uint8_t data[5] = {CONTROL_LED, state, color, color, 0x00};
    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);
    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    this->binWrite(pro_data, pro_len);

    uint8_t ret_data[13] = {0};
    uint32_t ret_data_len = 0;
    if (this->waitForDataTillRespon(ret_data, ret_data_len) == false) {
        return -1;
    }
    if (ret_data[9] != 0x00) {
        this->getDebugSerial()->println(" LED control failed.");
        return -1;
    }
    return 0;
}

/** Send serial cmd to configure module, and then waiting for response.
    setting : The code of configuration item.
    value : The value of configuration item.
 * */
template <class T, class T1>
int8_t FingerPrint_KCT202<T, T1>::configModule(uint8_t setting, uint8_t value)
{
    Protocol pro;
    Protocol_oprt pro_oprt;
    uint8_t pro_data[16] = {0};
    uint32_t pro_len = 0;
    /*directive_code + configuration item + configuration value */
    uint8_t data[3] = {CONFIG_MODULE, setting, value};
    pro.begin(CMD_PACK_ID, sizeof(data) + 2, data, sizeof(data), finger_chip_addr_);
    if (pro_oprt.generatePack(pro, pro_data, pro_len) < 0) {
        return -1;
    }
    this->binWrite(pro_data, pro_len);

    uint8_t ret_data[13] = {0};
    uint32_t ret_data_len = 0;
    if (this->waitForDataTillRespon(ret_data, ret_data_len) == false) {
        return -1;
    }
    if (ret_data[9] != 0x00) {
        this->getDebugSerial()->println(" Module configure failed.");
        return -1;
    }
    return 0;
}


#if defined(ARDUINO_ARCH_AVR)
    template class FingerPrint_KCT202<SoftwareSerial, HardwareSerial>;
#elif defined(ARDUINO_ARCH_SAM)
    //template class FingerPrint_KCT202<HardwareSerial,HardwareSerial>;
    //template class FingerPrint_KCT202<HardwareSerial,Serial_>;
    template class FingerPrint_KCT202<Uart, Serial_>;
#elif defined(ARDUINO_ARCH_SAMD)
    //template class FingerPrint_KCT202<HardwareSerial,HardwareSerial>;
    //template class FingerPrint_KCT202<HardwareSerial,Serial_>;
    template class FingerPrint_KCT202<Uart, Serial_>;
#elif defined(ARDUINO_ARCH_ESP32)
    #if defined(CONFIG_IDF_TARGET_ESP32C3) ||  defined(CONFIG_IDF_TARGET_ESP32C6) ||  defined(CONFIG_IDF_TARGET_ESP32S3)
    template class FingerPrint_KCT202<HardwareSerial,HWCDC>;
    #else
    template class FingerPrint_KCT202<HardwareSerial,HardwareSerial>;
    //template class FingerPrint_KCT202<Uart, Serial_>;
    #endif
#elif defined(NRF52840_XXAA)
template class FingerPrint_KCT202<SoftwareSerial,Adafruit_USBD_CDC>;
#else
    template class FingerPrint_KCT202<SoftwareSerial, HardwareSerial>;
#endif

