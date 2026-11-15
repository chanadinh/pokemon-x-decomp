/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bcf0c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  uint *puVar4;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_10873158(param_2,param_3);
    uVar3 = FUN_1084bf14(param_2);
    uVar2 = FUN_1086dddc(uVar3,uVar2);
    uVar2 = FUN_1084d348(param_2,uVar2);
    uVar3 = FUN_108544a0(param_2,uVar2);
    FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1085f844();
    if ((iVar1 == 0) && (iVar1 = FUN_1085f844(uVar3), iVar1 == 0)) {
      puVar4 = (uint *)FUN_10869f7c(param_2,0x34,param_3);
      *(char *)(puVar4 + 1) = (char)uVar2;
      *puVar4 = *puVar4 | 0x800000;
      FUN_1084d2d4(puVar4 + 2,2,0x284);
      FUN_1084d29c(puVar4 + 2,param_3);
      FUN_1084d29c(puVar4 + 2,(char)puVar4[1]);
      FUN_10867b48(param_2,puVar4);
      return;
    }
  }
  return;
}

