/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bd460(undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4)

{
  byte bVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  byte bVar8;
  uint uVar9;
  byte bVar10;
  uint uVar11;
  bool bVar12;
  undefined1 local_3c [8];
  undefined4 uStack_34;
  undefined4 local_30;
  int local_2c;
  undefined4 uStack_28;
  
  uStack_34 = param_1;
  local_30 = param_2;
  local_2c = param_3;
  uStack_28 = param_4;
  iVar3 = FUN_1086b91c(2);
  if (iVar3 == local_2c) {
    uVar4 = FUN_1084ac1c(local_30,local_2c,local_3c);
    bVar10 = 0;
    uVar9 = 0;
    iVar3 = FUN_1084c7d0(local_30,0x14);
    uVar11 = 0;
    if (uVar4 != 0) {
      do {
        iVar5 = FUN_108544a0(local_30,local_3c[uVar11]);
        bVar1 = *(byte *)(iVar5 + 0x152);
        bVar8 = 0;
        if (bVar1 != 0) {
          do {
            iVar6 = FUN_10855c68(iVar5,bVar8);
            iVar7 = thunk_FUN_0039ddc0();
            if (iVar7 == 0) {
              bVar2 = 1;
            }
            else {
              bVar2 = thunk_FUN_0039e044(iVar6);
              if (bVar2 == 1) {
                iVar7 = thunk_FUN_0039dcf0(iVar6);
                if (iVar7 == 9) {
                  bVar2 = 0x96;
                }
                else if ((iVar6 == 0x44 || iVar6 == 0xf3) || (iVar6 == 0x170)) {
                  bVar2 = 0x78;
                }
                else {
                  bVar2 = 0x50;
                }
              }
            }
            bVar12 = bVar10 <= bVar2;
            if (bVar10 < bVar2) {
              uVar9 = 0;
              bVar10 = bVar2;
            }
            if (bVar12) {
              iVar7 = uVar9 * 4;
              *(undefined1 *)(iVar3 + uVar9 * 4) = *(undefined1 *)(iVar5 + 0x19);
              uVar9 = uVar9 + 1 & 0xff;
              *(short *)(iVar3 + iVar7 + 2) = (short)iVar6;
            }
            bVar8 = bVar8 + 1;
          } while (bVar8 < bVar1);
        }
        uVar11 = uVar11 + 1 & 0xff;
      } while (uVar11 < uVar4);
      if (uVar9 != 0) {
        uVar4 = FUN_1084c7bc(uVar9);
        FUN_1084c948(local_30,1,local_2c);
        iVar5 = FUN_10869f7c(local_30,3,local_2c);
        FUN_1084d2d4(iVar5 + 4,2,0x1b1);
        FUN_1084d29c(iVar5 + 4,*(undefined1 *)(iVar3 + (uVar4 & 0xff) * 4));
        FUN_1084d29c(iVar5 + 4,*(undefined2 *)(iVar3 + (uVar4 & 0xff) * 4 + 2));
        FUN_10867b48(local_30,iVar5);
        FUN_1084c948(local_30,2,local_2c);
      }
    }
  }
  return;
}

