/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b87e4(undefined4 param_1,undefined4 param_2,int param_3,undefined1 *param_4)

{
  byte bVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  int local_40 [4];
  undefined4 uStack_30;
  int local_2c;
  undefined1 *puStack_28;
  
  local_40[3] = param_1;
  uStack_30 = param_2;
  local_2c = param_3;
  puStack_28 = param_4;
  iVar3 = FUN_1086b91c(3);
  if (iVar3 != local_2c) {
    return;
  }
  local_40[0] = FUN_1084b950(param_2);
  local_40[1] = FUN_1082001c(param_2,local_2c);
  if (local_40[1] == 0) {
    uVar4 = 1;
  }
  else {
    uVar4 = 2;
  }
  iVar3 = 0;
  uVar12 = 0;
  if (uVar4 != 0) {
    do {
      bVar1 = *(byte *)(local_40[uVar12] + 0x18);
      uVar11 = 0;
      if (bVar1 != 0) {
        do {
          iVar5 = FUN_1085daf8(local_40[uVar12],uVar11 & 0xff);
          bVar2 = *(byte *)(iVar5 + 0x19);
          uVar6 = (uint)bVar2;
          iVar5 = FUN_1084cb78(param_2,uVar6);
          if (iVar5 == 10) {
LAB_108b88d0:
            iVar5 = FUN_108544a0(param_2,uVar6);
            if ((iVar5 != 0) && (iVar5 = FUN_1086d8ac(), iVar5 != 0)) {
              param_4[iVar3] = bVar2;
              iVar3 = iVar3 + 1;
            }
          }
          else {
            uVar7 = FUN_1086b91c(5);
            uVar10 = 0;
            if (uVar7 != 0) {
              do {
                uVar8 = FUN_1086b91c(uVar10 + 6 & 0xff);
                if (uVar8 == uVar6) goto LAB_108b88d0;
                uVar10 = uVar10 + 1;
              } while (uVar10 < uVar7);
            }
          }
          uVar11 = uVar11 + 1;
        } while (uVar11 < bVar1);
      }
      uVar12 = uVar12 + 1;
    } while (uVar12 < uVar4);
    if (iVar3 != 0) {
      iVar5 = FUN_10869f7c(param_2,10,local_2c);
      *(char *)(iVar5 + 0x11) = (char)iVar3;
      iVar9 = 0;
      do {
        iVar13 = iVar5 + iVar9;
        iVar3 = iVar3 + -1;
        iVar9 = iVar9 + 1;
        *(undefined1 *)(iVar13 + 5) = *param_4;
        param_4 = param_4 + 1;
      } while (iVar3 != 0);
      *(undefined1 *)(iVar5 + 4) = 0x26;
      FUN_10867b48(param_2);
      return;
    }
  }
  FUN_108658d4(0x53,1);
  return;
}

