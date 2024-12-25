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
#include "Protocol.h"



int8_t Protocol_oprt::getCheckSum(uint8_t* data, uint32_t data_len, uint8_t* check_sum) {
    uint16_t sum = 0;
    if (NULL == data || NULL == check_sum) {
        return -1;
    }
    for (int i = 0; i < data_len; i++) {
        sum += data[i];
    }
    check_sum[0] = sum >> 8;
    check_sum[1] = (uint8_t)sum;
    return 0;
}

int8_t Protocol_oprt::generatePack(Protocol& protocol, uint8_t* result_pro, uint32_t& len) {
    if (NULL == result_pro) {
        return -1;
    }
    uint8_t check_sum[2] = {0};
    /*header 2B*/
    result_pro[0] = protocol.header_ >> 8;
    result_pro[1] = protocol.header_;
    /*chip addr 4B*/
    result_pro[2] = protocol.chip_addr_ >> 24;
    result_pro[3] = protocol.chip_addr_ >> 16;
    result_pro[4] = protocol.chip_addr_ >> 8;
    result_pro[5] = protocol.chip_addr_;
    /*pack id 1B*/
    result_pro[6] = protocol.pack_id_;
    /*pack length 2B*/
    result_pro[7] = protocol.length_ >> 8;
    result_pro[8] = protocol.length_;
    /*directive code*/
    //result_pro[9] = protocol.directive_;
    /*data length NB*/
    for (int i = 0; i < protocol.data_len_; i++) {
        result_pro[9 + i] = protocol.data_[i];
    }
    /*check sum 2B*/
    if (getCheckSum(&result_pro[6], protocol.length_ + 1, check_sum) < 0) {
        return -2;
    }
    result_pro[9 + protocol.data_len_] = check_sum[0];
    result_pro[10 + protocol.data_len_] = check_sum[1];

    len = 9 + protocol.length_;
    return 0;
}


int8_t Protocol_oprt::checkRecvPack(uint8_t* data, uint32_t data_len, uint8_t& error_code, uint8_t* ret_param,
                                    uint32_t& ret_param_len) {
    int8_t ret = 0;
    error_code = 0xff;
    if (checkHeader(data, data_len) < 0) {
        return -1;
    }
    if (checkCheckSum(data, data_len) < 0) {
        return -1;
    }
    error_code = data[9];
    if (getRetParam(data, ret_param, ret_param_len)) {
        return -1;
    }
    return 0;
}

/** param feild: after err code,before check sum;

 * */
int8_t Protocol_oprt::getRetParam(uint8_t* data, uint8_t* param, uint32_t& param_len) {
    uint16_t pack_len = 0;
    if (NULL == data) {
        return -1;
    }
    pack_len = (uint16_t)data[7] << 8 | data[8];
    //length field minus length(2B),confirm code(1B).
    param_len = pack_len - 2 - 1;
    memcpy(param, &data[10], param_len);

    return 0;
}


int8_t Protocol_oprt::checkHeader(uint8_t* data, uint32_t data_len) {
    uint16_t header = 0;
    if (NULL == data) {
        return -1;
    }

    if (HEADER != mergeU8ToU16(data[0], data[1])) {
        return -1;
    }
    return 0;

}



int8_t Protocol_oprt::checkCheckSum(uint8_t* data, uint32_t data_len) {
    uint16_t pack_len = 0;
    uint8_t check_sum[2] = {0};
    if (NULL == data) {
        return -1;
    }
    //get length filed of pack。
    pack_len = (uint16_t)data[7] << 8 | data[8];
    //caculate checkSum
    if (getCheckSum(&data[6], pack_len + 1, check_sum) < 0) {
        return -1;
    }
    //
    if (mergeU8ToU16(check_sum[0], check_sum[1]) != mergeU8ToU16(data[7 + pack_len], data[7 + pack_len + 1])) {
        return -1;
    }
    return 0;
}


uint16_t Protocol_oprt::mergeU8ToU16(uint8_t high, uint8_t low) {
    return (uint16_t)high << 8 | low;
}



