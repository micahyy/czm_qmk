# Rush60

Rush60_7U — 60% 7U 空格客制化键盘。

* MCU: AT32F405RCT7-7 (Cortex-M4F @216MHz, 256KB Flash, 96KB SRAM)
* USB: OTG-HS，片上 HS PHY
* 矩阵: 5×14，COL2ROW
* 灯: WS2812 @ PC8
* Bootloader: CherryUF2（拖拽刷写）

## 烧录

默认使用 UF2 拖拽：按住 Esc 插入进入 bootloader（或短接 Boot），将 uf2 拖入磁盘。

```
qmk compile -kb czmao/rush60 -km default
```
