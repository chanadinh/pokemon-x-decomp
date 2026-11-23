/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1082399c(int param_1,undefined4 param_2,int param_3,int param_4)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  *(undefined1 *)(param_4 + 0x70) = 0;
  while( true ) {
    bVar1 = *(byte *)(param_4 + 0x70);
    if ((uint)*(byte *)(param_4 + 0x6e) <= (uint)bVar1) {
      return;
    }
    *(byte *)(param_4 + 0x70) = bVar1 + 1;
    iVar4 = *(int *)(param_4 + (uint)bVar1 * 4);
    if (iVar4 == 0) break;
    iVar2 = FUN_10865ae0(param_1,param_3,iVar4);
    if ((iVar2 == 0) && (iVar2 = FUN_10874348(iVar4), iVar2 != 0x12)) {
      FUN_1086bb84();
      FUN_1086bac0(3,*(undefined1 *)(param_3 + 0x19));
      FUN_1086bac0(4,*(undefined1 *)(iVar4 + 0x19));
      FUN_1086bac0(0x22,iVar2);
      FUN_1086b9fc(0x43,1);
      FUN_1086b9fc(0x50,1);
      FUN_1086b9a4(param_1,0xa5);
      iVar2 = FUN_1086b91c(0x43);
      iVar3 = FUN_1086b91c(0x50);
      FUN_1086b880();
      if ((iVar2 != 0) && (FUN_10869784(param_4,iVar4), iVar3 != 0)) {
        FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0xd5,*(undefined1 *)(iVar4 + 0x19),
                     0xffff0000);
      }
    }
  }
  return;
}

