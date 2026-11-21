/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a5bd8(int param_1)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  uint uVar6;
  uint local_1c;
  
  iVar3 = FUN_1087c024(*(undefined1 *)(param_1 + 0x19),5);
  if ((iVar3 == 0) && (uVar4 = (uint)*(ushort *)(param_1 + 0x12), uVar4 != 0)) {
    uVar6 = 0;
    while ((&DAT_108d382c)[uVar6 * 2] != uVar4) {
      uVar6 = uVar6 + 1;
      if (0xb5 < uVar6) {
        return;
      }
    }
    uVar2 = FUN_108743a8(param_1,0xc);
    uVar1 = *(undefined1 *)(param_1 + 0x19);
    uVar5 = (*(code *)(&DAT_108d3830)[uVar6 * 2])(&local_1c);
    FUN_1084cfc0(5,uVar4,6,uVar2,uVar1,uVar5,local_1c & 0xffff);
  }
  return;
}

