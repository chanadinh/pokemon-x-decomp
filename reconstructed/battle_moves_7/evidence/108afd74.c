/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108afd74(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  undefined1 uVar2;
  int iVar3;
  uint *puVar4;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 == param_3) {
    uVar2 = FUN_1086b91c(6);
    iVar3 = FUN_108544a0(param_2,uVar2);
    uVar1 = *(undefined2 *)(iVar3 + 0x12);
    iVar3 = thunk_FUN_0039f0bc();
    if (iVar3 != 0) {
      iVar3 = FUN_10869f7c(param_2,0x21,param_3);
      *(undefined1 *)(iVar3 + 6) = uVar2;
      *(undefined2 *)(iVar3 + 4) = 0;
      FUN_1084d2d4(iVar3 + 0xc,2,0x308);
      FUN_1084d29c(iVar3 + 0xc,param_3);
      FUN_1084d29c(iVar3 + 0xc,uVar1);
      FUN_10867b48(param_2,iVar3);
      puVar4 = (uint *)FUN_10869f7c(param_2,0x23,param_3);
      *(undefined2 *)((int)puVar4 + 6) = uVar1;
      *(char *)(puVar4 + 1) = (char)param_3;
      *(undefined1 *)((int)puVar4 + 5) = 1;
      *puVar4 = *puVar4 | 0x1000000;
      FUN_10867b48(param_2);
      return;
    }
  }
  return;
}

