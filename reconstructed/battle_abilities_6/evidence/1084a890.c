/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1084a890(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  short sVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int local_18;
  
  local_18 = param_4;
  iVar2 = thunk_FUN_0039dcf0();
  iVar3 = thunk_FUN_00168bbc(param_1,10);
  if (iVar3 == 0) {
    if (iVar2 == 4) {
      sVar1 = thunk_FUN_00168bbc(param_1,0xb);
      if (sVar1 == 8) {
        return 0;
      }
    }
    else if (iVar2 != 6) {
      if (iVar2 == 7) {
        uVar4 = thunk_FUN_0039df08(param_1);
        uVar5 = 0;
        if (uVar4 != 0) {
          do {
            thunk_FUN_0039ddd4(param_1,uVar5,&local_18);
            if (local_18 < 0) {
              return 0;
            }
            uVar5 = uVar5 + 1;
          } while (uVar5 < uVar4);
        }
      }
      else if (param_1 != 0x122) {
        return 0;
      }
    }
  }
  return 1;
}

