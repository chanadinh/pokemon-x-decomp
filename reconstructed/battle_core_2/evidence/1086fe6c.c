/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1086fe6c(ushort *param_1,int param_2,uint param_3)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  ushort *puVar4;
  
  if (param_3 != 0) {
    uVar2 = param_3 & 1;
    uVar3 = 0;
    if (uVar2 != 0) {
      do {
        if ((ushort *)(uint)*(ushort *)(param_2 + uVar3 * 2) == param_1) {
          return 1;
        }
        uVar3 = uVar3 + 1;
      } while (uVar3 < uVar2);
    }
    for (; uVar2 < param_3; uVar2 = uVar2 + 2) {
      puVar4 = (ushort *)(param_2 + uVar2 * 2);
      uVar1 = *puVar4;
      if ((ushort *)(uint)uVar1 != param_1) {
        puVar4 = (ushort *)(uint)puVar4[1];
      }
      if ((ushort *)(uint)uVar1 == param_1 || puVar4 == param_1) {
        return 1;
      }
    }
  }
  return 0;
}

