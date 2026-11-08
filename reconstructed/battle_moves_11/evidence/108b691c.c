/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b691c(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_1086b91c(0x40);
    if (*param_4 == 1) {
      FUN_1084d2d4(uVar2,2,0x2e9);
      FUN_1084d29c(uVar2,param_3);
      FUN_108658d4(0x53,1);
      return;
    }
    if (*param_4 == 2) {
      FUN_1084d2d4(uVar2,2,0x2ec);
      FUN_1084d29c(uVar2,param_3);
      FUN_108658d4(0x53,1);
      return;
    }
  }
  return;
}

