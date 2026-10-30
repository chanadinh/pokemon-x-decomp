/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_0012ad74(int param_1,undefined4 param_2,int *param_3)

{
  uint uVar1;
  undefined4 uVar2;
  uint *puVar3;
  int iVar4;
  int iVar5;
  uint local_30;
  uint uStack_2c;
  uint uStack_28;
  
  iVar5 = *(int *)(param_1 + 8);
  if (iVar5 == 0) {
    do {
      do {
      } while (DAT_005d1144 == (code *)0x0);
      (*DAT_005d1144)(&DAT_00563574);
    } while( true );
  }
  *(undefined2 *)(param_1 + 0x24) = 0;
  *(undefined4 *)(param_1 + 0x20) = 0;
  *(undefined2 *)(param_1 + 0x26) = 0;
  FUN_001381c4(iVar5,param_2,0);
  FUN_00127b6c(iVar5,0xc,&local_30);
  uVar1 = local_30 & 0xffff;
  uVar2 = (**(code **)(*param_3 + 0x1c))(param_3);
  puVar3 = (uint *)FUN_0012b0c8(param_3,uVar1 * 4 + 0xc,uVar2,1);
  if (puVar3 != (uint *)0x0) {
    *puVar3 = local_30;
    puVar3[1] = uStack_2c;
    puVar3[2] = uStack_28;
    FUN_00127b6c(iVar5,(uint)(ushort)*puVar3 << 2,puVar3 + 3);
  }
  *(uint **)(param_1 + 0x18) = puVar3;
  *(undefined4 *)(param_1 + 0x10) = param_2;
  uVar1 = DAT_005d113c;
  if ((ushort)*puVar3 <= DAT_005d113c) {
    uVar1 = 0;
  }
  *(uint *)(param_1 + 0x14) = uVar1;
  if (*(char *)(param_1 + 0xc) == '\0') {
    *(undefined4 *)(param_1 + 0x1c) = 0;
  }
  else {
    uVar1 = puVar3[1];
    uVar2 = (**(code **)(*param_3 + 0x1c))(param_3);
    uVar2 = FUN_0012b0c8(param_3,uVar1,uVar2,1);
    *(undefined4 *)(param_1 + 0x1c) = uVar2;
    iVar5 = *(int *)(param_1 + 8);
    uVar2 = *(undefined4 *)(param_1 + 0x10);
    iVar4 = *(int *)(*(int *)(param_1 + 0x18) + *(int *)(param_1 + 0x14) * 4 + 0xc);
    if (iVar5 != 0) {
      FUN_001381c4(iVar5,uVar2,iVar4);
      FUN_00127b6c(iVar5,4,*(undefined4 *)(param_1 + 0x1c));
      FUN_001381c4(iVar5,uVar2,iVar4);
      FUN_00127b6c(iVar5,**(undefined4 **)(param_1 + 0x1c));
      return;
    }
    *(int *)(param_1 + 0x1c) = *(int *)(param_1 + 0x18) + iVar4;
  }
  return;
}

