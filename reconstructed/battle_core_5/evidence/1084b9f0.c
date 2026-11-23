/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1084b9f0(int param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = *(int *)(param_1 + 8);
  if (0x17 < param_2) {
    FUN_10800394(0,0);
  }
  iVar4 = iVar4 + param_2 * 4;
  if (*(int *)(iVar4 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar3 = *(int *)(iVar4 + 0x84);
  FUN_1086bb84();
  FUN_1086bac0(2,*(undefined1 *)(iVar3 + 0x19));
  FUN_10867c9c(0x36,0x1000,0x19a,0x20000);
  FUN_1086b9a4(param_1,0x84);
  iVar4 = FUN_1086b91c(0x36);
  FUN_1086b880();
  uVar1 = (uint)*(ushort *)(iVar3 + 0x150) * iVar4;
  uVar2 = uVar1 >> 0xc;
  if (0x800 < (uVar1 & 0xfff)) {
    uVar2 = uVar2 + 1;
  }
  if (uVar2 == 0) {
    uVar2 = 1;
  }
  return uVar2;
}

