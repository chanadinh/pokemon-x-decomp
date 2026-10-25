/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_001381c4(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined1 auStack_1c [8];
  int local_14;
  
  FUN_00127ccc(param_1,param_2,auStack_1c);
  iVar1 = FUN_001336bc(param_1 + 0x18,0,local_14 + param_3,local_14 + param_3 >> 0x1f,0);
  if ((iVar1 < 0) && (iVar1 < 0)) {
    FUN_00124504(iVar1,0x138208);
  }
  return;
}

