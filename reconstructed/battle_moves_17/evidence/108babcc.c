/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108babcc(int param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar3 = FUN_1086b91c(4);
  if (iVar3 == param_3) {
    FUN_108544a0(param_2,param_3);
    iVar3 = FUN_1085f844();
    if (iVar3 != 0) {
      uVar2 = FUN_1086b91c(0x12);
      iVar3 = FUN_1084c808(uVar2);
      if (iVar3 == 0) {
        uVar1 = FUN_1086b91c(3);
        uVar4 = FUN_108544a0(param_2,uVar1);
        iVar3 = FUN_1084c5a8(param_3,uVar1);
        if ((iVar3 == 0) && (iVar3 = FUN_1085f844(uVar4), iVar3 == 0)) {
          uVar2 = FUN_1086b91c(0x14);
          FUN_1084c700(param_1);
          iVar3 = FUN_10869f7c(param_2,0x12,uVar1);
          *(undefined2 *)(iVar3 + 6) = uVar2;
          *(char *)(iVar3 + 4) = (char)param_3;
          *(undefined1 *)(iVar3 + 5) = 1;
          FUN_10867b48(param_2);
          iVar3 = FUN_10869f7c(param_2,0x12,param_3);
          *(undefined1 *)(iVar3 + 4) = uVar1;
          *(undefined2 *)(iVar3 + 6) = *(undefined2 *)(param_1 + 0x38);
          FUN_1084d2d4(iVar3 + 8,2,0x275);
          FUN_1084d29c(iVar3 + 8,param_3);
          FUN_10867b48(param_2,iVar3);
        }
      }
      FUN_1084d1cc(param_1);
      return;
    }
  }
  return;
}

