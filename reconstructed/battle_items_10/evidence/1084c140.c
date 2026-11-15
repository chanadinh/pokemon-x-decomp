/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1084c140(int param_1)

{
  char cVar1;
  
  cVar1 = *(char *)(param_1 + 0x104);
  if ('\x05' < cVar1) {
    cVar1 = *(char *)(param_1 + 0x105);
  }
  if ('\x05' < cVar1) {
    cVar1 = *(char *)(param_1 + 0x106);
    if ('\x05' < cVar1) {
      cVar1 = *(char *)(param_1 + 0x107);
    }
    if ('\x05' < cVar1) {
      cVar1 = *(char *)(param_1 + 0x108);
      if ('\x05' < cVar1) {
        cVar1 = *(char *)(param_1 + 0x109);
      }
      if (('\x05' < cVar1) && ('\x05' < *(char *)(param_1 + 0x10a))) {
        return 0;
      }
    }
  }
  return 1;
}

