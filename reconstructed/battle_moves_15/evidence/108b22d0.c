/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b22d0(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    iVar2 = FUN_10869f7c(param_2,0x15,param_3);
    *(char *)(iVar2 + 5) = (char)param_3;
    *(undefined1 *)(iVar2 + 4) = 5;
    FUN_10867b48(param_2);
    FUN_108544a0(param_2,param_3);
    iVar2 = FUN_1084b804();
    if (iVar2 == 0) {
      iVar2 = FUN_10869f7c(param_2,0x38,param_3);
      *(undefined2 *)(iVar2 + 4) = 0x2ad;
      uVar1 = FUN_10873158(param_2,param_3);
      *(undefined1 *)(iVar2 + 6) = uVar1;
      *(undefined1 *)(iVar2 + 7) = 10;
      *(undefined1 *)(iVar2 + 0xb) = 1;
      FUN_10867b48(param_2,iVar2);
    }
    iVar2 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar2 + 4,2,0x268);
    FUN_1084d29c(iVar2 + 4,param_3);
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

