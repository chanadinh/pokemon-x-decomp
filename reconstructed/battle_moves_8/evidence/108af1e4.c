/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108af1e4(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  char cVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  
  iVar4 = FUN_1086b91c(3);
  if (iVar4 == param_3) {
    uVar1 = FUN_1086b91c(4);
    uVar5 = FUN_108544a0(param_2,uVar1);
    uVar6 = FUN_108544a0(param_2,param_3);
    cVar2 = FUN_10874420(uVar6,0x12);
    cVar3 = FUN_10874420(uVar5,0x12);
    if ((cVar2 == '\x02' || cVar3 == '\x02') || cVar2 == cVar3) {
      FUN_108658d4(0x41,1);
      return;
    }
  }
  return;
}

