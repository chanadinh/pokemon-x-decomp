/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b4e64(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = FUN_108544a0(param_2,param_3);
  if ((*(short *)(iVar2 + 0xc) == 0x288) && (iVar3 = FUN_1086b91c(3), iVar3 == param_3)) {
    cVar1 = *(char *)(iVar2 + 0x153);
    iVar2 = FUN_10869f7c(param_2,0x3a,param_3);
    *(bool *)(iVar2 + 5) = cVar1 == '\0';
    *(char *)(iVar2 + 4) = (char)param_3;
    FUN_1084d2d4(iVar2 + 8,2,0xde);
    FUN_1084d29c(iVar2 + 8,param_3);
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

