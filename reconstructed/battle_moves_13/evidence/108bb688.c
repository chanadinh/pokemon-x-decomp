/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

/* WARNING: Removing unreachable block (ram,0x108bb714) */

void FUN_108bb688(undefined4 param_1,undefined4 param_2,int param_3)

{
  bool bVar1;
  undefined1 uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  uint uVar6;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 == param_3) {
    uVar2 = FUN_1086b91c(6);
    uVar4 = FUN_108544a0(param_2,uVar2);
    uVar5 = FUN_1084bfc0(uVar2);
    uVar6 = 0;
    do {
      iVar3 = FUN_108acedc(param_2,uVar5,uVar6 & 0xff);
      if ((iVar3 != 0) && (iVar3 = FUN_1084b3e8(uVar6 & 0xff), iVar3 != 0)) {
        bVar1 = true;
        goto LAB_108bb734;
      }
      uVar6 = uVar6 + 1;
    } while (uVar6 < 0x11);
    bVar1 = false;
LAB_108bb734:
    iVar3 = FUN_1084c818(uVar4,7);
    if (iVar3 != 0 || bVar1) {
      FUN_1084b0ec(param_2,param_3,uVar4,0x208);
      return;
    }
  }
  return;
}

