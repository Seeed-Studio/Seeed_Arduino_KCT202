/*
 *  
 * Copyright (c) 2019 Seeed Technology Co., Ltd.
 * Website    : www.seeed.cc
 * Author     : downey
 * Create Time: Jun 2019
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef __KCT202__H
#define __KCT202__H

#include "ATSerial.h"
#include "Protocol.h"

#define FIX_HEADER                 0xEF01
#define CMD_PACK_ID                0x01
#define AUTO_REGISTER_FINGER_TEMP  0x31
#define AUTO_VERIFY_FINGER_TEMP    0x32
#define AUTO_DELETE_FINGER_TEMP    0x0c
#define CLEAN_ALL_FINGER_TEMP      0x0d
#define ENTER_SLEEP_MODE           0x33
#define READ_VALID_TEMP_CNT        0x1d
#define READ_INDEX_TABLE_TEMP      0x1f
#define SET_PASSWORD               0x12 
#define VERIFY_PASSWORD            0x13
#define CANCEL_ACTION              0x30


#define CHECK_ALL_FINGER_TEMP      0xffff

#define     LED_OFF_AFTER_GET_GRAGH             1 << 0
#define     PRETREATMENT_GRAGH                  1 << 1
#define     NOT_RET_FOR_EVERY_STEP              1 << 2
#define     OVERRIDE_CURR_FINGER_PRINT          1 << 3
#define     DETECT_FINGER_PRINT_EXIST           1 << 4




template <class T,class T1>
class FingerPrint_KCT202:public ATSerial<T,T1> ,public Protocol_oprt {

    public:
        FingerPrint_KCT202(){
            finger_chip_addr_ = 0xFFFFFFFF;
            //ATSerial<T,T1>::flush();
        }
        
            
        /** The auto-register fingerprint template protocol.
         * finger-ID ：The u16 type unique ID for fingerprint.
         * record_cnt : how many times you should put on your finger, to register one template.
         * param : control bytes(u16),get more detail by referring  to manual.
         **/
        int8_t autoRegisterFingerPrint(uint16_t finger_ID,uint8_t record_cnt,uint16_t param);
        /** The auto-verify fingerprint template protocol.
         * finger-ID ：The u16 type unique ID for fingerprint.
         * param : control bytes(u16),get more detail by referring  to manual.
         **/
        int8_t autoVerifyFingerPrint(uint16_t finger_ID,uint16_t param);
        
        /** The delete fingerprint template protocol.
         * finger-ID ：The u16 type unique ID for fingerprint,delete from finger-id to (finger-id+del_cnt).
         * del_cnt : how many fingerprint templates to be deleted. 
         **/
        int8_t DeleteFingerPrint(uint16_t finger_ID,uint8_t del_cnt);
        /** Clean the templates library,delete all registered fingerprint.
        **/
        int8_t cleanAllFingerPrint();

        int8_t enterSleepModeFingerPrint();
        int8_t readValidTempCountFingerPrint();
        /**Read index table of registered fingerprint template library.
         * The max fingerprint templates num is 1024.
         * but .... for this version,onlu support to read the page 0(0~255,total four pages,0~3).
         * 
         **/
        int8_t readIndexTableFingerPrint(uint8_t page);
        int8_t setPassWord(uint32_t pswd);
        int8_t verifyPassWord(uint32_t pswd);
        /** Cancel the last directive,in most case, we can send a new directive to override the last directive. 
         *  but for other directive,like autoregister, it should collect four fingerprint,if we have put on finger twice and then want to cancel it,we must use this directive.  
         * pswd ：u32 type password.
         **/
        int8_t cancelAction();
        /**We send cmd with serial, and then the sensor response.  
         * So we have to wait for response after send a cmd.This function uses a block way to wait for response. The function is waitForDataTillRespon(); 
         * Alternatively,you can use a unblock(query in a loop) way to do this,it up to you. 
         * This function to waiting for the register directive's response and parse the data , print the result.
         * */
        int8_t getRegisterResponAndparse();
        /**We send cmd with serial, and then the sensor response.  
         * So we have to wait for response after send a cmd.This function uses a block way to wait for response. The function is waitForDataTillRespon(); 
         * Alternatively,you can use a unblock(query in a loop) way to do this,it up to you. 
         * This function to waiting for the verify directive's response and parse the data , print the result.
         * finger_num : The finger id that matched.  
         * */
        int8_t getVerifyResponAndparse(uint16_t &finger_num);
        /**We send cmd with serial, and then the sensor response.  
         * So we have to wait for response after send a cmd.This function uses a block way to wait for response. The function is waitForDataTillRespon(); 
         * Alternatively,you can use a unblock(query in a loop) way to do this,it up to you. 
         * This function to waiting for the common(except autoregister& autoverify) directive's response and parse the data , print the result.
         * error_code : The status code that returned.  
         * param : The status param that returned,get more detail by referring manual for the speciefed directive.
         * param_len :
         * */
        int8_t getCommonResponAndparse(uint8_t &error_code,uint8_t *param,uint32_t &param_len);

        
        uint16_t getValidTempCntFromRsp(uint8_t *param);

        /**Invoke this function after readIndexTableFingerPrint() and getCommonResponAndparse(),parse the data which recieved from getCommonResponAndparse().
         * data : The data that sensor return back.
         * data_len : 
         * page : The page which read.
         * 
         **/
        int8_t getFingerTempID(uint8_t *data,uint32_t data_len,uint8_t page);

    private:
        /** The parameters of auto-register fingerprint template protocol.
         * Note: The param contain directive field.  
         * directive : fixed 0x31,indicates auto-register fingerprint template
         * finger-ID ：The u16 type unique ID for fingerprint.
         * record_cnt : how many times you should put on your finger, to register one template.
         * param : control bytes(u16),get more detail by referring  to manual.
         **/
        int8_t generateAutoRegisterParamData(uint8_t directive,uint16_t finger_ID,uint8_t record_cnt,uint16_t param,uint8_t *result_data);
        /** The parameters of auto-verify fingerprint template protocol.
         * Note: The param contain directive field.  
         * directive : fixed 0x32,indicates auto-verify fingerprint template
         * finger-ID ：The u16 type unique ID for fingerprint.
         * param : control bytes(u16),get more detail by referring  to manual.
         **/
        int8_t generateAutoVerifyParamData(uint8_t directive,uint16_t finger_ID,uint16_t param,uint8_t *result_data);
        /** The parameters of delete fingerprint template protocol.
         * Note: The param contain directive field.  
         * directive : fixed 0x0c,indicates delete fingerprint template num.
         * finger-ID ：The u16 type unique ID for fingerprint,delete from finger-id to (finger-id+del_cnt).
         * del_cnt : how many fingerprint templates to be deleted. 
         **/
        int8_t deleteParamData(uint8_t directive,uint16_t finger_ID,uint16_t del_cnt,uint8_t *result_data);
        /** The parameters of set password protocol.
         * Note: The param contain directive field.  
         * directive : fixed 0x12,indicates set pswd protocol.  
         * pswd ：u32 type password.
         **/
        int8_t generateSetPswdParamData(uint8_t directive,uint32_t pswd,uint8_t *result_data);
        uint32_t finger_chip_addr_;
};






#endif



