/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a680c(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  
  uVar2 = FUN_108544a0(param_2,param_3);
  iVar3 = FUN_1084c7d0(param_2,5);
  uVar5 = 0;
  iVar6 = 0;
  do {
    iVar4 = FUN_108722e8(uVar2,(&DAT_108d2c3a)[uVar5],1);
    if (iVar4 != 0) {
      *(undefined1 *)(iVar3 + iVar6) = (&DAT_108d2c3a)[uVar5];
      iVar6 = iVar6 + 1;
    }
    uVar5 = uVar5 + 1;
  } while (uVar5 < 5);
  if (iVar6 != 0) {
    iVar6 = FUN_1084c7bc(iVar6);
    uVar1 = *(undefined1 *)(iVar3 + iVar6);
    iVar3 = FUN_1086b91c(2);
    if (iVar3 == param_3) {
      iVar3 = FUN_10869f7c(param_2,0xd,param_3);
      *(undefined1 *)(iVar3 + 0x17) = 2;
      *(undefined1 *)(iVar3 + 4) = uVar1;
      *(undefined1 *)(iVar3 + 0x16) = 1;
      *(char *)(iVar3 + 0xc) = (char)param_3;
      FUN_10867b48(param_2);
      return;
    }
  }
  return;
}

