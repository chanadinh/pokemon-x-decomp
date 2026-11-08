/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c4bac(undefined4 param_1,undefined4 param_2,int param_3,uint param_4)

{
  int iVar1;
  uint uVar2;
  bool bVar3;
  uint local_18;
  
  local_18 = param_4;
  iVar1 = FUN_1086b91c(2);
  bVar3 = iVar1 == param_3;
  if (bVar3) {
    iVar1 = *(int *)(param_4 + 8);
  }
  if ((bVar3 && iVar1 == 0) &&
     ((4 < *(uint *)(param_4 + 4) ||
      (((iVar1 = FUN_1084b784(param_2,param_3,&local_18), iVar1 != 0 && ((local_18 & 0xf) != 3)) &&
       (uVar2 = *(int *)(param_4 + 4) + 1, *(uint *)(param_4 + 4) = uVar2, 4 < uVar2)))))) {
    iVar1 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar1 + 4,2,499);
    FUN_1084d29c(iVar1 + 4,param_3);
    FUN_10867b48(param_2,iVar1);
    *(undefined4 *)(param_4 + 8) = 1;
    *(undefined4 *)(param_4 + 0xc) = 0;
  }
  return;
}

