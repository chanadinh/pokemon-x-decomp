/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10882188(uint param_1,undefined4 param_2,int param_3,int param_4,short *param_5)

{
  byte bVar1;
  ushort uVar2;
  uint uVar3;
  undefined4 uVar4;
  short sVar5;
  
  uVar3 = 0;
  while ((ushort)(&DAT_108d25fe)[uVar3 * 3] != param_1) {
    uVar3 = uVar3 + 1;
    if (0x10 < uVar3) {
      return 0;
    }
  }
  if (param_4 != 0) {
    sVar5 = (&DAT_108d2602)[uVar3 * 3];
  }
  else {
    sVar5 = (&DAT_108d2600)[uVar3 * 3];
  }
  if (sVar5 < 0) {
    sVar5 = (&DAT_108d2600)[uVar3 * 3];
  }
  if (sVar5 < 0) {
    return 0;
  }
  uVar2 = param_5[1];
  *param_5 = sVar5;
  param_5[1] = uVar2 & 0xc000 | 2;
  bVar1 = *(byte *)(param_3 + 0x19);
  uVar2 = uVar2 >> 0xe;
  param_5[1] = (ushort)((uVar2 | 0x80000) + 0x2000000 >> 0x12) | uVar2 << 0xe;
  *(uint *)(param_5 + 2) = (uint)bVar1;
  if (param_4 != 0) {
    uVar2 = param_5[1];
    if ((uVar2 & 0x3fff) >> 7 < 8) {
      param_5[1] = (ushort)(((uint)(uVar2 >> 0xe) | (uint)uVar2 << 0x12) + 0x2000000 >> 0x12) |
                   (uVar2 >> 0xe) << 0xe;
      *(int *)((int)param_5 + (uVar2 >> 5 & 0x1fc) + 4) = param_4;
    }
  }
  if (param_1 == 8) {
    uVar4 = FUN_10871408(param_2);
    uVar2 = param_5[1];
    if ((uVar2 & 0x3fff) >> 7 < 8) {
      param_5[1] = (ushort)(((uint)(uVar2 >> 0xe) | (uint)uVar2 << 0x12) + 0x2000000 >> 0x12) |
                   (uVar2 >> 0xe) << 0xe;
      *(undefined4 *)((int)param_5 + (uVar2 >> 5 & 0x1fc) + 4) = uVar4;
    }
  }
  else if (param_1 == 0x12) {
    uVar2 = param_5[1];
    if ((uVar2 & 0x3fff) >> 7 < 8) {
      param_5[1] = (ushort)(((uint)(uVar2 >> 0xe) | (uint)uVar2 << 0x12) + 0x2000000 >> 0x12) |
                   (uVar2 >> 0xe) << 0xe;
      *(undefined4 *)((int)param_5 + (uVar2 >> 5 & 0x1fc) + 4) = 0x49;
    }
  }
  return 1;
}

