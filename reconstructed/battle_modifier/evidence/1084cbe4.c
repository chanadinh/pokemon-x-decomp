/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_1084cbe4(int param_1)

{
  int iVar1;
  uint in_fpscr;
  float fVar2;
  float fVar3;
  
  iVar1 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
  fVar2 = 0.5;
  fVar3 = (float)VectorSignedToFloat(iVar1 * 0x1000 + 0x64000,(byte)(in_fpscr >> 0x16) & 3);
  if (iVar1 != -100 && -1 < iVar1 + 100) {
    fVar2 = fVar3 + 0.5;
  }
  if (iVar1 + 100 < 1) {
    fVar2 = fVar3 - fVar2;
  }
  return (int)fVar2 / 100;
}

