/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b7b94(int param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  int iVar1;
  undefined4 uVar2;
  bool bVar3;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_10856a88(uVar2,0x19);
    if (iVar1 == 0) {
      iVar1 = FUN_10856a88(uVar2,2);
      bVar3 = iVar1 == 0;
      if (bVar3) {
        iVar1 = param_4[6];
      }
      if (bVar3 && iVar1 == 0) {
        iVar1 = FUN_10869f7c(param_2,0xb,param_3);
        *(undefined2 *)(iVar1 + 4) = 0x19;
        uVar2 = FUN_1084beac(5,*(undefined2 *)(param_1 + 0x38));
        *(undefined4 *)(iVar1 + 8) = uVar2;
        *(undefined1 *)(iVar1 + 0xc) = 0;
        *(char *)(iVar1 + 0xf) = (char)param_3;
        FUN_10867b48(param_2,iVar1);
        param_4[6] = 1;
        param_4[1] = 5;
        *param_4 = 0;
      }
    }
  }
  return;
}

