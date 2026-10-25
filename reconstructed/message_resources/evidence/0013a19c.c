/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined2 message_resource_get(int param_1,uint param_2,int param_3,undefined4 param_4)

{
  ushort uVar1;
  uint uVar2;
  int *piVar3;
  ushort *puVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  undefined4 uVar8;
  
  puVar4 = *(ushort **)(param_1 + 0x18);
  if (param_2 < puVar4[1]) {
    uVar2 = DAT_005d113c;
    if (*puVar4 <= DAT_005d113c) {
      uVar2 = 0;
    }
    uVar2 = uVar2 & 0xff;
    if ((*(uint *)(param_1 + 0x14) != uVar2) &&
       (*(uint *)(param_1 + 0x14) = uVar2, *(char *)(param_1 + 0xc) != '\0')) {
      iVar5 = *(int *)(param_1 + 8);
      iVar7 = *(int *)(puVar4 + uVar2 * 2 + 6);
      uVar8 = *(undefined4 *)(param_1 + 0x10);
      if (iVar5 == 0) {
        *(int *)(param_1 + 0x1c) = (int)puVar4 + iVar7;
      }
      else {
        FUN_001381c4(iVar5,uVar8,iVar7,param_4,param_4);
        FUN_00127b6c(iVar5,4,*(undefined4 *)(param_1 + 0x1c));
        FUN_001381c4(iVar5,uVar8,iVar7);
        FUN_00127b6c(iVar5,**(undefined4 **)(param_1 + 0x1c));
      }
    }
    iVar5 = param_2 * 8 + 4;
    if (*(char *)(param_1 + 0xc) == '\0') {
      FUN_001381c4(*(undefined4 *)(param_1 + 8),*(undefined4 *)(param_1 + 0x10),
                   *(int *)(*(int *)(param_1 + 0x18) + *(int *)(param_1 + 0x14) * 4 + 0xc) + iVar5);
      FUN_00127b6c(*(undefined4 *)(param_1 + 8),8,param_1 + 0x20);
      piVar6 = (int *)(param_1 + 0x20);
    }
    else {
      piVar6 = (int *)(*(int *)(param_1 + 0x1c) + iVar5);
    }
    if (*(char *)(param_1 + 0xc) == '\0') {
      uVar1 = *(ushort *)(piVar6 + 1);
      piVar3 = *(int **)(param_1 + 4);
      iVar5 = (**(code **)(*piVar3 + 0x1c))();
      iVar5 = FUN_0012b0c8(piVar3,(uint)uVar1 << 1,-iVar5,1);
      if (iVar5 != 0) {
        FUN_001381c4(*(undefined4 *)(param_1 + 8),*(undefined4 *)(param_1 + 0x10),
                     *piVar6 + *(int *)(*(int *)(param_1 + 0x18) +
                                       *(int *)(param_1 + 0x14) * 4 + 0xc));
        FUN_00127b6c(*(undefined4 *)(param_1 + 8),(uint)uVar1 << 1,iVar5);
        message_decode(param_1,iVar5,(short)piVar6[1],param_2,param_3);
        FUN_0013a50c(iVar5);
      }
    }
    else {
      message_decode(param_1,*piVar6 + *(int *)(param_1 + 0x1c),(short)piVar6[1],param_2,param_3);
    }
    return *(undefined2 *)((int)piVar6 + 6);
  }
  if (*(char *)(param_3 + 0xc) != '\0') {
    **(undefined2 **)(param_3 + 4) = 0;
    *(undefined2 *)(param_3 + 10) = 0;
  }
  return 0;
}

