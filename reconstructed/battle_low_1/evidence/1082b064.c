/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1082b064(undefined4 param_1,undefined2 *param_2,int param_3,uint param_4,int param_5)

{
  uint uVar1;
  
  if (param_5 == 0) {
    param_2[3] = (short)param_3;
    *(undefined1 *)((int)param_2 + 0xb) = 0;
    if (param_3 == 0) {
      *(undefined1 *)((int)param_2 + 9) = 0;
      uVar1 = 0;
    }
    else {
      uVar1 = thunk_FUN_00150220(param_3,0);
      *(char *)((int)param_2 + 9) = (char)uVar1;
    }
    if (param_4 != 0) {
      uVar1 = (uint)*(byte *)((int)param_2 + 9);
    }
    if (param_4 != 0 && param_4 < uVar1) {
      *(char *)((int)param_2 + 9) = (char)param_4;
    }
    *(undefined1 *)(param_2 + 4) = *(undefined1 *)((int)param_2 + 9);
    *(undefined1 *)(param_2 + 6) = 0;
  }
  else {
    *param_2 = (short)param_3;
    *(undefined1 *)((int)param_2 + 5) = 0;
    if (param_3 == 0) {
      *(undefined1 *)((int)param_2 + 3) = 0;
      uVar1 = 0;
    }
    else {
      uVar1 = thunk_FUN_00150220(param_3,0);
      *(char *)((int)param_2 + 3) = (char)uVar1;
    }
    if (param_4 != 0) {
      uVar1 = (uint)*(byte *)((int)param_2 + 3);
    }
    if (param_4 != 0 && param_4 < uVar1) {
      *(char *)((int)param_2 + 3) = (char)param_4;
    }
    *(undefined1 *)(param_2 + 1) = *(undefined1 *)((int)param_2 + 3);
    if (*(char *)(param_2 + 6) != '\0') {
      FUN_1085fa8c(param_2 + 3,param_2);
      return;
    }
  }
  return;
}

