/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0012b808(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  bool bVar4;
  
  bVar4 = *(char *)(*(int *)(param_1 + 0xc) + 0xd) == '\0';
  if (bVar4) {
    FUN_0013a768(*(undefined4 *)(param_1 + 0xc));
  }
  uVar1 = FUN_00152fe0(*(undefined4 *)(param_1 + 0xc));
  uVar2 = FUN_00152fa8(*(undefined4 *)(param_1 + 0xc));
  uVar3 = FUN_0016b45c(*(undefined4 *)(param_1 + 0xc));
  uVar1 = FUN_00168b3c(uVar1,uVar2,uVar3);
  FUN_0014fd04(*(undefined4 *)(param_1 + 0xc),uVar1);
  FUN_0013aa84(param_1,param_2);
  FUN_0014fb00(param_1);
  FUN_0014fbd4(param_1);
  FUN_0014fda4(param_1);
  FUN_0014fe78(param_1);
  FUN_0014fa2c(param_1);
  if ((*(byte *)(*(int *)(param_1 + 0xc) + 0xd) & bVar4) != 0) {
    FUN_0013a720(*(undefined4 *)(param_1 + 0xc));
    return;
  }
  return;
}

