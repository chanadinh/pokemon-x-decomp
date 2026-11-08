/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108be340(undefined4 param_1,undefined4 param_2,int param_3)

{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  
  uVar2 = FUN_1086b91c(5);
  uVar5 = 0;
  if (uVar2 != 0) {
    do {
      iVar3 = FUN_1086b91c(uVar5 + 6 & 0xff);
      if (iVar3 == param_3) {
        iVar3 = FUN_10865c48(param_2);
        if (iVar3 != 0) {
          return;
        }
        uVar4 = FUN_108544a0(param_2,param_3);
        iVar3 = FUN_1085f844();
        if (iVar3 != 0) {
          return;
        }
        uVar1 = FUN_1086b91c(0x16);
        if ((uVar1 & 0xff) == 0x12) {
          return;
        }
        iVar3 = FUN_1086f960(uVar4,uVar1 & 0xff);
        if (iVar3 != 0) {
          return;
        }
        FUN_1084c948(param_2,1,param_3);
        iVar3 = FUN_10869f7c(param_2,0x13,param_3);
        *(char *)(iVar3 + 6) = (char)param_3;
        *(ushort *)(iVar3 + 4) = uVar1 & 0x1f | (uVar1 & 0x1f) << 5 | 0x4800;
        FUN_10867b48(param_2);
        FUN_1084c948(param_2,2,param_3);
        return;
      }
      uVar5 = uVar5 + 1;
    } while (uVar5 < uVar2);
  }
  return;
}

