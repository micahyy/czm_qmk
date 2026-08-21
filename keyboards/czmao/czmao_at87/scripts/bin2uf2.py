#!/usr/bin/env python3
"""Convert a raw binary file to UF2 format for AT32F405 CherryUF2 bootloader."""
import struct
import sys
import argparse

UF2_MAGIC_START0 = 0x0A324655  # "UF2\n"
UF2_MAGIC_START1 = 0x9E5D5157
UF2_MAGIC_END    = 0x0AB16F30

UF2_FLAG_FAMILY_ID_PRESENT = 0x00002000

def bin2uf2(bin_path, uf2_path, base_addr, family_id, chunk_size=256):
    with open(bin_path, 'rb') as f:
        data = f.read()

    # Pad to chunk_size boundary
    padded_len = (len(data) + chunk_size - 1) & ~(chunk_size - 1)
    data = data + b'\xff' * (padded_len - len(data))

    num_blocks = (len(data) + chunk_size - 1) // chunk_size

    with open(uf2_path, 'wb') as f:
        for block_no in range(num_blocks):
            offset = block_no * chunk_size
            chunk = data[offset:offset + chunk_size]
            if len(chunk) < chunk_size:
                chunk = chunk + b'\x00' * (chunk_size - len(chunk))

            addr = base_addr + offset

            block = struct.pack('<IIIIIIII',
                UF2_MAGIC_START0,
                UF2_MAGIC_START1,
                UF2_FLAG_FAMILY_ID_PRESENT,
                addr,
                chunk_size,
                block_no,
                num_blocks,
                family_id
            )
            block += chunk
            block += b'\x00' * (476 - chunk_size)  # pad to 476 bytes
            block += struct.pack('<I', UF2_MAGIC_END)

            assert len(block) == 512
            f.write(block)

    print(f"Converted {bin_path} -> {uf2_path}")
    print(f"  Base address: 0x{base_addr:08X}")
    print(f"  Family ID:    0x{family_id:08X}")
    print(f"  Binary size:  {padded_len} bytes")
    print(f"  UF2 blocks:   {num_blocks}")
    print(f"  UF2 size:     {num_blocks * 512} bytes")

def main():
    parser = argparse.ArgumentParser(description='BIN to UF2 converter for AT32F405')
    parser.add_argument('input', help='Input .bin file')
    parser.add_argument('output', help='Output .uf2 file')
    parser.add_argument('-a', '--base-address', type=lambda x: int(x, 0),
                        default=0x08004000, help='Flash base address (default: 0x08004000)')
    parser.add_argument('-f', '--family-id', type=lambda x: int(x, 0),
                        default=0xf35c900d, help='UF2 family ID (default: 0xf35c900d)')
    args = parser.parse_args()
    bin2uf2(args.input, args.output, args.base_address, args.family_id)

if __name__ == '__main__':
    main()
