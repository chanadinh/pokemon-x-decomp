/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b8cc4(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    uVar3 = FUN_108544a0(param_2,param_3);
    cVar1 = FUN_1086b91c(0x23);
    if (cVar1 == '\x02') {
      FUN_108658d4(0x23,0);
      return;
    }
    iVar2 = FUN_10856a88(uVar3,2);
    if (iVar2 == 0) {
      FUN_108658d4(0x23,0x1b);
      return;
    }
  }
  return;
}

