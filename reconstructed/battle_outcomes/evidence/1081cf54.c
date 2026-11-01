/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081cf54(int param_1,uint *param_2)

{
  undefined1 uVar1;
  int iVar2;
  uint uVar3;
  short sVar4;
  undefined4 *puVar5;
  int *unaff_r8;
  bool bVar6;
  int local_20;
  undefined2 local_1c;
  
  iVar2 = FUN_1086c2cc(param_1 + 0x120c,(char)param_2[1]);
  if (iVar2 != 10) {
    uVar3 = param_2[1];
    iVar2 = *(int *)(param_1 + 8);
    if (0x17 < (byte)uVar3) {
      FUN_10800394(0,0);
    }
    iVar2 = iVar2 + (uint)(byte)uVar3 * 4;
    if (*(int *)(iVar2 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    puVar5 = *(undefined4 **)(iVar2 + 0x84);
    uVar3 = FUN_10874420(puVar5,0xd);
    bVar6 = uVar3 == 0;
    if (bVar6) {
      uVar3 = (uint)*(byte *)((int)param_2 + 5);
    }
    if (!bVar6 || uVar3 != 0) {
      sVar4 = 0x1f;
      iVar2 = 0;
      bVar6 = (*param_2 & 0x1fff) >> 8 != 0x1f;
      if (bVar6) {
        sVar4 = *(short *)((int)param_2 + 6);
      }
      if (bVar6 && sVar4 != 0) {
        unaff_r8 = *(int **)(param_1 + 8);
        uVar3 = (*param_2 & 0x1fff) >> 8;
        if (0x17 < uVar3) {
          FUN_10800394(0,0);
        }
        if (*(int *)((int)unaff_r8 + uVar3 * 4 + 0x84) == 0) {
          FUN_10800394(0,0);
        }
        iVar2 = *(int *)((int)unaff_r8 + uVar3 * 4 + 0x84);
      }
      local_1c = *(undefined2 *)((int)param_2 + 6);
      local_20 = iVar2;
      FUN_1086bbe8(param_1,param_2 + 2);
      uVar3 = *param_2;
      if (iVar2 != 0) {
        unaff_r8 = &local_20;
      }
      uVar1 = *(undefined1 *)((int)puVar5 + 0x19);
      if (iVar2 == 0) {
        unaff_r8 = (int *)0x0;
      }
      FUN_1086bb84();
      FUN_1086bac0(4,uVar1);
      FUN_1086bac0(3,(uVar3 & 0x1fff) >> 8);
      FUN_1086b9a4(param_1,0xb7);
      FUN_1086b880();
      FUN_10874420(puVar5,0xd);
      uVar1 = *(undefined1 *)((int)puVar5 + 0x19);
      *(undefined2 *)(puVar5 + 4) = 0;
      thunk_FUN_0039cbe0(*puVar5);
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),4,uVar1);
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x4b,uVar1,0);
      FUN_108646c4(param_1,puVar5,0,unaff_r8);
      return 1;
    }
  }
  return 0;
}

