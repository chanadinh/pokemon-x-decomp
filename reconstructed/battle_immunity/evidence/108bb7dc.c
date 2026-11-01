/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bb7dc(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 == param_3) {
    uVar2 = FUN_1086b91c(4);
    iVar3 = FUN_108544a0(param_2,uVar2);
    sVar1 = *(short *)(iVar3 + 0xc);
    if (((sVar1 == 0x32 || sVar1 == 0x33) ||
        (((sVar1 == 0x5e && (iVar4 = FUN_108c8540(iVar3), iVar4 != 0)) ||
         (iVar4 = FUN_10856a88(iVar3,0x1f), iVar4 != 0)))) ||
       (iVar3 = FUN_10856a88(iVar3,0x15), iVar3 != 0)) {
      FUN_108658d4(0x41,1);
      return;
    }
  }
  return;
}

