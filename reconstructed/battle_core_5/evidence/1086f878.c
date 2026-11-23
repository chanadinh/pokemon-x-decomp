/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1086f878(int param_1,uint param_2,int param_3)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  bool bVar6;
  
  if (param_2 < 0xb) {
    iVar5 = param_1 + param_2 * 4;
    if ((*(int *)(iVar5 + 0x274) != 0) && (uVar1 = *(uint *)(iVar5 + 0x248), uVar1 < 10)) {
      param_1 = param_1 + param_2 * 0x28;
      if (uVar1 != 0) {
        uVar2 = uVar1 & 1;
        uVar3 = 0;
        if (uVar2 != 0) {
          do {
            if (*(int *)(param_1 + uVar3 * 4 + 0x90) == param_3) {
              return 0;
            }
            uVar3 = uVar3 + 1;
          } while (uVar3 < uVar2);
        }
        for (; uVar2 < uVar1; uVar2 = uVar2 + 2) {
          iVar4 = param_1 + uVar2 * 4;
          bVar6 = *(int *)(iVar4 + 0x90) == param_3;
          if (!bVar6) {
            iVar4 = *(int *)(iVar4 + 0x94);
          }
          if (bVar6 || iVar4 == param_3) {
            return 0;
          }
        }
      }
      *(int *)(param_1 + uVar1 * 4 + 0x90) = param_3;
      *(uint *)(iVar5 + 0x248) = uVar1 + 1;
      return 1;
    }
  }
  else {
    FUN_10800394(0,0);
    FUN_10800394(0,0);
  }
  return 0;
}

