/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_00133634(uint *param_1,int *param_2,int param_3,uint param_4)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  uint local_20;
  
  iVar3 = 0;
  if (param_4 == 0) {
LAB_001336ac:
    iVar1 = 0;
    *param_2 = iVar3;
  }
  else {
    while (iVar1 = FUN_00146588(&local_20,*param_1 & 0xfffffffe,param_1[1],param_1[2],param_3,
                                param_4), -1 < iVar1) {
      uVar2 = param_1[1];
      iVar3 = iVar3 + local_20;
      param_1[1] = uVar2 + local_20;
      param_1[2] = param_1[2] + ((int)local_20 >> 0x1f) + (uint)CARRY4(uVar2,local_20);
      if ((local_20 == param_4) || (local_20 == 0)) goto LAB_001336ac;
      param_3 = param_3 + local_20;
      param_4 = param_4 - local_20;
    }
  }
  return iVar1;
}

