/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bf22c(int param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 uVar3;
  uint *puVar4;
  
  iVar2 = FUN_1086b91c(2);
  if ((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x52), iVar2 != 0)) {
    uVar1 = FUN_1086b91c(0x2e);
    iVar2 = thunk_FUN_0039f0bc(uVar1);
    if (iVar2 != 0) {
      uVar3 = FUN_108544a0(param_2,param_3);
      puVar4 = (uint *)FUN_10869f7c(param_2,4,param_3);
      *(char *)((int)puVar4 + 6) = (char)param_3;
      *puVar4 = *puVar4 | 0x800000;
      uVar1 = FUN_1084d2f4(uVar3,3);
      *(undefined2 *)(puVar4 + 1) = uVar1;
      FUN_1084d2d4(puVar4 + 2,2,0x183);
      FUN_1084d29c(puVar4 + 2,param_3);
      FUN_1084d29c(puVar4 + 2,*(undefined2 *)(param_1 + 0x38));
      FUN_10867b48(param_2,puVar4);
      return;
    }
  }
  return;
}

