/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10871250(int param_1,undefined4 param_2,undefined2 param_3)

{
  undefined4 uVar1;
  
  uVar1 = FUN_108620d4();
  switch(uVar1) {
  case 8:
    *(undefined2 *)(param_1 + 0xf6) = param_3;
    return;
  case 9:
    *(undefined2 *)(param_1 + 0xf8) = param_3;
    return;
  case 10:
    *(undefined2 *)(param_1 + 0xfa) = param_3;
    return;
  case 0xb:
    *(undefined2 *)(param_1 + 0xfc) = param_3;
    return;
  case 0xc:
    *(undefined2 *)(param_1 + 0xfe) = param_3;
    return;
  default:
    FUN_10800394(0,0);
    return;
  }
}

