/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c19bc(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint *puVar2;
  
  iVar1 = FUN_1086b91c(4);
  if (((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x47), iVar1 == 0)) &&
     (iVar1 = FUN_1086b91c(0x16), iVar1 == 0x10)) {
    puVar2 = (uint *)FUN_10869f7c(param_2,0xd,param_3);
    *(char *)(puVar2 + 3) = (char)param_3;
    *(undefined1 *)((int)puVar2 + 0x16) = 1;
    *(undefined1 *)(puVar2 + 1) = 1;
    *(undefined1 *)((int)puVar2 + 0x17) = 1;
    *puVar2 = *puVar2 | 0x800000;
    FUN_10867b48(param_2);
    return;
  }
  return;
}

