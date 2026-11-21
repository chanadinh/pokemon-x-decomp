/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ac704(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  char cVar2;
  undefined2 uVar3;
  int iVar4;
  undefined4 uVar5;
  
  uVar1 = FUN_1086b91c(4);
  iVar4 = FUN_1084bfc0(uVar1);
  if (iVar4 == param_3) {
    uVar3 = FUN_1086b91c(0x12);
    cVar2 = thunk_FUN_00168bbc(uVar3,0x1b);
    iVar4 = thunk_FUN_0039e058(uVar3);
    if (((iVar4 != 0) && (cVar2 == '\x05' || cVar2 == '\x04')) &&
       (iVar4 = FUN_108658d4(0x41,1), iVar4 != 0)) {
      uVar5 = FUN_1086b91c(0x40);
      FUN_1084d2d4(uVar5,2,0x31d);
      FUN_1084d29c(uVar5,uVar1);
      return;
    }
  }
  return;
}

