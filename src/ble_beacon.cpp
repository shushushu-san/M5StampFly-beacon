#include "ble_beacon.hpp"
#include "Arduino.h" // Serial出力用

// 静的メンバー変数の初期化
BLEScan* BeaconTracker::pBLEScan = nullptr;
bool BeaconTracker::beaconFound = false;
int BeaconTracker::currentRSSI = -100; // 未発見時は弱いRSSI値で初期化
std::string BeaconTracker::targetDeviceName = TARGET_BEACON_NAME;

BeaconTracker::BeaconTracker() {}

// BLEアドバタイズデバイスのコールバック処理
void BeaconTracker::MyAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveName() && advertisedDevice.getName() == BeaconTracker::targetDeviceName) {
        // ターゲットビーコンを発見
        BeaconTracker::currentRSSI = advertisedDevice.getRSSI();
        BeaconTracker::beaconFound = true;
        // スキャンを停止する場合はここで pBLEScan->stop(); を呼ぶ
    }
}

void BeaconTracker::init() {
    Serial.println("Initializing BLE...");
    BLEDevice::init(""); // BLEデバイス初期化 (デバイス名は空でOK)
    pBLEScan = BLEDevice::getScan(); // スキャンオブジェクト取得
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); // コールバック設定
    pBLEScan->setActiveScan(true); // アクティブスキャン (消費電力は増えるが結果取得が速い)
    pBLEScan->setInterval(100);    // スキャン間隔 (ms)
    pBLEScan->setWindow(99);       // スキャンウィンドウ (ms) <= Interval
    Serial.println("BLE Initialized.");
}

void BeaconTracker::startScan() {
    if (pBLEScan == nullptr) {
        Serial.println("BLE Scan not initialized!");
        return;
    }
    beaconFound = false; // スキャン開始前にフラグをリセット
    currentRSSI = -100;  // RSSIをリセット

    // 短時間のブロッキングスキャンを実行 (期間は調整が必要)
    // 注意: このスキャン時間は flight_control のループ周期に影響します。
    // 本格的な実装では非同期スキャンや別タスクでの処理が望ましいです。
    BLEScanResults foundDevices = pBLEScan->start(1, false); // 100ms程度のスキャン (第1引数は調整)
    // スキャン結果は MyAdvertisedDeviceCallbacks::onResult で処理される
}

void BeaconTracker::stopScan() {
    if (pBLEScan != nullptr) {
        pBLEScan->stop();
    }
    Serial.println("BLE scan stopped.");
}

bool BeaconTracker::isBeaconFound() {
    return beaconFound;
}

int BeaconTracker::getBeaconRSSI() {
    return currentRSSI;
}

/*
// RSSIから距離を推定する関数の例 (要キャリブレーション)
float BeaconTracker::getEstimatedDistance() {
    if (!beaconFound) return -1.0f;
    // RSSIから距離への変換式: d = 10^((TxPower - RSSI) / (10 * N))
    // TxPower: 1m地点での受信強度 (例: -59 dBm、要キャリブレーション)
    // N: 環境要因 (通常2～4)
    float txPower = -59; // キャリブレーションしてください
    float n = 2.5;       // 環境に合わせて調整してください
    if (currentRSSI == 0) {
        return -1.0; // 不正なRSSI値
    }
    float ratio = (float)currentRSSI / txPower; // TxPowerが負の値であることに注意
    // 単純な式: float distance = pow(10.0, (txPower - currentRSSI) / (10.0 * n));
    // return distance;

    // 簡易的な代替式 (あくまで目安)
    if (ratio < 1.0) {
         return pow(ratio, 10); // この式は精度が低いです
    } else {
         double distance = (0.89976) * pow(ratio, 7.7095) + 0.111; // さらに調整が必要
         return distance;
    }
}
*/