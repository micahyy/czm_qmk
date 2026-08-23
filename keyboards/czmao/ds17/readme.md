# CZMAO DS17

CZMAO DS17 17-key numpad. Two hardware revisions share the same USB VID/PID and VIA definition.

CZMAO DS17 17键数字小键盘。两个硬件版本共享相同的 USB VID/PID 和 VIA 定义。

- **r3**: Diode matrix (COL2ROW scan), soldered diodes / 二极管行列扫描
- **r6**: Direct pin matrix (no diodes) / 直连矩阵（无需二极管）

## Build / 编译

```bash
# r3 (diode matrix)
make czmao/ds17/r3:via COLOR=false

# r6 (direct pin)
make czmao/ds17/r6:via COLOR=false
```

## 3D Printed Case / 3D打印外壳

https://makerworld.com.cn/zh/@micahyy/upload

## VIA

After flashing VIA firmware, open [usevia.app](https://usevia.app) to configure keys.
刷入VIA固件后，打开 [usevia.app](https://usevia.app) 即可在线改键。
