/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108abe30(int param_1)

{
  undefined1 uVar1;
  undefined2 uVar2;
  uint uVar3;
  undefined4 uVar4;
  uint uVar5;
  uint local_1c;
  
  uVar3 = (uint)*(ushort *)(param_1 + 0x12);
  uVar4 = 0;
  if (uVar3 != 0) {
    uVar5 = 0;
    while ((&DAT_108d382c)[uVar5 * 2] != uVar3) {
      uVar5 = uVar5 + 1;
      if (0xb5 < uVar5) {
        return 0;
      }
    }
    uVar2 = FUN_108743a8(param_1,0xc);
    uVar1 = *(undefined1 *)(param_1 + 0x19);
    uVar4 = (*(code *)(&DAT_108d3830)[uVar5 * 2])(&local_1c);
    uVar4 = FUN_1084cfc0(5,uVar3,6,uVar2,uVar1,uVar4,local_1c & 0xffff);
  }
  return uVar4;
}

