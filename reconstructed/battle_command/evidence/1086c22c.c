/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1086c22c(undefined4 param_1,byte param_2,undefined4 *param_3,undefined4 param_4)

{
  byte bVar1;
  uint uVar2;
  undefined4 **ppuVar3;
  uint uVar4;
  bool bVar5;
  undefined4 *apuStack_8 [2];
  
  apuStack_8[1] = (undefined4 *)param_4;
  apuStack_8[0] = param_3;
  if (param_2 < 0x6d) {
    bVar1 = (&DAT_108d13fc)[param_2];
    uVar4 = bVar1 & 0xf;
    if (0x10 < uVar4) {
      param_3 = (undefined4 *)0x0;
      FUN_10800394(0,0);
    }
    bVar5 = (bVar1 & 0xf) != 0;
    if (bVar5) {
      param_3 = &DAT_10b0f460;
    }
    uVar2 = 0;
    ppuVar3 = apuStack_8;
    if (bVar5) {
      do {
        param_3[uVar2] = *ppuVar3;
        uVar2 = uVar2 + 1;
        ppuVar3 = ppuVar3 + 1;
      } while (uVar2 < uVar4);
    }
    FUN_1084dbf4(param_1,param_2,(uint)bVar1,&DAT_10b0f460);
    return;
  }
  FUN_10800394(0,0);
  return;
}

