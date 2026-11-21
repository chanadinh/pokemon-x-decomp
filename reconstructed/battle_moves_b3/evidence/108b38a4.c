/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b38a4(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 uVar5;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(6);
    uVar3 = FUN_108544a0(param_2,uVar1);
    iVar2 = FUN_10856a88(uVar3,0xd);
    if ((((iVar2 == 0) && (iVar2 = FUN_1084c278(uVar3), iVar2 != 0)) && (iVar2 != 0xa5)) &&
       (iVar4 = FUN_108c8548(uVar3,iVar2), iVar4 != 0)) {
      uVar5 = 4;
      iVar4 = FUN_1084c818(uVar3,1);
      if (iVar4 != 0) {
        uVar5 = 5;
      }
      iVar4 = FUN_10869f7c(param_2,0xb,param_3);
      *(undefined2 *)(iVar4 + 4) = 0xd;
      uVar3 = FUN_1084beac(uVar5,iVar2);
      *(undefined4 *)(iVar4 + 8) = uVar3;
      *(undefined1 *)(iVar4 + 0xf) = uVar1;
      FUN_10867b48(param_2,iVar4);
      return;
    }
  }
  return;
}

