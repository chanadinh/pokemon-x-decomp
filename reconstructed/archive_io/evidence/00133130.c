/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_00133130(int param_1,int *param_2)

{
  if (param_1 != 0 && param_2 != (int *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x00133150. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*param_2 + 0xc))(param_2,param_1);
    return;
  }
  return;
}

