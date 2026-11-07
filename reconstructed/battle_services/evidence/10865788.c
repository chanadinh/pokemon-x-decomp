/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10865788(int param_1,int param_2,uint param_3,int param_4)

{
  undefined1 uVar1;
  uint uVar2;
  int iVar3;
  
  uVar1 = *(undefined1 *)(param_2 + 0x19);
  if ((int)param_3 < 1) {
    if ((int)param_3 < 0) {
      FUN_10867a04(param_1,param_2,-param_3,0);
    }
  }
  else {
    uVar2 = (uint)*(ushort *)(param_2 + 0x10) + (param_3 & 0xffff);
    *(short *)(param_2 + 0x10) = (short)uVar2;
    if ((uint)*(ushort *)(param_2 + 0xe) < (uVar2 & 0xffff)) {
      *(ushort *)(param_2 + 0x10) = *(ushort *)(param_2 + 0xe);
    }
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),3,uVar1,param_3 & 0xffff);
  }
  if ((param_4 != 0) && (iVar3 = FUN_1086c2cc(param_1 + 0x120c,uVar1), iVar3 != 10)) {
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x48,uVar1);
    return;
  }
  return;
}

