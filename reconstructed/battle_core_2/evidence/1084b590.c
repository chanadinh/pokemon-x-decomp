/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined1 FUN_1084b590(int param_1,uint param_2,undefined4 param_3,uint param_4)

{
  undefined1 *puVar1;
  int iVar2;
  
  iVar2 = *(int *)(param_1 + 0x1244);
  if (iVar2 != 0) {
    param_4 = *(uint *)(param_1 + 0x1248);
  }
  if (iVar2 == 0 || param_4 <= param_2) {
    puVar1 = (undefined1 *)(param_1 + 0x124c);
  }
  else {
    puVar1 = (undefined1 *)(iVar2 + param_2);
  }
  return *puVar1;
}

