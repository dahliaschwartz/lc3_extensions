// AND r1, r1, #0
write_value (0x5020 | (r1 << 9) | (r1 << 6) | (0x00 & 0x1F));