/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10820c9c(int param_1,uint *param_2,undefined4 param_3,undefined4 param_4)

{
  undefined2 uVar1;
  uint uVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  uVar2 = param_2[1];
  iVar5 = *(int *)(param_1 + 8);
  if (0x17 < (byte)uVar2) {
    FUN_10800394(0,0,0,param_4,param_4);
  }
  iVar5 = iVar5 + (uint)(byte)uVar2 * 4;
  if (*(int *)(iVar5 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar5 = *(int *)(iVar5 + 0x84);
  iVar7 = *(int *)(param_1 + 8);
  uVar2 = (*param_2 & 0x1fff) >> 8;
  if (0x17 < uVar2) {
    FUN_10800394(0,0);
  }
  iVar7 = iVar7 + uVar2 * 4;
  if (*(int *)(iVar7 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar6 = *(int *)(iVar7 + 0x84);
  uVar2 = (uint)*(ushort *)(iVar5 + 0x12);
  uVar1 = *(undefined2 *)(iVar6 + 0x12);
  puVar3 = (undefined4 *)(param_1 + 0x437c);
  uVar4 = FUN_1086b284();
  FUN_1086bb84();
  FUN_1086bac0(2,*(undefined1 *)(iVar5 + 0x19));
  FUN_1086bac0(0x2e,uVar1);
  FUN_1086b9fc(0x42,0);
  FUN_1086b9a4(param_1,0xa6);
  iVar7 = FUN_1086b91c(0x42);
  FUN_1086b880();
  *puVar3 = uVar4;
  if (iVar7 == 0) {
    if ((*param_2 & 0x800000) != 0) {
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,*(undefined1 *)(iVar6 + 0x19));
    }
    FUN_1086bbe8(param_1,param_2 + 2);
    FUN_1086bbe8(param_1,param_2 + 0xb);
    FUN_1086bbe8(param_1,param_2 + 0x14);
    if ((*param_2 & 0x800000) != 0) {
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,*(undefined1 *)(iVar6 + 0x19));
    }
    FUN_1086b140(param_1,iVar6,uVar2,0);
    FUN_1086b140(param_1,iVar5,uVar1,0);
    if (*(short *)(iVar6 + 0x12) != 0) {
      uVar4 = FUN_1086b284(puVar3);
      FUN_1086bb84();
      FUN_1086bac0(2,*(undefined1 *)(iVar6 + 0x19));
      FUN_1086bac0(0x2f,7);
      FUN_1086b9a4(param_1,0x9c);
      FUN_1086b880();
      *puVar3 = uVar4;
    }
    if (*(short *)(iVar5 + 0x12) != 0) {
      uVar4 = FUN_1086b284(puVar3);
      FUN_1086bb84();
      FUN_1086bac0(2,*(undefined1 *)(iVar5 + 0x19));
      FUN_1086bac0(0x2f,7);
      FUN_1086b9a4(param_1,0x9c);
      FUN_1086b880();
      *puVar3 = uVar4;
    }
    return 1;
  }
  uVar4 = FUN_1086b284(puVar3);
  FUN_1086bb84();
  FUN_1086bac0(2,*(undefined1 *)(iVar5 + 0x19));
  FUN_1086b9a4(param_1,0xa7);
  FUN_1086b880();
  *puVar3 = uVar4;
  return 0;
}

