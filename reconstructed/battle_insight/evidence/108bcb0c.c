/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bcb0c(undefined4 param_1,undefined4 param_2,uint param_3,int *param_4)

{
  undefined1 uVar1;
  ushort uVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  uint *puVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  undefined1 local_48 [8];
  undefined1 local_40 [12];
  undefined4 uStack_34;
  undefined4 local_30;
  uint uStack_2c;
  int *piStack_28;
  
  uStack_34 = param_1;
  local_30 = param_2;
  uStack_2c = param_3;
  piStack_28 = param_4;
  uVar3 = FUN_1086b91c(2);
  uVar3 = uVar3 & 0xff;
  if (uVar3 == param_3) {
    uVar3 = FUN_1084cb78(local_30);
    uVar3 = FUN_1084bc20(local_30,uVar3 | 0x100,local_48);
    uVar9 = 0;
    uVar8 = 0;
    if (uVar3 != 0) {
      do {
        uVar4 = FUN_108544a0(local_30,local_48[uVar8]);
        uVar1 = FUN_10874420(uVar4,0x10);
        iVar5 = FUN_108a4854(uVar1);
        if (iVar5 == 0) {
          local_40[uVar9] = local_48[uVar8];
          uVar9 = uVar9 + 1 & 0xff;
        }
        uVar8 = uVar8 + 1 & 0xff;
      } while (uVar8 < uVar3);
      if (uVar9 != 0) {
        if (uVar9 == 1) {
          uVar3 = 0;
        }
        else {
          uVar3 = FUN_1084c7bc();
        }
        iVar5 = FUN_108544a0(local_30,local_40[uVar3 & 0xff]);
        uVar2 = FUN_10874420(iVar5,0x10);
        uVar2 = uVar2 & 0xff;
        uVar3 = (uint)*(byte *)(iVar5 + 0x19);
        goto LAB_108bcbfc;
      }
    }
    *param_4 = 1;
  }
  else {
    iVar5 = FUN_1084c5a8(uVar3,param_3);
    if (iVar5 != 0) {
      return;
    }
    if (*param_4 != 1) {
      return;
    }
    iVar5 = FUN_10873158(local_30,uVar3);
    uVar4 = FUN_1084cb78(local_30,param_3);
    if (iVar5 == 10) {
      return;
    }
    iVar7 = FUN_1084bf14(local_30);
    if ((iVar7 == 2) && (iVar5 = FUN_108673f8(uVar4,iVar5), iVar5 == 0)) {
      return;
    }
    uVar4 = FUN_108544a0(local_30,uVar3);
    uVar2 = FUN_10874420(uVar4,0x10);
    uVar2 = uVar2 & 0xff;
    iVar5 = FUN_108a4854(uVar2);
    if (iVar5 != 0) {
      return;
    }
LAB_108bcbfc:
    if (uVar2 != 0 && param_3 != 0x1f) {
      puVar6 = (uint *)FUN_10869f7c(local_30,0x20,param_3);
      *(char *)((int)puVar6 + 6) = (char)param_3;
      *(ushort *)(puVar6 + 1) = uVar2;
      *(undefined1 *)(puVar6 + 2) = 1;
      *puVar6 = *puVar6 | 0x800000;
      FUN_1084d2d4(puVar6 + 3,2,0x17d);
      FUN_1084d29c(puVar6 + 3,uVar3);
      FUN_1084d29c(puVar6 + 3,(short)puVar6[1]);
      FUN_10867b48(local_30,puVar6);
      return;
    }
  }
  return;
}

