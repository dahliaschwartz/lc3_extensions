 /* NOT r1, r2
    ADD r1, r1, #1 */
write_value (0x903F | (r1 << 9) | (r2 << 6));
write_value (0x1020 | (r1 << 9) | (r1 << 6) | (0x01 & 0x1F));