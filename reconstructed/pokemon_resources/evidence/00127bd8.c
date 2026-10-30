/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined2 FUN_00127bd8(int param_1)

{
  int *piVar1;
  undefined4 uVar2;
  undefined2 local_14 [2];
  
  if (*(int *)(param_1 + 0x4c) < 0) {
    piVar1 = (int *)FUN_00128134(param_1,*(undefined4 *)(param_1 + 4));
    uVar2 = (**(code **)(*piVar1 + 0x24))(piVar1);
    if (piVar1 != (int *)0x0) {
      (**(code **)(*piVar1 + 4))(piVar1);
    }
    local_14[0] = 0;
    FUN_00127c5c(param_1,uVar2,2,local_14,0);
    return local_14[0];
  }
  return *(undefined2 *)(param_1 + 0x34);
}

