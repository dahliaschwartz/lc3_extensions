// De Morgan's: A | B = NOT (NOT(A) & NOT(B))

/* find a register that is not called by the user in the OR operation */
;
tempA = 0;
while ((tempA == r1) || (tempA == r2) || (tempA == r3)) tempA++;

/* store what's located in tempA into memory:
    ST tempA, #1
    BRnzp #1 (to skip the location in memory that contains tempA) */
write_value (0x3000 | (tempA << 9) | (0x01 & 0x1FF));
inst.ccode = CC_N | CC_Z | CC_P;
write_value (inst.ccode | (0x01 & 0x1FF));

// PLACEHOLDER
write_value (0x0000);

/* move the contents of r3 into tempA:
    AND tempA, tempA, #0
    ADD tempA, tempA, r3 */
write_value (0x5020 | (tempA << 9) | (tempA << 6) | (0x00 & 0x1F));
if (operands == O_RRI) {
    (void)read_val (o3, &val, 5);
    write_value (0x1020 | (tempA << 9) | (tempA << 6) | (val & 0x1F));
}
else write_value (0x1000 | (tempA << 9) | (tempA << 6) | r3);

/* r1 = NOT(R2) & NOT(R3) */
// NOT R1, R2
write_value (0x903F | (r1 << 9) | (r2 << 6));
// NOT tempA, tempA
write_value (0x903F | (tempA << 9) | (tempA << 6));
// AND R1, R1, tempA
write_value (0x5000 | (r1 << 9) | (r1 << 6) | tempA);

/* R1 = NOT (NOT(R2) & NOT(R3))
    NOT R1, R1 */
write_value (0x903F | (r1 << 9) | (r1 << 6));

// restore what was originally in tempA back into tempA (LD tempA, #-7)
write_value (0x2000 | (tempA << 9) | (-0x08 & 0x1FF));

/* Update condition code by adding 0 to the updated register */
write_value (0x1020 | (r1 << 9) | (r1 << 6) | (0x00 & 0x1F));