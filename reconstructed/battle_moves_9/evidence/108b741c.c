/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b741c(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  byte local_20 [4];
  char local_1c [8];
  
  iVar2 = FUN_1086b91c(3);
  if ((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x48), iVar2 == 0)) {
    uVar1 = FUN_1086b91c(6);
    iVar2 = FUN_108261e8(param_2,param_3,uVar1,0x1e);
    if (iVar2 != 0) {
      uVar3 = FUN_108544a0(param_2,param_3);
      FUN_108bb8ac(param_2,local_1c,local_20);
      if (local_1c[0] == '\0') {
        iVar2 = FUN_10869f7c(param_2,0xb,param_3);
        *(undefined1 *)(iVar2 + 0xf) = uVar1;
        *(ushort *)(iVar2 + 4) = (ushort)local_20[0];
        FUN_1084c438(local_20[0],uVar3,iVar2 + 8);
        FUN_10867b48(param_2,iVar2);
        return;
      }
      if (local_1c[0] == '\x01') {
        iVar2 = FUN_10869f7c(param_2,0xd,param_3);
        *(undefined1 *)(iVar2 + 0xc) = uVar1;
        *(undefined1 *)(iVar2 + 0x16) = 1;
        *(byte *)(iVar2 + 4) = local_20[0];
        *(undefined1 *)(iVar2 + 0x17) = 0xff;
        *(byte *)(iVar2 + 0x18) = *(byte *)(iVar2 + 0x18) | 4;
        FUN_10867b48(param_2);
        return;
      }
      if (local_1c[0] == '\x02') {
        iVar2 = FUN_10869f7c(param_2,0x2c,param_3);
        *(undefined1 *)(iVar2 + 4) = uVar1;
        *(undefined1 *)(iVar2 + 5) = 100;
        FUN_10867b48(param_2);
      }
    }
  }
  return;
}

