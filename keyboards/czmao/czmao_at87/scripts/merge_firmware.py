#!/usr/bin/env python3
"""
Merge CherryUF2 bootloader + QMK application binary into a single
production firmware image for AT-Link mass programming.

Memory layout:
  0x08000000 - 0x08003FFF  CherryUF2 bootloader (16KB)
  0x08004000 - 0x0803EFFF  Application (up to 236KB)
  0x0803F000 - 0x0803FFFF  EEPROM/wear-leveling (4KB, left as 0xFF)
"""
import sys
import argparse
import os

BOOTLOADER_ADDR = 0x08000000
APP_ADDR        = 0x08004000
FLASH_TOTAL     = 256 * 1024  # 256KB
BOOTLOADER_MAX  = APP_ADDR - BOOTLOADER_ADDR  # 16KB

def merge(bootloader_path, app_path, output_path, fill=0xFF):
    with open(bootloader_path, 'rb') as f:
        bl = f.read()
    with open(app_path, 'rb') as f:
        app = f.read()

    if len(bl) > BOOTLOADER_MAX:
        print(f"ERROR: Bootloader too large: {len(bl)} > {BOOTLOADER_MAX}", file=sys.stderr)
        sys.exit(1)

    app_max = FLASH_TOTAL - BOOTLOADER_MAX
    if len(app) > app_max:
        print(f"ERROR: Application too large: {len(app)} > {app_max}", file=sys.stderr)
        sys.exit(1)

    # Build full 256KB image
    image = bytearray([fill] * FLASH_TOTAL)

    # Place bootloader at 0x08000000 (offset 0)
    image[0:len(bl)] = bl

    # Place application at 0x08004000 (offset 0x4000)
    app_offset = APP_ADDR - BOOTLOADER_ADDR
    image[app_offset:app_offset + len(app)] = app

    with open(output_path, 'wb') as f:
        f.write(image)

    print(f"Merged firmware: {output_path}")
    print(f"  Bootloader: {bootloader_path} ({len(bl)} bytes at 0x{BOOTLOADER_ADDR:08X})")
    print(f"  Application: {app_path} ({len(app)} bytes at 0x{APP_ADDR:08X})")
    print(f"  Total image: {len(image)} bytes ({len(image)/1024:.0f}KB)")
    print(f"  App usage:  {len(app)}/{app_max} bytes ({100*len(app)/app_max:.1f}%)")
    print(f"  Free:       {app_max - len(app)} bytes")

def main():
    parser = argparse.ArgumentParser(description='Merge bootloader + app for AT-Link production')
    parser.add_argument('bootloader', help='Bootloader .bin file')
    parser.add_argument('app', help='Application .bin file')
    parser.add_argument('output', help='Merged output .bin file')
    args = parser.parse_args()
    merge(args.bootloader, args.app, args.output)

if __name__ == '__main__':
    main()
