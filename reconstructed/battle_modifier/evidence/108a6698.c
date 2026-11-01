/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a6698(int param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  short sVar2;
  short sVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  uint uVar7;
  uint in_fpscr;
  float fVar8;
  
  iVar4 = FUN_1086b91c(3);
  if (iVar4 != param_3) {
    return;
  }
  uVar1 = FUN_1086b91c(0x12);
  iVar4 = FUN_1084c808(uVar1);
  if (iVar4 != 0) {
    return;
  }
  uVar5 = FUN_108544a0(param_2,param_3);
  sVar2 = FUN_108c86c8();
  if (sVar2 == 0) {
    return;
  }
  uVar6 = FUN_1086b91c(0x12);
  uVar7 = FUN_1084c7fc(uVar5);
  if ((uVar6 & 0xffff) != uVar7) {
    return;
  }
  sVar3 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
  uVar6 = (uint)(ushort)(sVar3 * sVar2 + 100);
  if (uVar6 < 0xc9) {
    if (uVar6 == 0) {
      fVar8 = -0.5;
      goto LAB_108a6758;
    }
  }
  else {
    uVar6 = 200;
  }
  fVar8 = (float)VectorUnsignedToFloat(uVar6 << 0xc,(byte)(in_fpscr >> 0x16) & 3);
  fVar8 = fVar8 + 0.5;
LAB_108a6758:
  FUN_1084ce84(0x36,(int)fVar8 / 100);
  return;
}

