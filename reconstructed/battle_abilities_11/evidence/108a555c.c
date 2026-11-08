/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a555c(int param_1,undefined4 param_2,undefined4 param_3,undefined4 *param_4)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  
  bVar1 = FUN_1086b91c(2);
  uVar2 = FUN_1084d348(param_2,param_3);
  if (uVar2 == bVar1) {
    iVar3 = FUN_10869f7c(param_2,0x2b,0x1f);
    *(char *)(iVar3 + 4) = (char)*param_4;
    *(byte *)(iVar3 + 5) = bVar1;
    FUN_10867b48(param_2);
    FUN_1086c218(param_2,*(ushort *)(param_1 + 0x38) & 0xff,param_3);
    FUN_1084d1cc(param_1);
    return;
  }
  return;
}

