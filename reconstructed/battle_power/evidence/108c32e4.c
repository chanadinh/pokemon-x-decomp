/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c32e4(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  undefined8 uVar3;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    uVar3 = FUN_1086b91c(0x12);
    iVar1 = (int)((ulonglong)uVar3 >> 0x20);
    uVar2 = (uint)uVar3 & 0xffff;
    if (uVar2 != 0x160) {
      iVar1 = uVar2 - 0x100;
    }
    if ((uVar2 == 0x160 || iVar1 == 0x8f) || (uVar2 == 0x196 || uVar2 == 0x18c)) {
      FUN_1084ce84(0x32,0x1800);
      return;
    }
  }
  return;
}

