/* find two registers that are not called by the user in the MLT operation */
;
int tempA, tempB = 0;

int i = 0;
while ((i == r1) || (i == r2) || (i == r3)) i++;
tempA = i;

i = 0;
while ((i == r1) || (i == r2) || (i == r3) || (i == tempA)) i++;
tempB = i;

/* store what's located in tempA and tempB into memory:
    ST tempA, #2
    ST tempB, #2
    BRnzp #2 (to skip the locations in memory that contain tempA and tempB) */
write_value (0x3000 | (tempA << 9) | (0x02 & 0x1FF));
write_value (0x3000 | (tempB << 9) | (0x02 & 0x1FF));
inst.ccode = CC_N | CC_Z | CC_P;
write_value (inst.ccode | (0x02 & 0x1FF));

// PLACEHOLDERS
write_value (0x0000);
write_value (0x0000);

/* move the contents of r2 into tempA:
    AND tempA, tempA, #0
    ADD tempA, tempA, r2 */
write_value (0x5020 | (tempA << 9) | (tempA << 6) | (0x00 & 0x1F));
write_value (0x1000 | (tempA << 9) | (tempA << 6) | r2);
/* move the contents of r3 into tempB:
    AND tempB, tempB, #0
    ADD tempB, tempB, r3 */
write_value (0x5020 | (tempB << 9) | (tempB << 6) | (0x00 & 0x1F));
if (operands == O_RRI) {
    (void)read_val (o3, &val, 5);
    write_value (0x1020 | (tempB << 9) | (tempB << 6) | (val & 0x1F));
}
else write_value (0x1000 | (tempB << 9) | (tempB << 6) | r3);

// Reset R1 to 0
write_value (0x5020 | (r1 << 9) | (r1 << 6) | (0x00 & 0x1F));

/* check if tempB is negative by adding 0 to tempB and checking condition code */
write_value (0x1020 | (tempB << 9) | (tempB << 6) | (0x00 & 0x1F));
// if tempB is negative, branch to code for negative multiplication
inst.ccode = CC_N;
write_value (inst.ccode | (0x04 & 0x1FF));

// general case (positive R3):
/*  R1 = R1 + tempA
    tempB = tempB - 1
    BR not zero 2 spots earlier */

// ADD R1, R1, tempA
write_value (0x1000 | (r1 << 9) | (r1 << 6) | tempA);
// ADD tempB, tempB, #-1
write_value (0x1020 | (tempB << 9) | (tempB << 6) | (0xFF & 0x1F));
// BRnp 2 spots earlier
inst.ccode = CC_P | CC_N;
write_value (inst.ccode | (-0x03 & 0x1FF));
// branch to end of code (to skip code for negative r3)
inst.ccode = CC_P | CC_N | CC_Z;
write_value (inst.ccode | (0x05 & 0x1FF));

// negative R3:
/*  R1 = R1 + tempA
    tempB = tempB + 1
    BR not zero 2 spots earlier
    Negate answer in R1 */

// ADD R1, R1, tempA
write_value (0x1000 | (r1 << 9) | (r1 << 6) | tempA);
// ADD tempB, tempB, #1
write_value (0x1020 | (tempB << 9) | (tempB << 6) | (0x01 & 0x1F));
// BRnp 2 spots earlier
inst.ccode = CC_P | CC_N;
write_value (inst.ccode | (-0x03 & 0x1FF));
/* negate R1, because answer should be negative:
    NOT r1, r1
    ADD r1, r1, #1 */
write_value (0x903F | (r1 << 9) | (r1 << 6));
write_value (0x1020 | (r1 << 9) | (r1 << 6) | (0x01 & 0x1F));

// restore what was originally in tempA back into tempA (LD tempA, #-19)
write_value (0x2000 | (tempA << 9) | (-0x13 & 0x1FF));
// restore what was originally in tempB back into tempB (LD tempB, #-19)
write_value (0x2000 | (tempB << 9) | (-0x13 & 0x1FF));

/* Update condition code by adding 0 to the updated register */
write_value (0x1020 | (r1 << 9) | (r1 << 6) | (0x00 & 0x1F));