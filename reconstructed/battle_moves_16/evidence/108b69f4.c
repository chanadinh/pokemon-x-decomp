/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b69f4(undefined4 param_1,undefined4 param_2,int param_3)

{
  byte bVar1;
  undefined2 uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  undefined4 unaff_r11;
  byte local_30 [8];
  undefined4 local_28;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 == param_3) {
    bVar1 = FUN_1086b91c(4);
    uVar4 = FUN_1084b590(param_2,bVar1);
    if (uVar4 != 10) {
      uVar4 = FUN_1084bc20(param_2,uVar4 | 0x900,local_30);
      if (uVar4 != 0) {
        unaff_r11 = 0x457;
      }
      uVar5 = 0;
      if (uVar4 != 0) {
        do {
          if (local_30[uVar5] != bVar1) {
            local_28 = FUN_108544a0(param_2);
            iVar3 = FUN_10869f7c(param_2,6,param_3);
            *(byte *)(iVar3 + 6) = local_30[uVar5];
            uVar2 = FUN_1084d2f4(local_28,0x10);
            *(undefined2 *)(iVar3 + 4) = uVar2;
            *(byte *)(iVar3 + 7) = *(byte *)(iVar3 + 7) | 1;
            FUN_1084d2d4(iVar3 + 0xc,2,unaff_r11);
            FUN_1084d29c(iVar3 + 0xc,*(undefined1 *)(iVar3 + 6));
            FUN_10867b48(param_2,iVar3);
          }
          uVar5 = uVar5 + 1 & 0xff;
        } while (uVar5 < uVar4);
      }
    }
  }
  return;
}

