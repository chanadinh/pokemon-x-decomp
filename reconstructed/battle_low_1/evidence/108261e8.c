/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_108261e8(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  uint uVar2;
  
  FUN_1086bb84();
  FUN_1086bac0(4,param_3);
  FUN_1086bac0(3,param_2);
  FUN_1086b520(0x27,param_4);
  FUN_1086b9a4(param_1,0xb5);
  uVar1 = FUN_1086b91c(0x27);
  FUN_1086b880();
  uVar2 = thunk_FUN_00174bb4(DAT_108fca54,100);
  return uVar2 < uVar1;
}

