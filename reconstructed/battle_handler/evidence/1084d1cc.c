/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084d1cc(int *param_1)

{
  uint uVar1;
  
  if ((param_1 != (int *)0x0) && (uVar1 = param_1[6], (uVar1 & 0x20000000) != 0)) {
    if ((uVar1 & 0x1000000) == 0) {
      if (param_1 == DAT_108fca60) {
        DAT_108fca60 = (int *)param_1[1];
      }
      if (*param_1 != 0) {
        *(int *)(*param_1 + 4) = param_1[1];
      }
      if ((int *)param_1[1] != (int *)0x0) {
        *(int *)param_1[1] = *param_1;
      }
      if (DAT_108fca64 == 0) {
        FUN_10800394(0,0);
        return;
      }
      *param_1 = 0;
      param_1[1] = 0;
      param_1[2] = 0;
      param_1[3] = 0;
      param_1[4] = 0;
      param_1[5] = 0;
      param_1[6] = 0;
      param_1[7] = 0;
      param_1[8] = 0;
      param_1[9] = 0;
      param_1[10] = 0;
      param_1[0xb] = 0;
      param_1[0xc] = 0;
      param_1[0xd] = 0;
      param_1[0xe] = 0;
      DAT_108fca64 = DAT_108fca64 + -1;
      (&DAT_10b1196c)[DAT_108fca64] = param_1;
    }
    else {
      param_1[6] = uVar1 | 0x8000000;
    }
  }
  return;
}

