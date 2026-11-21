/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

short FUN_1081d2b0(undefined4 param_1,int param_2,undefined4 param_3)

{
  char cVar1;
  short sVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  undefined1 auStack_20 [4];
  int local_1c;
  
  uVar3 = FUN_108676d0();
  cVar1 = *(char *)(param_2 + 0x19);
  sVar2 = 0;
  FUN_108656c4(param_1,auStack_20);
  while (iVar4 = FUN_10865568(param_1,auStack_20,&local_1c), iVar4 != 0) {
    if ((*(char *)(local_1c + 0x19) != cVar1) &&
       (uVar5 = FUN_108676d0(param_1,local_1c,param_3), uVar3 < uVar5)) {
      sVar2 = sVar2 + 1;
    }
  }
  return sVar2;
}

