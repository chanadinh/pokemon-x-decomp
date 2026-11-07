/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bcd28(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  uint *puVar3;
  
  iVar2 = FUN_1086b91c(4);
  if (((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x47), iVar2 == 0)) &&
     ((cVar1 = FUN_1086b91c(0x16), cVar1 == '\x10' || (cVar1 == '\x06' || cVar1 == '\a')))) {
    puVar3 = (uint *)FUN_10869f7c(param_2,0xd,param_3);
    *(char *)(puVar3 + 3) = (char)param_3;
    *(undefined1 *)((int)puVar3 + 0x16) = 1;
    *puVar3 = *puVar3 | 0x800000;
    *(undefined1 *)(puVar3 + 1) = 5;
    *(undefined1 *)((int)puVar3 + 0x17) = 1;
    FUN_10867b48(param_2);
    return;
  }
  return;
}

