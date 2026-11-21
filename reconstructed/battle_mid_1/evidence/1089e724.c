/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined2 FUN_1089e724(int param_1,uint param_2)

{
  int iVar1;
  ushort uVar2;
  int iVar3;
  undefined2 uVar4;
  ushort *puVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  ushort *puVar9;
  
  iVar3 = DAT_108fca58;
  uVar7 = 0;
  puVar5 = (ushort *)&DAT_00000001;
  do {
    if (param_2 != 0) {
      uVar6 = param_2 & 1;
      uVar8 = 0;
      if (uVar6 != 0) {
        do {
          if ((ushort *)(uint)*(ushort *)(param_1 + uVar8 * 2) == puVar5) goto LAB_1089e7ac;
          uVar8 = uVar8 + 1;
        } while (uVar8 < uVar6);
      }
      for (; uVar6 < param_2; uVar6 = uVar6 + 2) {
        puVar9 = (ushort *)(param_1 + uVar6 * 2);
        uVar2 = *puVar9;
        if ((ushort *)(uint)uVar2 != puVar5) {
          puVar9 = (ushort *)(uint)puVar9[1];
        }
        if ((ushort *)(uint)uVar2 == puVar5 || puVar9 == puVar5) goto LAB_1089e7ac;
      }
    }
    iVar1 = uVar7 * 2;
    uVar7 = uVar7 + 1 & 0xffff;
    *(short *)(iVar3 + iVar1) = (short)puVar5;
LAB_1089e7ac:
    puVar5 = (ushort *)((uint)((int)puVar5 + 1) & 0xffff);
    if ((ushort *)0x269 < puVar5) {
      if (uVar7 == 0) {
        uVar4 = 0;
      }
      else {
        uVar7 = thunk_FUN_00174bb4(DAT_108fca54,uVar7);
        uVar4 = *(undefined2 *)(DAT_108fca58 + (uVar7 & 0xffff) * 2);
      }
      return uVar4;
    }
  } while( true );
}

