/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084b028(undefined4 param_1,int param_2,undefined4 param_3,int param_4,undefined4 param_5)

{
  undefined1 uVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  char cVar6;
  undefined2 local_28;
  undefined2 local_26;
  undefined1 local_23;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 == param_2) {
    iVar3 = FUN_108544a0(param_1,param_2);
    uVar1 = *(undefined1 *)(iVar3 + 0x19);
    cVar6 = '\0';
    do {
      cVar2 = cVar6 + '\x01';
      iVar4 = FUN_1084bac4(iVar3,0,cVar6,&local_28);
      if (iVar4 == 0) {
        return;
      }
      iVar4 = FUN_1084c5a8(uVar1,local_23);
      cVar6 = cVar2;
    } while ((iVar4 != 0) ||
            ((param_4 != 0 && (iVar4 = thunk_FUN_0039ddc0(local_28), iVar4 != param_4))));
    uVar5 = FUN_1089e708(local_26,param_5);
    FUN_108658d4(0x38,uVar5);
  }
  return;
}

