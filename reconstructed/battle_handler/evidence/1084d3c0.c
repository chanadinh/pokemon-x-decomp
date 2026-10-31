/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_1084d3c0(uint param_1,uint param_2)

{
  int iVar1;
  
  iVar1 = DAT_108fca60;
  while( true ) {
    if (iVar1 == 0) {
      return 0;
    }
    if ((((*(uint *)(iVar1 + 0x18) & 0x20000000) != 0) && (*(byte *)(iVar1 + 0x10) == param_1)) &&
       (*(byte *)(iVar1 + 0x3a) == param_2 && (*(uint *)(iVar1 + 0x18) & 0x8000000) == 0)) break;
    iVar1 = *(int *)(iVar1 + 4);
  }
  return iVar1;
}

