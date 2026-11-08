/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bfbe8(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  
  iVar1 = FUN_108544a0(param_2,param_3);
  if (*(short *)(iVar1 + 0xc) == 0x22b) {
    uVar2 = FUN_10874420(iVar1,0xd);
    uVar3 = FUN_1084c124(iVar1,2);
    if ((bool)*(char *)(iVar1 + 0x153) != uVar2 <= uVar3) {
      puVar4 = (uint *)FUN_10869f7c(param_2,0x3a,param_3);
      *(char *)(puVar4 + 1) = (char)param_3;
      *(bool *)((int)puVar4 + 5) = uVar2 <= uVar3;
      *puVar4 = *puVar4 | 0x800000;
      FUN_1084d2d4(puVar4 + 2,1);
      FUN_10867b48(param_2,puVar4);
      return;
    }
  }
  return;
}

