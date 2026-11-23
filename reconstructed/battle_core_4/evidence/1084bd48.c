/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084bd48(undefined4 param_1,int param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  uint *puVar4;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_2) {
    FUN_108544a0(param_1,param_2);
    iVar2 = FUN_10869f7c(param_1,0x15,param_2);
    *(char *)(iVar2 + 5) = (char)param_2;
    *(undefined1 *)(iVar2 + 4) = 7;
    FUN_10867b48(param_1);
    uVar3 = FUN_108544a0(param_1,param_2);
    puVar4 = (uint *)FUN_10869f7c(param_1,0x27,param_2);
    cVar1 = FUN_1084bb2c(uVar3,3);
    *(char *)(puVar4 + 1) = (char)param_2;
    *(undefined1 *)((int)puVar4 + 5) = 3;
    *(char *)((int)puVar4 + 6) = cVar1 + '\x01';
    *puVar4 = *puVar4 & 0xfeffffff;
    FUN_10867b48(param_1,puVar4);
    iVar2 = FUN_10869f7c(param_1,3,param_2);
    FUN_1084d2d4(iVar2 + 4,2,0x205);
    FUN_1084d29c(iVar2 + 4,param_2);
    FUN_10867b48(param_1,iVar2);
    return;
  }
  return;
}

