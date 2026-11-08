/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a7598(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint in_fpscr;
  float fVar2;
  
  iVar1 = FUN_1086b91c(4);
  if (iVar1 != param_3) {
    return;
  }
  iVar1 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
  if (100 - iVar1 == 0) {
    fVar2 = -0.5;
  }
  else {
    fVar2 = (float)VectorUnsignedToFloat((100 - iVar1) * 0x1000,(byte)(in_fpscr >> 0x16) & 3);
    fVar2 = fVar2 + 0.5;
  }
  FUN_1084ce84(0x36,(int)fVar2 / 100);
  return;
}

