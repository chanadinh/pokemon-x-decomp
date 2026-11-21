/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b3b18(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  uint *puVar5;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 == param_3) {
    uVar1 = FUN_1086b91c(6);
    uVar4 = FUN_108544a0(param_2,uVar1);
    iVar3 = FUN_10869f7c(param_2,0x13,param_3);
    *(char *)(iVar3 + 6) = (char)param_3;
    uVar2 = FUN_10870994(uVar4);
    *(undefined2 *)(iVar3 + 4) = uVar2;
    *(undefined1 *)(iVar3 + 7) = 1;
    FUN_10867b48(param_2,iVar3);
    puVar5 = (uint *)FUN_10869f7c(param_2,3,param_3);
    *puVar5 = *puVar5 | 0x1000000;
    FUN_1084d2d4(puVar5 + 1,2,0x447);
    FUN_1084d29c(puVar5 + 1,param_3);
    FUN_1084d29c(puVar5 + 1,uVar1);
    FUN_10867b48(param_2,puVar5);
    return;
  }
  return;
}

