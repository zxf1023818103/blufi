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

# 后续计划

- [ ] 支持加密协商
- [ ] 实现服务端，为 Linux 设备和 macOS 设备提供配网服务
- [ ] 汉化