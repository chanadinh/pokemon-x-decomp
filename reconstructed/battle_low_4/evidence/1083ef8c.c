/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1083ef8c(int param_1)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0x197c) {
    DAT_108fcb84 = 0x197c;
  }
  iVar1 = *(int *)(param_1 + 0x6b8);
  if (iVar1 == 0) {
    if (DAT_108fcb84 != 0xfc4) {
      DAT_108fcb84 = 0xfc4;
    }
    iVar1 = FUN_10858cb8(param_1,0);
    if (iVar1 == 0) {
      return 0;
    }
    FUN_1085db0c(param_1,0);
    FUN_10859cc4(*(int *)(param_1 + 400) + 0x280);
  }
  else if (iVar1 == 100) {
    if ((*(int *)(param_1 + 0x1b0) != -1) &&
       (iVar1 = thunk_FUN_00495a7c(*(undefined4 *)(param_1 + 0xcc),
                                   param_1 + *(int *)(param_1 + 0x1b0) * 4 + 0x1b8), iVar1 == 0)) {
      return 0;
    }
    FUN_1083ac28(param_1);
  }
  else {
    if (iVar1 != 200) {
      return 0;
    }
    FUN_108574e8(param_1);
  }
  return 1;
}

