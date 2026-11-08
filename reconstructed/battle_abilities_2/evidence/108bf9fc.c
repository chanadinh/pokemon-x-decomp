/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bf9fc(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint *puVar2;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) && (iVar1 = FUN_1084be58(param_2), iVar1 == 2)) {
    FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1086d8ac();
    if (iVar1 != 0) {
      FUN_1084c948(param_2,1,param_3);
      puVar2 = (uint *)FUN_10869f7c(param_2,10,param_3);
      *(undefined1 *)(puVar2 + 1) = 0x26;
      *(char *)((int)puVar2 + 5) = (char)param_3;
      *(undefined1 *)((int)puVar2 + 0x11) = 1;
      *puVar2 = *puVar2 | 0x2000000;
      FUN_10867b48(param_2);
      FUN_1084c948(param_2,2,param_3);
      return;
    }
  }
  return;
}

