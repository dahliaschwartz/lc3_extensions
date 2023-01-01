// when moving a value from registerA into registerA, simply update condition code
if (r1 == r2){
    /* Update condition code by adding 0 to the updated register */
    write_value (0x1020 | (r1 << 9) | (r1 << 6) | (0x00 & 0x1F));
}
else{
    /* AND r1, #0
    ADD r1, r1, r2 */
    write_value (0x5020 | (r1 << 9) | (val & 0x00));
    write_value (0x1000 | (r1 << 9) | (r1 << 6) | r2);
}