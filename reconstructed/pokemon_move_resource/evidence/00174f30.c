/* Unreviewed Ghidra pseudocode obtained through live REA MCP; types require assembly review. */

void move_resource_load(int param_1,uint param_2)

{
  if (0x269 < param_2) {
    param_2 = 0;
  }
  if (*(ushort *)(param_1 + 4) != param_2) {
    FUN_00138214(DAT_005e5338,param_2,*(undefined4 *)(param_1 + 8));
    *(short *)(param_1 + 4) = (short)param_2;
  }
  return;
}

