/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bb138(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined1 *puVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  
  FUN_1084bfc0(param_3);
  uVar1 = FUN_1084af00();
  iVar2 = FUN_10869f7c(param_2,0x1b,param_3);
  *(undefined1 *)(iVar2 + 8) = uVar1;
  *(undefined1 *)(iVar2 + 4) = 4;
  iVar5 = 1;
  puVar3 = (undefined1 *)(iVar2 + 5);
  *puVar3 = 0;
  do {
    iVar5 = iVar5 + -1;
    puVar3[1] = 0;
    puVar3 = puVar3 + 2;
    *puVar3 = 0;
  } while (iVar5 != 0);
  uVar6 = 0;
  do {
    iVar5 = FUN_1084b3e8(uVar6 & 0xff);
    if (iVar5 != 0) {
      uVar4 = (uVar6 >> 3) + 1 & 0xff;
      if (uVar4 < *(byte *)(iVar2 + 4)) {
        *(byte *)(iVar2 + 4 + uVar4) = *(byte *)(iVar2 + 4 + uVar4) | (byte)(1 << (uVar6 & 7));
      }
    }
    uVar6 = uVar6 + 1;
  } while (uVar6 < 0x11);
  FUN_10867b48(param_2,iVar2);
  return;
}

