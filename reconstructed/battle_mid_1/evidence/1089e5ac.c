/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1089e5ac(undefined4 param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  uVar3 = 0;
  uVar2 = 0;
  do {
    iVar1 = FUN_10860c80(param_1,uVar2 & 0xff);
    if (iVar1 == 6 || iVar1 == 0) {
      *(char *)(param_2 + uVar3) = (char)uVar2;
      uVar3 = uVar3 + 1 & 0xff;
    }
    uVar2 = uVar2 + 1;
  } while (uVar2 < 0x12);
  return uVar3;
}

