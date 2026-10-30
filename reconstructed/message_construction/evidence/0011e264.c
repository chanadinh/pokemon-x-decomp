/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 *
FUN_0011e264(undefined4 *param_1,undefined4 param_2,undefined4 param_3,int *param_4,
            undefined4 param_5,undefined4 param_6)

{
  undefined4 uVar1;
  int iVar2;
  
  param_1[2] = 0;
  *param_1 = &DAT_005985b4;
  param_1[1] = param_4;
  *(char *)(param_1 + 3) = (char)param_5;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  uVar1 = (**(code **)(*param_4 + 0x1c))(param_4);
  iVar2 = FUN_0012b0c8(param_4,0x78,uVar1,1);
  uVar1 = 0;
  if (iVar2 != 0) {
    uVar1 = FUN_00128288(iVar2,param_4,param_2,param_6);
  }
  param_1[2] = uVar1;
  FUN_0012ad74(param_1,param_3,param_4,param_5);
  return param_1;
}

