/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084c5e0(int param_1,undefined4 param_2,int param_3,int *param_4,undefined4 param_5)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  
  if ((*param_4 == 0) && (iVar2 = FUN_1086b91c(2), iVar2 == param_3)) {
    puVar3 = (undefined4 *)FUN_108544a0(param_2,param_3);
    iVar2 = FUN_10869f7c(param_2,4,param_3);
    *(char *)(iVar2 + 6) = (char)param_3;
    uVar4 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
    uVar1 = FUN_1084d2f4(puVar3,uVar4);
    *(undefined2 *)(iVar2 + 4) = uVar1;
    FUN_1084d2d4(iVar2 + 8,2,0x392);
    FUN_1084d29c(iVar2 + 8,param_3);
    FUN_1084d29c(iVar2 + 8,*(undefined2 *)(param_1 + 0x38));
    FUN_10867b48(param_2,iVar2);
    iVar2 = thunk_FUN_0039bcb8(*puVar3,param_5);
    if (iVar2 == 2) {
      iVar2 = FUN_10869f7c(param_2,0xb,param_3);
      *(char *)(iVar2 + 0xf) = (char)param_3;
      *(undefined2 *)(iVar2 + 4) = 6;
      FUN_1084c438(6,puVar3,iVar2 + 8);
      FUN_10867b48(param_2,iVar2);
    }
    *param_4 = 1;
  }
  return;
}

