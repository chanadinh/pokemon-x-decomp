/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084ad50(undefined4 param_1,int param_2,undefined2 param_3,uint param_4,undefined4 param_5)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  uint *puVar4;
  
  iVar3 = FUN_1086b91c(4);
  if ((iVar3 == param_2) && (iVar3 = FUN_1086b91c(0x47), iVar3 == 0)) {
    uVar2 = FUN_1086b91c(0x12);
    iVar3 = thunk_FUN_0039df44(uVar2,0);
    if ((iVar3 != 0) &&
       ((iVar3 = FUN_1084c0f8(param_5), iVar3 != 0 || (iVar3 = FUN_1084c0ec(param_1,1), iVar3 != 0))
       )) {
      puVar4 = (uint *)FUN_10869f7c(param_1,0xb,param_2);
      *(undefined2 *)(puVar4 + 1) = param_3;
      puVar4[2] = param_4;
      *(undefined1 *)(puVar4 + 3) = 0;
      *puVar4 = *puVar4 | 0x800000;
      uVar1 = FUN_1086b91c(3);
      *(undefined1 *)((int)puVar4 + 0xf) = uVar1;
      FUN_10867b48(param_1,puVar4);
      return;
    }
  }
  return;
}

