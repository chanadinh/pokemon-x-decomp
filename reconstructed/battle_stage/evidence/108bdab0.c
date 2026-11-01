/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bdab0(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  uint *puVar3;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(3);
    iVar2 = FUN_1084c5a8(param_3,uVar1);
    if ((iVar2 == 0) && (iVar2 = FUN_1086b91c(0x21), iVar2 < 0)) {
      puVar3 = (uint *)FUN_10869f7c(param_2,0xd,param_3);
      *(undefined1 *)(puVar3 + 1) = 1;
      *puVar3 = *puVar3 | 0x800000;
      *(undefined1 *)((int)puVar3 + 0x17) = 2;
      *(byte *)(puVar3 + 6) = (byte)puVar3[6] | 2;
      *(undefined1 *)((int)puVar3 + 0x16) = 1;
      *(char *)(puVar3 + 3) = (char)param_3;
      FUN_10867b48(param_2);
      return;
    }
  }
  return;
}

