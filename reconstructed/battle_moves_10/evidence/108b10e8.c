/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b10e8(undefined4 param_1,undefined4 param_2,int param_3)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int unaff_r9;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 != param_3) {
    return;
  }
  iVar2 = FUN_108544a0(param_2,param_3);
  uVar3 = (uint)*(byte *)(iVar2 + 0x152);
  if (uVar3 != 0) {
    unaff_r9 = 0x183;
  }
  uVar6 = 0;
  uVar5 = 0;
  bVar1 = false;
  if (uVar3 != 0) {
    do {
      iVar4 = FUN_10855c68(iVar2,uVar5);
      if (iVar4 == unaff_r9) {
        bVar1 = true;
      }
      else {
        iVar4 = FUN_108c8714(iVar2,uVar5);
        if (iVar4 != 0) {
          uVar6 = uVar6 + 1 & 0xff;
        }
      }
      uVar5 = uVar5 + 1 & 0xff;
    } while (uVar5 < uVar3);
    if (((bVar1) && (1 < uVar3)) && ((int)(uVar3 - 1) <= (int)uVar6)) {
      return;
    }
  }
  FUN_108658d4(0x23,0x1b);
  return;
}

