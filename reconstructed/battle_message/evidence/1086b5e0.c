/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1086b5e0(uint *param_1,char param_2,undefined2 param_3,int param_4)

{
  uint uVar1;
  int iVar2;
  int *piVar3;
  int local_4;
  
  local_4 = param_4;
  FUN_10862078(param_1,param_2);
  FUN_10862078(param_1,param_3);
  piVar3 = &local_4;
  if (param_2 == 'k') {
    FUN_1084db70(param_1,local_4);
    piVar3 = (int *)register0x00000054;
  }
  do {
    iVar2 = *piVar3;
    if (0x176c < *param_1) {
      thunk_FUN_0011e864(0,0);
    }
    uVar1 = *param_1;
    *param_1 = uVar1 + 1;
    *(char *)((int)param_1 + uVar1 + 8) = (char)((uint)iVar2 >> 0x18);
    uVar1 = *param_1;
    *param_1 = uVar1 + 1;
    *(char *)((int)param_1 + uVar1 + 8) = (char)((uint)iVar2 >> 0x10);
    uVar1 = *param_1;
    *param_1 = uVar1 + 1;
    *(char *)((int)param_1 + uVar1 + 8) = (char)((uint)iVar2 >> 8);
    uVar1 = *param_1;
    *param_1 = uVar1 + 1;
    *(char *)((int)param_1 + uVar1 + 8) = (char)iVar2;
    piVar3 = piVar3 + 1;
  } while (iVar2 != -0x10000);
  return;
}

