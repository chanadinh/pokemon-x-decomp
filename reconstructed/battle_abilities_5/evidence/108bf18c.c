/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bf18c(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  short sVar2;
  int iVar3;
  uint *puVar4;
  
  iVar3 = FUN_1086b91c(4);
  if (iVar3 == param_3) {
    sVar2 = FUN_1086b91c(0x21);
    FUN_108658d4(0x21,0);
    if (sVar2 != 0) {
      puVar4 = (uint *)FUN_10869f7c(param_2,6,param_3);
      *puVar4 = *puVar4 | 0x800000;
      uVar1 = FUN_1086b91c(3);
      *(short *)(puVar4 + 1) = sVar2;
      *(undefined1 *)((int)puVar4 + 6) = uVar1;
      FUN_1084d2d4(puVar4 + 3,2,0x1c9);
      FUN_1084d29c(puVar4 + 3,*(undefined1 *)((int)puVar4 + 6));
      FUN_10867b48(param_2,puVar4);
      return;
    }
  }
  return;
}

