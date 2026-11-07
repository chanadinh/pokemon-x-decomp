/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10867bbc(int param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  undefined4 uVar1;
  
  uVar1 = *(undefined4 *)(param_1 + 0xc);
  if (param_3 == 1) {
    FUN_1086b5e0(uVar1,0x69,param_2,*param_4,0xffff0000);
    return;
  }
  if (param_3 == 2) {
    FUN_1086b5e0(uVar1,0x69,param_2,*param_4,param_4[1],0xffff0000);
    return;
  }
  if (param_3 == 3) {
    FUN_1086b5e0(uVar1,0x69,param_2,*param_4,param_4[1],param_4[2],0xffff0000);
    return;
  }
  if (param_3 != 4) {
    FUN_1086b5e0(uVar1,0x69,param_2,0xffff0000);
    return;
  }
  FUN_1086b5e0(uVar1,0x69,param_2,*param_4,param_4[1],param_4[2],param_4[3],0xffff0000);
  return;
}

