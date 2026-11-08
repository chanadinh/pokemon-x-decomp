/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bf590(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  uint *puVar8;
  uint uVar9;
  
  iVar3 = FUN_1086b91c(2);
  if ((iVar3 == param_3) && (iVar3 = FUN_108544a0(param_2,param_3), *(short *)(iVar3 + 0x12) == 0))
  {
    uVar4 = FUN_1084cb78(param_2,param_3);
    iVar5 = FUN_1084c7d0(param_2,0xe);
    iVar6 = FUN_1084bc20(param_2,uVar4 | 0x200,iVar5);
    *(char *)(iVar5 + 0xc) = (char)iVar6;
    uVar9 = 0;
    uVar4 = 0;
    *(undefined1 *)(iVar5 + 0xd) = 0;
    if (iVar6 != 0) {
      do {
        iVar6 = FUN_108544a0(param_2,*(undefined1 *)(iVar5 + uVar4));
        iVar7 = FUN_1084c818(iVar6,8);
        if ((iVar7 != 0) && (*(short *)(iVar6 + 0x14) != 0)) {
          iVar7 = iVar5 + uVar9;
          uVar9 = uVar9 + 1 & 0xff;
          *(undefined1 *)(iVar7 + 6) = *(undefined1 *)(iVar6 + 0x19);
        }
        uVar4 = uVar4 + 1 & 0xff;
      } while (uVar4 < *(byte *)(iVar5 + 0xc));
      if (uVar9 != 0) {
        uVar4 = FUN_1084c7bc(uVar9);
        uVar1 = *(undefined1 *)((uVar4 & 0xff) + iVar5 + 6);
        iVar5 = FUN_108544a0(param_2,uVar1);
        sVar2 = *(short *)(iVar5 + 0x14);
        if ((sVar2 != 0) &&
           (iVar3 = FUN_1085441c(param_2,*(undefined2 *)(iVar3 + 0xc),sVar2), iVar3 == 0)) {
          puVar8 = (uint *)FUN_10869f7c(param_2,0x21,param_3);
          *(undefined1 *)((int)puVar8 + 9) = uVar1;
          *(char *)((int)puVar8 + 6) = (char)param_3;
          *(undefined1 *)(puVar8 + 2) = 1;
          *(short *)(puVar8 + 1) = sVar2;
          *puVar8 = *puVar8 | 0x800000;
          FUN_1084d2d4(puVar8 + 3,2,0x1ea);
          FUN_1084d29c(puVar8 + 3,param_3);
          FUN_1084d29c(puVar8 + 3,sVar2);
          FUN_10867b48(param_2,puVar8);
          return;
        }
      }
    }
  }
  return;
}

