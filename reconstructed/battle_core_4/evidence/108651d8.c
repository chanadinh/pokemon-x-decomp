/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108651d8(undefined4 param_1,int param_2,int param_3)

{
  char cVar1;
  char cVar2;
  undefined4 uVar3;
  
  if (param_3 == 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = FUN_1086f960(param_2,2);
  }
  FUN_1086bb84();
  FUN_1086bac0(2,*(undefined1 *)(param_2 + 0x19));
  FUN_1086b9fc(0x53,uVar3);
  FUN_1086b9fc(0x42,0);
  FUN_1086b9a4(param_1,0x12);
  if ((*(uint *)(param_2 + 0x70) & 7) != 0) {
    FUN_108658d4(0x42,1);
  }
  if ((*(uint *)(param_2 + 0x98) & 7) != 0) {
    FUN_108658d4(0x42,1);
  }
  if (((*(uint *)(param_2 + 0x9c) & 7) != 0) || ((*(uint *)(param_2 + 0x94) & 7) != 0)) {
    FUN_108658d4(0x53,1);
  }
  cVar1 = FUN_1086b91c(0x53);
  cVar2 = FUN_1086b91c(0x42);
  FUN_1086b880();
  if ((cVar2 == '\0') && (cVar1 != '\0')) {
    uVar3 = 1;
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}

