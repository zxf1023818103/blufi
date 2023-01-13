# 概览

通过 Blufi 协议配置 ESP32/ESP8266 设备的网络。

# 使用

```plaintext
Usage: C:\Users\zxf10\Desktop\blufi\out\build\x64-Debug\blufi.exe [options]
Configure Wi-Fi network for ESP32/ESP8266 device via Bluetooth channel

Options:
  -n, --name <nameFilter>        Blufi device name regular expression filter
                                 (Case sensitive)
  -a, --address <addressFilter>  Bluetooth LE public address regular expression
                                 filter (Case insensitive)
  -s, --ssid <ssid>              SSID of Wi-Fi hotspot
  -p, --psk <password>           Pre-shared key of Wi-Fi hotspot
  -?, -h, --help                 Displays help on commandline options.
  --help-all                     Displays help including Qt specific options.
```

# 示例

```shell
$ ./blufi -n BLUFI_DEVICE -s Zeng_2.4G -p 123456789
```

```plaintext
"BlufiClientApplication::start"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onControllerStateChanged" QLowEnergyController::ConnectingState
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onControllerStateChanged" QLowEnergyController::ConnectedState
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onConnected"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onControllerStateChanged" QLowEnergyController::DiscoveringState
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onServiceDiscovered" "{00001800-0000-1000-8000-00805f9b34fb}"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onServiceDiscovered" "{00001801-0000-1000-8000-00805f9b34fb}"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onServiceDiscovered" "{0000ffff-0000-1000-8000-00805f9b34fb}"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onControllerStateChanged" QLowEnergyController::DiscoveredState
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onServiceDiscoveryFinished"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onServiceStateChanged" QLowEnergyService::RemoteServiceDiscovering
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onServiceStateChanged" QLowEnergyService::RemoteServiceDiscovered
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onDescriptorWritten" "{00002902-0000-1000-8000-00805f9b34fb}" "01 00"
"BlufiScanner::onBlufiClientReady"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClientFrameCoderAdapter::onDataSent" "09 00 00 09 5a 65 6e 67 5f 32 2e 34 47 5e 37"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClientFrameCoderAdapter::onDataSent" "0d 60 01 09 31 32 33 34 35 36 37 38 39 03 00"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClientFrameCoderAdapter::onDataSent" "0c e0 02 00 fe 7f"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onCharacteristicWritten" "{0000ff01-0000-1000-8000-00805f9b34fb}" "09 00 00 09 5a 65 6e 67 5f 32 2e 34 47 5e 37"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onCharacteristicWritten" "{0000ff01-0000-1000-8000-00805f9b34fb}" "0d 60 01 09 31 32 33 34 35 36 37 38 39 03 00"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onCharacteristicWritten" "{0000ff01-0000-1000-8000-00805f9b34fb}" "0c e0 02 00 fe 7f"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onCharacteristicChanged" "{0000ff02-0000-1000-8000-00805f9b34fb}" "3d 04 00 16 01 00 00 01 06 dc 2c 6e 61 f4 c1 02 09 5a 65 6e 67 5f 32 2e 34 47"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClientFrameCoderAdapter::onDataReceived" "3d 04 00 16 01 00 00 01 06 dc 2c 6e 61 f4 c1 02 09 5a 65 6e 67 5f 32 2e 34 47"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClientApplication::onConnectionStatusReceived" BlufiFrameCoder::STATUS_CONNECTED
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onServiceStateChanged" QLowEnergyService::InvalidService
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onControllerStateChanged" QLowEnergyController::ClosingState
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onControllerStateChanged" QLowEnergyController::UnconnectedState
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClient::onDisconnected"
"BLUFI_DEVICE" "84:F7:03:A5:24:E5" "BlufiClientApplication::onDataFrameReceived" BlufiFrameCoder::DATA_STATUS "01 00 00 01 06 dc 2c 6e 61 f4 c1 02 09 5a 65 6e 67 5f 32 2e 34 47"
"BlufiScanner::onBlufiClientDestroyed" 0
"BlufiScanner::onScanFinished"
"BlufiClientApplication::onBlufiClientAllDestroyed"
```

# 后续计划

- [ ] 支持加密协商
- [ ] 实现服务端，为 Linux 设备和 macOS 设备提供配网服务
- [ ] 汉化