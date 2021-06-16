// 開く
// nif: ネットワークインターフェースID
// return: 0=失敗 / 1=成功
int EtherCAT_open(char* nif);

// 閉じる
void EtherCAT_close(void);

#define ALL_SLAVES_OP_STATE 0   // 全てのスレーブがOP状態になりました (成功)
#define NO_SLAVES_FOUND     1   // スレーブがみつかりません
#define NOT_ALL_OP_STATE    2   // OP状態にならないスレーブがあります

// コンフィグする
// return 結果
int EtherCAT_config(void);

// スレーブの数を取得
// return: スレーブの数
int EtherCAT_getSlaveCount(void);

// スレーブの状態を更新
// return: 全スレーブの中で最も低い状態
int EtherCAT_updateState(void);

// スレーブの状態を取得
// slave: スレーブのインクリメンタルアドレス
// return: 状態
int EtherCAT_getState(int slave);

// スレーブのALステータスコードを取得
// slave: スレーブのインクリメンタルアドレス
// return: ALステータスコード
int EtherCAT_getALStatusCode(int slave);

// スレーブのALステータスの説明を取得
// slave: スレーブのインクリメンタルアドレス
// desc: ALステータスの説明 (最大31文字)
void EtherCAT_getALStatusDesc(int slave, char* desc);

// スレーブの状態変更を要求
void EtherCAT_requestState(int slave, int state);

// スレーブの名前を取得
// slave: スレーブのインクリメンタルアドレス
// name: スレーブの名前 (最大31文字)
void EtherCAT_getName(int slave, char* name);

// スレーブのベンダ番号/製品番号/バージョン番号を取得
// slave: スレーブのインクリメンタルアドレス
// id: {ベンダ番号, 製品番号, バージョン番号}
void EtherCAT_getId(int slave, unsigned long* id);

// PDO転送する
// return:  0=失敗 / 1=成功
int EtherCAT_transferPDO(void);

// PDO入力バッファから値を取得する
// slave: スレーブのインクリメンタルアドレス
// offset: オフセットアドレス
// return: 入力値
uint8_t  EtherCAT_getUint8 (int slave, int offset);
int8_t   EtherCAT_getInt8  (int slave, int offset);
uint16_t EtherCAT_getUint16(int slave, int offset);
int16_t  EtherCAT_getInt16 (int slave, int offset);
uint32_t EtherCAT_getUint32(int slave, int offset);
int32_t  EtherCAT_getInt32 (int slave, int offset);

// PDO出力バッファに値を設定する
// slave: スレーブのインクリメンタルアドレス
// offset: オフセットアドレス
// value: 出力値
void EtherCAT_setUint8 (int slave, int offset, uint8_t  value);
void EtherCAT_setInt8  (int slave, int offset, int8_t   value);
void EtherCAT_setUint16(int slave, int offset, uint16_t value);
void EtherCAT_setInt16 (int slave, int offset, int16_t  value);
void EtherCAT_setUint32(int slave, int offset, uint32_t value);
void EtherCAT_setInt32 (int slave, int offset, int32_t  value);

