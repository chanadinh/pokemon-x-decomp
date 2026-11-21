/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1086fd30(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
  iVar1 = param_1 + param_2 * 4;
  iVar3 = 0;
  *(undefined4 *)(iVar1 + 0xc) = 0;
  *(undefined4 *)(iVar1 + 0x38) = 0;
  *(undefined4 *)(iVar1 + 100) = 0;
  *(undefined4 *)(iVar1 + 0x248) = 0;
  *(undefined4 *)(iVar1 + 0x274) = 0;
  uVar2 = 0;
  param_1 = param_1 + param_2 * 0x28;
  do {
    iVar1 = iVar3 + 1;
    uVar2 = uVar2 + 2;
    *(undefined4 *)(param_1 + iVar3 * 4 + 0x90) = 0x1f;
    iVar3 = iVar3 + 2;
    *(undefined4 *)(param_1 + iVar1 * 4 + 0x90) = 0x1f;
  } while (uVar2 < 10);
  return;
}

