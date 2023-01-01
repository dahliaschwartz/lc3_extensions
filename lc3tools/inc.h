// ADD r1, r1, #1
write_value (0x1020 | (r1 << 9) | (r1 << 6) | (0x01 & 0x1F));