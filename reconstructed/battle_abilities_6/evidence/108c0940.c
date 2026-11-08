/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c0940(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1085f844();
    if ((iVar1 == 0) && (iVar1 = FUN_1084d318(uVar2), iVar1 == 0)) {
      uVar3 = FUN_1084d2f4(uVar2,3);
      iVar1 = FUN_10874420(uVar2,0xe);
      iVar4 = FUN_10874420(uVar2,0xd);
      if ((uint)(iVar1 - iVar4) < uVar3) {
        uVar3 = iVar1 - iVar4;
      }
      iVar1 = FUN_10869f7c(param_2,7,param_3);
      *(char *)(iVar1 + 7) = (char)param_3;
      *(undefined1 *)(iVar1 + 4) = 1;
      *(uint *)(iVar1 + 0x14) = uVar3;
      *(undefined1 *)(iVar1 + 5) = 1;
      FUN_10867b48(param_2,iVar1);
      return;
    }
  }
  return;
}

