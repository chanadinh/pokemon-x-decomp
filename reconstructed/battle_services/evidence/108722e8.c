/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108722e8(int param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  char *pcVar2;
  
  cVar1 = '\0';
  switch(param_2) {
  default:
    FUN_10800394(0,0);
    goto LAB_10872370;
  case 1:
    pcVar2 = (char *)(param_1 + 0x104);
    break;
  case 2:
    pcVar2 = (char *)(param_1 + 0x105);
    break;
  case 3:
    pcVar2 = (char *)(param_1 + 0x106);
    break;
  case 4:
    pcVar2 = (char *)(param_1 + 0x107);
    break;
  case 5:
    pcVar2 = (char *)(param_1 + 0x108);
    break;
  case 6:
    pcVar2 = (char *)(param_1 + 0x109);
    break;
  case 7:
    pcVar2 = (char *)(param_1 + 0x10a);
  }
  cVar1 = *pcVar2;
LAB_10872370:
  if (param_3 < 1) {
    if ('\0' < cVar1) {
      return 1;
    }
  }
  else if (cVar1 < '\f') {
    return 1;
  }
  return 0;
}

