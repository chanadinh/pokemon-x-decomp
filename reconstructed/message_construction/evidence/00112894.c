/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_00112894(undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  FUN_0011ee68(param_1,*(undefined4 *)(DAT_005d1870 + 0x30));
  iVar1 = FUN_00122920(0x28,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0011e264(iVar1,*(undefined4 *)(DAT_005d1870 + 0x44),
                         *(undefined4 *)(DAT_005d1870 + 0x54),param_1,1,5);
  }
  DAT_005d1888 = uVar2;
  iVar1 = FUN_00122920(0xc,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0012b968(iVar1,param_1);
  }
  DAT_005d188c = uVar2;
  iVar1 = FUN_00122920(0xc,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0011edec(iVar1,param_1,*(uint *)(DAT_005d1870 + 0x34) & 0xff);
  }
  DAT_005d1890 = uVar2;
  return;
}

