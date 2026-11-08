/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bea04(void)

{
  char cVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 *in_r3;
  
  uVar2 = FUN_1086b91c(0x12);
  uVar2 = uVar2 & 0xffff;
  *in_r3 = 0;
  if ((uVar2 == 0x99 || uVar2 == 0x78) && (cVar1 = FUN_1086b91c(0x23), cVar1 == '\0')) {
    uVar3 = FUN_108658d4(0x23,0x13);
    *in_r3 = uVar3;
    in_r3[1] = uVar2;
  }
  return;
}

