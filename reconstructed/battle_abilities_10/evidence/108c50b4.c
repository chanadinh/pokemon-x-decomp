/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c50b4(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 *param_4)

{
  uint uVar1;
  int iVar2;
  uint *puVar3;
  
  uVar1 = FUN_1084be58(param_2);
  iVar2 = FUN_108544a0(param_2,param_3);
  if (*(short *)(iVar2 + 0xc) == 0x15f) {
    if ((uVar1 == 0) || ((uVar1 != 1 && uVar1 != 2) && uVar1 != 3)) {
      uVar1 = 0;
    }
    if (uVar1 != *(byte *)(iVar2 + 0x153)) {
      puVar3 = (uint *)FUN_10869f7c(param_2,0x3a,param_3);
      *(char *)((int)puVar3 + 5) = (char)uVar1;
      *(char *)(puVar3 + 1) = (char)param_3;
      *puVar3 = *puVar3 | 0x800000;
      FUN_1084d2d4(puVar3 + 2,2,0xde);
      FUN_1084d29c(puVar3 + 2,param_3);
      FUN_10867b48(param_2,puVar3);
    }
  }
  *param_4 = 1;
  return;
}

