/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b150c(int param_1,int param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 != param_3) {
    return;
  }
  uVar1 = *(undefined2 *)(param_1 + 0x38);
  iVar3 = FUN_1084d364(param_2);
  iVar5 = 0;
  iVar2 = iVar3 + -1;
  if (-1 < iVar3 + -1) {
    do {
      iVar3 = iVar2 + -1;
      iVar2 = FUN_108c9a64(param_2 + 0x20,uVar1,iVar2);
      if (iVar2 == 0) break;
      iVar5 = iVar5 + 1;
      iVar2 = iVar3;
    } while (-1 < iVar3);
    if (iVar5 != 0) {
      if (iVar5 == 1) {
        uVar4 = 0x50;
      }
      else if (iVar5 == 2) {
        uVar4 = 0x78;
      }
      else if (iVar5 == 3) {
        uVar4 = 0xa0;
      }
      else {
        uVar4 = 200;
      }
      goto LAB_108b15b0;
    }
  }
  uVar4 = 0x28;
LAB_108b15b0:
  FUN_108658d4(0x31,uVar4);
  return;
}

