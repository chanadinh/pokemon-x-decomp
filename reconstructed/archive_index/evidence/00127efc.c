/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_00127efc(int param_1,int *param_2)

{
  undefined2 uVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  undefined4 *unaff_r10;
  undefined4 local_2c [2];
  
  iVar2 = FUN_00123a6c(param_1 + 0x6c);
  if (iVar2 != 0) {
    piVar3 = (int *)(**(code **)(**(int **)(param_1 + 4) + 0x34))();
    piVar4 = (int *)FUN_00128134(param_1,*(undefined4 *)(param_1 + 4));
    uVar5 = (**(code **)(*piVar4 + 0x20))(piVar4);
    if (piVar4 != (int *)0x0) {
      (**(code **)(*piVar4 + 4))(piVar4);
    }
    local_2c[0] = 0;
    FUN_00127c5c(param_1,uVar5,4,local_2c,0);
    uVar5 = local_2c[0];
    uVar6 = (**(code **)(*piVar3 + 0x1c))(piVar3);
    uVar6 = FUN_0012b0c8(piVar3,uVar5,uVar6,1);
    FUN_00127c5c(param_1,0,uVar5,uVar6,0);
    piVar3 = (int *)FUN_00128134(param_1,piVar3);
    (**(code **)(*piVar3 + 8))(piVar3,uVar6);
    if (*(int *)(param_1 + 0x3c) != 0) {
      FUN_002ff520();
      *(undefined4 *)(param_1 + 0x3c) = 0;
    }
    uVar7 = (**(code **)(*piVar3 + 0xc))(piVar3);
    uVar5 = (**(code **)(*param_2 + 0x1c))(param_2);
    iVar2 = FUN_0012b0c8(param_2,uVar7 * 0xc,uVar5,1);
    if (uVar7 != 0) {
      unaff_r10 = &DAT_005d10d8;
    }
    uVar8 = 0;
    if (uVar7 != 0) {
      do {
        uVar5 = (**(code **)(*piVar3 + 0x10))(piVar3,uVar8,unaff_r10[0x13]);
        *(undefined4 *)(iVar2 + uVar8 * 0xc) = uVar5;
        uVar5 = (**(code **)(*piVar3 + 0x14))(piVar3,uVar8,unaff_r10[0x13]);
        iVar9 = iVar2 + uVar8 * 0xc;
        *(undefined4 *)(iVar9 + 4) = uVar5;
        uVar5 = (**(code **)(*piVar3 + 0x1c))(piVar3,uVar8,unaff_r10[0x13]);
        uVar8 = uVar8 + 1;
        *(undefined4 *)(iVar9 + 8) = uVar5;
      } while (uVar8 < uVar7);
    }
    *(int *)(param_1 + 0x3c) = iVar2;
    uVar1 = (**(code **)(*piVar3 + 0xc))(piVar3);
    *(undefined2 *)(param_1 + 0x34) = uVar1;
    uVar5 = (**(code **)(*piVar3 + 0x18))(piVar3);
    *(undefined4 *)(param_1 + 0x38) = uVar5;
    if (piVar3 != (int *)0x0) {
      (**(code **)(*piVar3 + 4))(piVar3);
    }
    FUN_002ff520(uVar6);
    FUN_0016f1e8(param_1 + 0x4c);
    return;
  }
  return;
}

