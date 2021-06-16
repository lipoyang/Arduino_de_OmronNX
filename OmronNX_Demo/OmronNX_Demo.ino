#include <stdio.h>
#include <string.h>
#include <stdint.h>

#if defined(GRROSE)
#include "FreeRTOS.h"
#include "task.h"
#endif

#include <SOEM.h>
#include "EtherCAT.h"

#define LCHIKA_PATTERN_MAX  4   // Lチカパターンの種類
#define LCHIKA_TIME         100 // Lチカの時間間隔[msec]
#define SW_MAX              4   // スイッチの数
#define CHATTERING_TIME     30  // チャタリング防止

// スイッチのビットマスク(スイッチは1,5,9,13に接続されている)
const uint16_t SW_BIT_MASK[SW_MAX]={
    0x0002, 0x0020, 0x0200, 0x2000
};

// Lチカパターン

// パターン0
const uint16_t LCHIKA_PATTERN_0[] ={
    0x0003, 0x000C, 0x0030, 0x00C0, 0x0300 ,0x0C00, 0x3000,
    0xC000, 0x3000, 0x0C00, 0x0300, 0x00C0, 0x0030, 0x000C
};

// パターン1
const uint16_t LCHIKA_PATTERN_1[] ={
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010 ,0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
};

// パターン2
const uint16_t LCHIKA_PATTERN_2[] ={
    0x0003, 0x0000, 0x000C, 0x0000, 0x0030 ,0x0000, 0x00C0, 0x0000,
    0x0300, 0x0000, 0x0C00, 0x0000, 0x3000 ,0x0000, 0xC000, 0x0000
};
 
// パターン3
const uint16_t LCHIKA_PATTERN_3[] ={
    0x000F
};

// パターンの長さ
const int LCHIKA_PATTERN_LEN[LCHIKA_PATTERN_MAX] = {
    14,
    16,
    16,
    1
};

// パターンのテーブル
const uint16_t* LCHIKA_PATTERN [LCHIKA_PATTERN_MAX] = {
    LCHIKA_PATTERN_0,
    LCHIKA_PATTERN_1,
    LCHIKA_PATTERN_2,
    LCHIKA_PATTERN_3
};

// Lチカパターン3のときスイッチ1～3に対応するパターン
const uint16_t LCHIKA_PATTERN_SUB[3]={ 0x00F0, 0x0F00, 0xF000 };

// スイッチ0の状態カウンタ(チャタリング防止用)
int sw0_cnt = 0;
// スイッチ0の状態
int sw0_state = 0;
// Lチカパターン番号
int Lchica_num = 0;
// Lチカカウンタ
int Lchika_cnt = 0;

// スイッチ0が押下されたか？
// inputVal: 16接点の入力値
bool sw0_pushed(uint16_t inputVal)
{
    uint16_t val = inputVal & SW_BIT_MASK[0];
    if(val != 0){
        if(sw0_state == 0){
            sw0_cnt++;
            if(sw0_cnt >= CHATTERING_TIME){
                sw0_state = 1;
                return true;
            }
        }
    }else{
        sw0_cnt = 0;
        sw0_state = 0;
    }
    return false;
}

// Lチカ制御
void Lchika_ctrl(void)
{
    char ifname[] = "EthernetShield2"; // It's dummy
    
    int result = EtherCAT_open(ifname);
    if(result == 0)
    {
        printf("can not open network adapter!\n");
        return ;
    }
    result = EtherCAT_config();
    if(result == NO_SLAVES_FOUND)
    {
        printf("slaves not found!\n");
        return ;
    }
    if(result == NOT_ALL_OP_STATE)
    {
        printf("at least one slave can not reach OP state!\n");
        return ;
    }
    printf("All slaves OP state!\n");
    
    uint32_t last_time = millis();
    
    while (1)
    {
        // EtherCATの入力バッファからスイッチ入力を取得
        uint16_t inputval  = EtherCAT_getUint16(1, 34);
        //Serial.println(inputval, HEX);
        
        // 一定の時間間隔ごとにパターンにしたがってLチカ
        uint16_t outputval = LCHIKA_PATTERN[Lchica_num][Lchika_cnt];
        
        uint32_t now = millis();
        if(now - last_time >= LCHIKA_TIME){
            last_time += LCHIKA_TIME;
            
            Lchika_cnt++;
            if(Lchika_cnt >= LCHIKA_PATTERN_LEN[Lchica_num]) Lchika_cnt = 0;
        }
        
        // スイッチ0が押されたらLチカのパターンを変更
        if(sw0_pushed(inputval)){
            Lchica_num++;
            if(Lchica_num >= LCHIKA_PATTERN_MAX) Lchica_num = 0;
            Lchika_cnt = 0;
        }
        
        // Lチカパターン3のときスイッチ1～3が押されている間は対応するLEDを点灯
        if(Lchica_num == 3){
            if(inputval & SW_BIT_MASK[1]) outputval |= LCHIKA_PATTERN_SUB[0];
            if(inputval & SW_BIT_MASK[2]) outputval |= LCHIKA_PATTERN_SUB[1];
            if(inputval & SW_BIT_MASK[3]) outputval |= LCHIKA_PATTERN_SUB[2];
        }
        
        // EtherCATの出力バッファにLED出力を設定
        EtherCAT_setUint16(1, 0, outputval);
        
        // EtherCATのPDO転送
        EtherCAT_transferPDO();
    }

    // 切断処理
    EtherCAT_requestState(0, EC_STATE_INIT);
    EtherCAT_close();
}

//********** for GR-ROSE **********
#if defined(GRROSE)

void app_main(void* arg)
{
    while(1){
        Lchika_ctrl();
    }
}
void setup()
{
    Serial.begin(115200);
    delay(3000);
    int ret = xTaskCreate(app_main, "APP_MAIN_TASK", 10*1024, NULL, 3, NULL);
}
void loop()
{
    ; // do nothing here
}

#else
//********** except for GR-ROSE **********

void setup()
{
    Serial.begin(115200);
    delay(1000);
}

void loop()
{
    Lchika_ctrl();
}

#endif
