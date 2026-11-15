/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bd33c(undefined4 param_1,undefined4 param_2,uint param_3)

{
  byte bVar1;
  ushort uVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  
  uVar3 = FUN_1086b91c(4);
  if (uVar3 == param_3) {
    uVar2 = FUN_1086b91c(0x1e);
    uVar2 = uVar2 & 0xff;
    if (((uVar2 == 5 || uVar2 == 1) || uVar2 == 4) &&
       (bVar1 = FUN_1086b91c(3), bVar1 != 0x1f && bVar1 != param_3)) {
      uVar4 = FUN_108544a0(param_2,param_3);
      uVar5 = FUN_1086b91c(0x1f);
      FUN_1084c948(param_2,1,param_3);
      iVar6 = FUN_10869f7c(param_2,0xb,param_3);
      *(byte *)(iVar6 + 0xf) = bVar1;
      *(ushort *)(iVar6 + 4) = uVar2;
      if ((uVar2 == 5) && (iVar7 = FUN_1087a1c0(uVar5), iVar7 != 0)) {
        *(undefined4 *)(iVar6 + 8) = uVar5;
      }
      else {
        FUN_1084c438(uVar2,uVar4,iVar6 + 8);
      }
      *(undefined1 *)(iVar6 + 0xc) = 1;
      FUN_10867b48(param_2,iVar6);
      FUN_1084c948(param_2,2,param_3);
      return;
    }
  }
  return;
}

