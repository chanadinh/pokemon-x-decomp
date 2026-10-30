/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_00127ccc(int param_1,uint param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  undefined4 local_24;
  
  if (-1 < *(int *)(param_1 + 0x4c)) {
    if (*(ushort *)(param_1 + 0x34) <= param_2) {
      do {
        do {
        } while (DAT_005d1144 == (code *)0x0);
        (*DAT_005d1144)(&DAT_00563574);
      } while( true );
    }
    puVar5 = (undefined4 *)(*(int *)(param_1 + 0x3c) + param_2 * 0xc);
    uVar1 = *puVar5;
    uVar6 = puVar5[1];
    param_3[2] = puVar5[2];
    *param_3 = uVar1;
    param_3[1] = uVar6;
    return;
  }
  piVar2 = (int *)FUN_00128134(param_1,*(undefined4 *)(param_1 + 4));
  uVar1 = (**(code **)(*piVar2 + 0x20))(piVar2);
  if (piVar2 != (int *)0x0) {
    (**(code **)(*piVar2 + 4))(piVar2);
  }
  local_24 = 0;
  FUN_00127c5c(param_1,uVar1,4,&local_24,0);
  uVar1 = local_24;
  piVar2 = *(int **)(param_1 + 4);
  iVar3 = (**(code **)(*piVar2 + 0x1c))(piVar2);
  uVar6 = FUN_0012b0c8(piVar2,uVar1,-iVar3,1);
  FUN_00127c5c(param_1,0,uVar1,uVar6,0);
  piVar2 = (int *)FUN_00128134(param_1,*(undefined4 *)(param_1 + 4));
  (**(code **)(*piVar2 + 8))(piVar2,uVar6);
  uVar4 = (**(code **)(*piVar2 + 0xc))(piVar2);
  if (uVar4 <= param_2) {
    do {
      do {
      } while (DAT_005d1144 == (code *)0x0);
      (*DAT_005d1144)(&DAT_00563574);
    } while( true );
  }
  uVar1 = (**(code **)(*piVar2 + 0x1c))(piVar2,param_2,DAT_005d1124);
  param_3[2] = uVar1;
  uVar1 = (**(code **)(*piVar2 + 0x10))(piVar2,param_2,DAT_005d1124);
  *param_3 = uVar1;
  uVar1 = (**(code **)(*piVar2 + 0x14))(piVar2,param_2,DAT_005d1124);
  param_3[1] = uVar1;
  if (piVar2 != (int *)0x0) {
    (**(code **)(*piVar2 + 4))(piVar2);
  }
  FUN_002ff520(uVar6);
  return;
}

