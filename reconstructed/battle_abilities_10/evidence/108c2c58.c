/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c2c58(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  int iVar1;
  
  if (((*param_4 != 0) && (iVar1 = FUN_108544a0(param_2,param_3), *(short *)(iVar1 + 0xc) == 0x15f))
     && (*(char *)(iVar1 + 0x153) != '\0')) {
    iVar1 = FUN_10869f7c(param_2,0x3a,param_3);
    *(char *)(iVar1 + 4) = (char)param_3;
    *(undefined1 *)(iVar1 + 5) = 0;
    FUN_1084d2d4(iVar1 + 8,2,0xde);
    FUN_1084d29c(iVar1 + 8,param_3);
    FUN_10867b48(param_2,iVar1);
    return;
  }
  return;
}

