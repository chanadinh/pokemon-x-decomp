/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

longlong FUN_0013358c(uint *param_1,int param_2,uint param_3,uint param_4)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint unaff_r5;
  uint unaff_r6;
  uint local_28;
  uint local_24;
  
  iVar2 = -0x1f1fb93f;
  if ((int)param_4 < 0) {
LAB_0013360c:
    return CONCAT44(param_2,iVar2);
  }
  uVar3 = param_1[3];
  uVar1 = param_1[4];
  uVar4 = param_3 - uVar3;
  if ((int)(param_4 - (uVar1 + (param_3 < uVar3))) < 0 ==
      (SBORROW4(param_4,uVar1) != SBORROW4(param_4 - uVar1,(uint)(param_3 < uVar3)))) {
    iVar2 = FUN_00133130(&local_28,*param_1 & 0xfffffffe);
    if (iVar2 < 0) {
      param_1[3] = 0;
      param_1[4] = 0;
      local_24 = unaff_r5;
      local_28 = unaff_r6;
    }
    else {
      param_1[4] = local_24;
      param_1[3] = local_28;
    }
    param_2 = -(iVar2 >> 0x1f);
    if ((param_2 != 0) ||
       (uVar4 = 0, iVar2 = -0x1f1fb93f,
       (int)(local_24 - (param_4 + (local_28 < param_3))) < 0 !=
       (SBORROW4(local_24,param_4) != SBORROW4(local_24 - param_4,(uint)(local_28 < param_3)))))
    goto LAB_0013360c;
  }
  param_1[1] = param_3;
  param_1[2] = param_4;
  return (ulonglong)uVar4 << 0x20;
}

