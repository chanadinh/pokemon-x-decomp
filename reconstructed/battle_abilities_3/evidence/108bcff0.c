/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bcff0(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint *puVar3;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1084ca4c(uVar2,0);
    if (iVar1 != 0) {
      puVar3 = (uint *)FUN_10869f7c(param_2,0xd,param_3);
      *(char *)(puVar3 + 3) = (char)param_3;
      *(undefined1 *)(puVar3 + 1) = 5;
      *puVar3 = *puVar3 | 0x800000;
      *(undefined1 *)((int)puVar3 + 0x16) = 1;
      *(undefined1 *)((int)puVar3 + 0x17) = 1;
      FUN_10867b48(param_2);
      return;
    }
  }
  return;
}

