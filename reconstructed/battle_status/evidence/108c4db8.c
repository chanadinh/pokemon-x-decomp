/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c4db8(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 *param_4)

{
  char cVar1;
  char cVar2;
  ushort uVar3;
  int iVar4;
  undefined4 uVar5;
  
  cVar1 = FUN_1086b91c(4);
  iVar4 = FUN_1084c5a8(param_3,cVar1);
  if (iVar4 != 0) {
    uVar5 = FUN_108544a0(param_2,cVar1);
    iVar4 = FUN_1086f960(uVar5,0xb);
    if ((iVar4 != 0) && (cVar2 = FUN_1086b91c(3), cVar2 != cVar1)) {
      uVar3 = FUN_1086b91c(0x1e);
      if ((uVar3 < 6) || (uVar3 == 0xe)) {
        uVar5 = FUN_108658d4(0x42,1);
        *param_4 = uVar5;
      }
    }
  }
  return;
}

