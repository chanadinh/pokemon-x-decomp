/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c27ac(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint *puVar2;
  
  iVar1 = FUN_1086b91c(0x23);
  if ((iVar1 == 6) && (iVar1 = FUN_1086b91c(2), iVar1 == param_3)) {
    puVar2 = (uint *)FUN_10869f7c(param_2,0xd,param_3);
    *(undefined1 *)(puVar2 + 1) = 5;
    *puVar2 = *puVar2 | 0x800000;
    *(undefined1 *)((int)puVar2 + 0x17) = 1;
    *(byte *)(puVar2 + 6) = (byte)puVar2[6] & 0xfd;
    *(undefined1 *)((int)puVar2 + 0x16) = 1;
    *(char *)(puVar2 + 3) = (char)param_3;
    FUN_10867b48(param_2);
    return;
  }
  return;
}

