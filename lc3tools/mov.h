// when moving a value from registerA into registerA, simply update condition code
if (r1 == r2){
    /* Update condition code by adding 0 to the updated register */
    write_value (0x1020 | (r1 << 9) | (r1 << 6) | (0x00 & 0x1F));
}
// otherwise, clear r1, add the contents of r2 into r1
else{
    write_value (0x5020 | (r1 << 9) | (val & 0x00));
    write_value (0x1000 | (r1 << 9) | (r1 << 6) | r2);
}