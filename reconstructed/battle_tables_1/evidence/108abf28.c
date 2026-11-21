/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_108abf28(int param_1,int param_2)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  uint local_1c;
  
  if (param_2 != 0) {
    uVar5 = 0;
    do {
      if ((&DAT_108d382c)[uVar5 * 2] == param_2) {
        uVar2 = FUN_108743a8(param_1,0xc);
        uVar1 = *(undefined1 *)(param_1 + 0x19);
        uVar3 = (*(code *)(&DAT_108d3830)[uVar5 * 2])(&local_1c);
        iVar4 = FUN_1084cfc0(5,param_2,6,uVar2,uVar1,uVar3,local_1c & 0xffff);
        if (iVar4 == 0) {
          return 0;
        }
        FUN_10882674();
        FUN_1084cf8c(iVar4,6,1);
        return iVar4;
      }
      uVar5 = uVar5 + 1;
    } while (uVar5 < 0xb6);
  }
  return 0;
}

