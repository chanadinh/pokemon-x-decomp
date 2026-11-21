/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b4678(int param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  uint *puVar5;
  
  iVar3 = FUN_1086b91c(3);
  if (((iVar3 == param_3) && (iVar3 = FUN_1084ca78(param_2,param_3), iVar3 != 0)) &&
     (iVar3 = FUN_1084c000(param_2), iVar3 != 0)) {
    uVar4 = FUN_1087425c(param_3);
    iVar3 = FUN_10869f7c(param_2,0x2a,param_3);
    *(char *)(iVar3 + 0x4c) = (char)param_3;
    FUN_1084d2d4(iVar3 + 4,2,0x302);
    FUN_1084d29c(iVar3 + 4,param_3);
    FUN_1084d29c(iVar3 + 4,uVar4);
    FUN_10867b48(param_2,iVar3);
    puVar5 = (uint *)FUN_10869f7c(param_2,0x38,param_3);
    if (*(short *)(param_1 + 0x38) == 0x171) {
      uVar2 = 0x2bf;
    }
    else {
      uVar2 = 0x2c0;
    }
    *(undefined2 *)(puVar5 + 1) = uVar2;
    uVar1 = FUN_10873158(param_2,param_3);
    *(undefined1 *)((int)puVar5 + 6) = uVar1;
    *(undefined1 *)((int)puVar5 + 7) = 10;
    *puVar5 = *puVar5 | 0x1000000;
    FUN_10867b48(param_2,puVar5);
    return;
  }
  return;
}

