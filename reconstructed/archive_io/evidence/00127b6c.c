/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_00127b6c(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  undefined4 local_18;
  
  local_18 = param_4;
  iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x28))(param_1 + 8);
  local_18 = 0;
  if (iVar1 != 0) {
    local_18 = 0;
    uVar2 = FUN_00133634(param_1 + 0x18,&local_18,param_3,param_2);
    if ((uVar2 & 0x80000000) != 0) {
      if ((int)uVar2 < 0) {
        FUN_00124504(uVar2,0x127bcc);
      }
      local_18 = 0;
    }
  }
  return local_18;
}

