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
#ifndef __PROTOCOL__H
#define __PROTOCOL__H

#include <Arduino.h>

#define HEADER  0xEF01



/** Protocol format:
 * *********************************************************************************************
    CMD pack:
     header     chip_address    pack_id     length     directive   paras    checkSum
       2B           4B            1B         2B          1B        ...        2B
     0xFE01  FFFFFFFF(default)   0x01         N          x         ...        xx

    DATA pack:(with subsequent pack)
     header     chip_address    pack_id     length     data    checkSum
       2B           4B            1B         2B        ...       2B
     0xFE01  FFFFFFFF(default)   0x02         N         x        xx

    DATA end pack:
     header     chip_address    pack_id     length     data    checkSum
       2B           4B            1B         2B        ...       2B
     0xFE01  FFFFFFFF(default)   0x08         N         x        xx

    Return pack:
     header     chip_address    pack_id     length     confirm_code    checkSum
       2B           4B            1B         2B            ...            2B
     0xFE01  FFFFFFFF(default)   0x07         N             x             xx

 * *********************************************************************************************
 * */

class Protocol {
  public:
    Protocol() {
        set_data_flag_ = 0;
    }
    void begin(uint8_t pack_id, uint16_t length, uint8_t* data, uint32_t data_len, uint32_t chip_addr = 0xFFFFFFFF) {

        header_ = HEADER;
        pack_id_ = pack_id;
        length_ = length;
        pack_id_ = pack_id;
        if ((data_len > 0)  && (NULL != data)) {
            set_data_flag_ = 1;
            data_ = new uint8_t[data_len];
            memcpy(data_, data, data_len);
        }

        data_len_ = data_len;
        chip_addr_ = chip_addr;
    };

    void setData(uint8_t* data, uint32_t data_len) {
        if ((data_len > 0)  && (NULL != data) && (1 != set_data_flag_)) {
            set_data_flag_ = 1;
            data_ = new uint8_t[data_len];
            memcpy(data_, data, sizeof(data_len));
        }
    }

    ~Protocol() {
        if (1 == set_data_flag_) {
            delete[] data_;
        }
    }

    /** Fixed oxFE01.

      * */
    uint16_t header_;

    /** Default 0xFFFFFFFF

     * */
    uint32_t chip_addr_;
    /** The data is 0x02 or 0x08(depends on whether has subsequent pack)

     * */
    uint8_t pack_id_;
    /** The cnt of length is from length to checkSum, except length.

     * */
    uint16_t length_;

    /** Depends on what the command is.

     * */
    uint8_t directive_;

    /** Depends on what the command sets before, then set the corresponding data, maybe doesn't exist.

     * */
    uint8_t* data_;
    uint32_t data_len_;
    /** checkSum,sum from pack_id to checkSum, without pack_id&checkSum.

     * */
    uint16_t check_sum_;

    uint8_t set_data_flag_;

};

#if 0
class Protocol_Cmd {
  public:
    Protocol_Cmd(uint8_t* chip_addr, uint8_t* length, uint8_t directive, uint8_t* params, uint32_t params_len,
                 uint8_t* check_sum) {
        if (NULL == chip_addr)
            //uint8_t default_chip_addr[4] = {0xFF,0xFF,0xFF,0xFF};
            //memcpy(chip_addr_,default_chip_addr,sizeof(chip_addr_));
        {
            memset(chip_addr_, 0xFF, sizeof(chip_addr_));
        } else {
            memcpy(chip_addr_, chip_addr, sizeof(chip_addr_));
        }
        memcpy(length_, length, sizeof(length_));
        directive_ = directive;
        params_ = params;
        params_len_ = params_len;
        memcpy(check_sum_, check_sum, sizeof(check_sum_));
    };
    /** Fixed oxFE01.

     * */
    uint8_t header_[2];

    /** Default 0xFFFFFFFF

     * */
    uint8_t chip_addr_[4];
    /** The command pack's id is fixed 0x01, the data is 0x02 or 0x08(depends on whether has subsequent pack)

     * */
    uint8_t pack_id_;
    /** The cnt of length is from length to checkSum, except length.

     * */
    uint8_t length_[2];

    /** Depends on what the command is.

     * */
    uint8_t directive_;

    /** Depends on what the command is, then set the corresponding param, maybe doesn't exist.

     * */
    uint8_t* params_;
    uint32_t params_len_;

    /** checkSum,sum from pack_id to checkSum, without pack_id&checkSum.

     * */
    uint8_t check_sum_[2];
};
#endif


class Protocol_oprt {
  public:
    int8_t generatePack(Protocol& protocol, uint8_t* result_pro, uint32_t& len);
    int8_t checkRecvPack(uint8_t* data, uint32_t data_len, uint8_t& error_code, uint8_t* ret_param,
                         uint32_t& ret_param_len);
    uint16_t mergeU8ToU16(uint8_t high, uint8_t low);

  private:
    int8_t getCheckSum(uint8_t* data, uint32_t data_len, uint8_t* check_sum);

    int8_t checkHeader(uint8_t* data, uint32_t data_len);
    int8_t checkCheckSum(uint8_t* data, uint32_t data_len);
    int8_t getRetParam(uint8_t* data, uint8_t* param, uint32_t& param_len);

    //int8_t getMessage(uint8_t *data,uint32_t data_len,uint8_t *msg_data,uint32_t msg_len);

};




#endif