/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108abc80(int param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    uVar1 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
    iVar2 = FUN_1084c0f8(uVar1);
    if (((iVar2 != 0) || (iVar2 = FUN_1084c0ec(param_2,2), iVar2 != 0)) &&
       (iVar2 = FUN_108658d4(0x11,2), iVar2 != 0)) {
      FUN_1084c96c(param_1);
      FUN_1084c948(param_2,0,param_3);
      return;
    }
  }
  return;
}

