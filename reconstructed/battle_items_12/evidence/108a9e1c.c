/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4
FUN_108a9e1c(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,int param_5)

{
  byte bVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  
  iVar2 = FUN_108544a0(param_2,param_3);
  uVar3 = FUN_108544a0(param_2,param_3);
  uVar4 = FUN_1084c278();
  uVar5 = FUN_1084c224(uVar3,uVar4);
  if ((uVar5 != 4) && (iVar6 = FUN_108520e4(uVar3,uVar5), iVar6 == 0)) {
LAB_108a9f08:
    iVar6 = FUN_10869f7c(param_2,8,param_3);
    *(char *)(iVar6 + 5) = (char)param_3;
    *(char *)(iVar6 + 6) = (char)uVar5;
    *(undefined1 *)(iVar6 + 4) = 10;
    *(byte *)(iVar6 + 7) = *(byte *)(iVar6 + 7) | 1;
    FUN_1084d2d4(iVar6 + 8,2,0x395);
    FUN_1084d29c(iVar6 + 8,param_3);
    FUN_1084d29c(iVar6 + 8,*(undefined2 *)(param_1 + 0x38));
    uVar3 = FUN_10855c68(iVar2,uVar5);
    FUN_1084d29c(iVar6 + 8,uVar3);
    FUN_10867b48(param_2,iVar6);
    return 1;
  }
  iVar6 = FUN_108544a0(param_2,param_3);
  bVar1 = *(byte *)(iVar6 + 0x152);
  uVar5 = 0;
  if (bVar1 != 0) {
    do {
      iVar7 = FUN_108520e4(iVar6,uVar5 & 0xff);
      if (iVar7 == 0) {
        uVar5 = uVar5 & 0xff;
        if (uVar5 != 4) goto LAB_108a9f08;
        break;
      }
      uVar5 = uVar5 + 1;
    } while (uVar5 < bVar1);
  }
  if (param_5 != 0) {
    bVar1 = *(byte *)(iVar2 + 0x152);
    uVar5 = 0;
    if (bVar1 != 0) {
      do {
        iVar6 = FUN_1084d184(iVar2,uVar5 & 0xff);
        if (iVar6 != 0) {
          uVar5 = uVar5 & 0xff;
          if (uVar5 == 4) {
            return 0;
          }
          goto LAB_108a9f08;
        }
        uVar5 = uVar5 + 1;
      } while (uVar5 < bVar1);
    }
  }
  return 0;
}

