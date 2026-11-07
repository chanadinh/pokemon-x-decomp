/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108714fc(int param_1,undefined4 param_2,uint param_3)

{
  undefined1 *puVar1;
  
  switch(param_2) {
  default:
    goto switchD_10871500_caseD_0;
  case 1:
    puVar1 = (undefined1 *)(param_1 + 0x104);
    break;
  case 2:
    puVar1 = (undefined1 *)(param_1 + 0x105);
    break;
  case 3:
    puVar1 = (undefined1 *)(param_1 + 0x106);
    break;
  case 4:
    puVar1 = (undefined1 *)(param_1 + 0x107);
    break;
  case 5:
    puVar1 = (undefined1 *)(param_1 + 0x108);
    break;
  case 6:
    puVar1 = (undefined1 *)(param_1 + 0x109);
    break;
  case 7:
    puVar1 = (undefined1 *)(param_1 + 0x10a);
  }
  if (param_3 < 0xd) {
    *puVar1 = (char)param_3;
    return;
  }
switchD_10871500_caseD_0:
  FUN_10800394(0,0);
  return;
}

