/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c0140(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  uint *puVar5;
  
  iVar3 = FUN_1086b91c(2);
  if ((iVar3 == param_3) && (sVar1 = FUN_1086b91c(0x1e), sVar1 == 5)) {
    uVar4 = FUN_108544a0(param_2,param_3);
    FUN_108658d4(0x33,0);
    puVar5 = (uint *)FUN_10869f7c(param_2,4,param_3);
    uVar2 = FUN_1084d2f4(uVar4,8);
    *(char *)((int)puVar5 + 6) = (char)param_3;
    *(undefined2 *)(puVar5 + 1) = uVar2;
    *puVar5 = *puVar5 | 0x800000;
    FUN_10867b48(param_2,puVar5);
    return;
  }
  return;
}

