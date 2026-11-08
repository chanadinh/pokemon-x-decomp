/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c4280(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  uint *puVar5;
  
  iVar3 = FUN_1086b91c(2);
  if (iVar3 == param_3) {
    uVar4 = FUN_108544a0(param_2,param_3);
    cVar1 = FUN_1086b91c(0x3a);
    if (cVar1 == '\x01') {
      FUN_1084c948(param_2,1,param_3);
      iVar3 = FUN_10869f7c(param_2,6,param_3);
      *(char *)(iVar3 + 6) = (char)param_3;
      uVar2 = FUN_1084d2f4(uVar4,8);
      *(undefined2 *)(iVar3 + 4) = uVar2;
      FUN_10867b48(param_2,iVar3);
      FUN_1084c948(param_2,2,param_3);
      return;
    }
    if (cVar1 == '\x02') {
      puVar5 = (uint *)FUN_10869f7c(param_2,4,param_3);
      *(char *)((int)puVar5 + 6) = (char)param_3;
      *puVar5 = *puVar5 | 0x800000;
      uVar2 = FUN_1084d2f4(uVar4,8);
      *(undefined2 *)(puVar5 + 1) = uVar2;
      FUN_10867b48(param_2,puVar5);
      return;
    }
  }
  return;
}

