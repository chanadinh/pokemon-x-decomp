/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bd6e0(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  uint *puVar4;
  uint uVar5;
  
  iVar3 = FUN_1086b91c(3);
  if (((iVar3 == param_3) && (iVar3 = FUN_1086b91c(0x47), iVar3 == 0)) &&
     (iVar3 = FUN_1086b91c(0x48), iVar3 == 0)) {
    uVar2 = FUN_1086b91c(0x12);
    iVar3 = thunk_FUN_0039df44(uVar2,0);
    if ((iVar3 != 0) &&
       ((iVar3 = FUN_1084c0f8(0x1e), iVar3 != 0 || (iVar3 = FUN_1084c0ec(param_2,1), iVar3 != 0))))
    {
      puVar4 = (uint *)FUN_10869f7c(param_2,0xb,param_3);
      *puVar4 = *puVar4 | 0x800000;
      uVar1 = FUN_1086b91c(4);
      *(undefined1 *)((int)puVar4 + 0xf) = uVar1;
      *(undefined2 *)(puVar4 + 1) = 5;
      uVar5 = FUN_10860e00();
      puVar4[2] = uVar5;
      FUN_1084d2d4(puVar4 + 5,2,0x1d8);
      FUN_1084d29c(puVar4 + 5,*(undefined1 *)((int)puVar4 + 0xf));
      FUN_10867b48(param_2,puVar4);
      return;
    }
  }
  return;
}

