/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ae5e0(int param_1)

{
  undefined1 uVar1;
  int iVar2;
  
  uVar1 = *(undefined1 *)(param_1 + 0x19);
  while (iVar2 = FUN_1084d3c0(0,uVar1), iVar2 != 0) {
    FUN_1084d1cc();
  }
  return;
}

