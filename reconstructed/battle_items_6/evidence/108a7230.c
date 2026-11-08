/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a7230(int param_1,undefined4 param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  uint in_fpscr;
  float fVar3;
  
  iVar2 = FUN_1086b91c(4);
  if (iVar2 != param_3) {
    return;
  }
  iVar2 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
  uVar1 = 100U - iVar2 & 0xffff;
  if (uVar1 == 0) {
    fVar3 = -0.5;
  }
  else {
    fVar3 = (float)VectorUnsignedToFloat(uVar1 << 0xc,(byte)(in_fpscr >> 0x16) & 3);
    fVar3 = fVar3 + 0.5;
  }
  FUN_1084ce84(0x36,(int)fVar3 / 100);
  return;
}

