/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108af7d8(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  uint *puVar4;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    iVar2 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar2 + 4,2,0x1ff);
    FUN_1084d29c(iVar2 + 4,param_3);
    FUN_10867b48(param_2,iVar2);
    *(undefined4 *)(param_4 + 0x18) = 1;
    uVar3 = FUN_108544a0(param_2,param_3);
    puVar4 = (uint *)FUN_10869f7c(param_2,0x27,param_3);
    cVar1 = FUN_1084bb2c(uVar3,3);
    *(char *)((int)puVar4 + 6) = cVar1 + '\x01';
    *(char *)(puVar4 + 1) = (char)param_3;
    *(undefined1 *)((int)puVar4 + 5) = 3;
    *puVar4 = *puVar4 & 0xfeffffff;
    FUN_10867b48(param_2,puVar4);
    return;
  }
  return;
}

