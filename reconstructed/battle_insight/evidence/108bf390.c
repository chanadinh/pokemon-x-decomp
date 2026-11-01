/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bf390(undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 *puVar8;
  uint uVar9;
  byte bVar10;
  uint uVar11;
  undefined1 local_40 [8];
  undefined4 local_38;
  undefined4 uStack_34;
  undefined4 uStack_30;
  int local_2c;
  undefined4 uStack_28;
  
  uStack_34 = param_1;
  uStack_30 = param_2;
  local_2c = param_3;
  uStack_28 = param_4;
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == local_2c) {
    uVar3 = FUN_1084ac1c(param_2,local_2c,local_40);
    uVar4 = FUN_108544a0(param_2,local_2c);
    uVar11 = 0;
    if (uVar3 != 0) {
      do {
        iVar2 = FUN_108544a0(param_2,local_40[uVar11]);
        local_38 = FUN_10870994(uVar4);
        bVar1 = *(byte *)(iVar2 + 0x152);
        bVar10 = 0;
        if (bVar1 != 0) {
          do {
            iVar5 = FUN_10855c68(iVar2,bVar10);
            iVar6 = thunk_FUN_0039dcf0();
            if (iVar6 == 9) goto LAB_108bf4e8;
            iVar6 = thunk_FUN_0039e058(iVar5);
            if (iVar6 != 0) {
              if (iVar5 == 0xed) {
                puVar8 = (undefined4 *)FUN_108544a0(param_2,*(undefined1 *)(iVar2 + 0x19));
                if (puVar8 == (undefined4 *)0x0) {
                  FUN_10800394(0,0);
                  uVar7 = 0;
                }
                else {
                  uVar7 = thunk_FUN_004a0fdc(*puVar8);
                }
              }
              else {
                uVar7 = thunk_FUN_0039dfc4(iVar5);
              }
              uVar9 = FUN_10870900(uVar7,local_38);
              if (7 < uVar9) goto LAB_108bf4e8;
            }
            bVar10 = bVar10 + 1;
          } while (bVar10 < bVar1);
        }
        uVar11 = uVar11 + 1 & 0xff;
      } while (uVar11 < uVar3);
    }
LAB_108bf4e8:
    if (uVar11 != uVar3) {
      FUN_1084c948(param_2,1,local_2c);
      iVar2 = FUN_10869f7c(param_2,3,local_2c);
      FUN_1084d2d4(iVar2 + 4,2,0x1b4);
      FUN_1084d29c(iVar2 + 4,local_2c);
      FUN_10867b48(param_2,iVar2);
      FUN_1084c948(param_2,2,local_2c);
    }
  }
  return;
}

