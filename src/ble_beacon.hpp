#ifndef BLE_BEACON_HPP
#define BLE_BEACON_HPP

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// ターゲットビーコン名 (実際のビーコン名に置き換えてください)
#define TARGET_BEACON_NAME "YourBeaconName"

class BeaconTracker {
public:
    BeaconTracker();
    void init();                      // BLE初期化
    void startScan();                 // スキャン開始 (短時間で結果を返す想定)
    void stopScan();                  // スキャン停止
    bool isBeaconFound();             // ターゲットビーコン発見フラグ
    int getBeaconRSSI();              // ターゲットビーコンのRSSI値取得
    // 必要であればRSSIから距離を推定する関数も追加
    // float getEstimatedDistance();

private:
    static BLEScan* pBLEScan;                     // BLEスキャンオブジェクト
    static bool beaconFound;                      // ビーコン発見フラグ
    static int currentRSSI;                       // 現在のRSSI値
    static std::string targetDeviceName;          // ターゲットデバイス名

    // スキャン結果を処理するコールバッククラス
    class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
        void onResult(BLEAdvertisedDevice advertisedDevice) override;
    };
};

#endif // BLE_BEACON_HPP