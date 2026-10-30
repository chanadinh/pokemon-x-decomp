/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_001336bc(uint *param_1,uint param_2,uint param_3,int param_4,int param_5)

{
  int iVar1;
  uint unaff_r4;
  uint unaff_r5;
  bool bVar2;
  uint local_20;
  uint local_1c;
  
  if (param_5 != 0) {
    if (param_5 != 1) {
      if (param_5 == 2) {
        iVar1 = FUN_00133130(&local_20,*param_1 & 0xfffffffe);
        if (iVar1 < 0) {
          param_1[3] = 0;
          param_1[4] = 0;
          local_1c = unaff_r4;
          local_20 = unaff_r5;
        }
        else {
          param_1[4] = local_1c;
          param_1[3] = local_20;
        }
        if (-1 < iVar1) {
          bVar2 = CARRY4(param_3,local_20);
          param_3 = param_3 + local_20;
          param_4 = param_4 + local_1c + (uint)bVar2;
          param_2 = 0;
          goto LAB_00133748;
        }
      }
      else {
        iVar1 = -0x1f1fb943;
      }
      return iVar1;
    }
    param_2 = param_1[2];
    bVar2 = CARRY4(param_3,param_1[1]);
    param_3 = param_3 + param_1[1];
    param_4 = param_2 + param_4 + (uint)bVar2;
  }
LAB_00133748:
  iVar1 = FUN_0013358c(param_1,param_2,param_3,param_4);
  return iVar1;
}

