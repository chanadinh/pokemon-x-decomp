/* Unreviewed Ghidra pseudocode; types require assembly review. */

void _static_::anon_segment_0_3a7bc(int param_1)

{
  undefined1 uVar1;
  byte bVar2;
  undefined2 uVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  
  uVar3 = FUN_00152f54(*(undefined4 *)(param_1 + 0xc));
  FUN_0014f930(*(undefined4 *)(param_1 + 0xc),uVar3);
  FUN_0014fcb4(*(undefined4 *)(param_1 + 0xc),0);
  bVar2 = 0;
  do {
    uVar4 = FUN_00153014(*(undefined4 *)(param_1 + 0xc),bVar2);
    uVar1 = FUN_00152e50(*(undefined4 *)(param_1 + 0xc),bVar2);
    iVar5 = anon_segment_0_50220(uVar4,uVar1);
    iVar6 = FUN_00153014(*(undefined4 *)(param_1 + 0xc),bVar2);
    if (iVar6 != 0) {
      uVar4 = FUN_00153014(*(undefined4 *)(param_1 + 0xc),bVar2);
      uVar1 = FUN_00152e50(*(undefined4 *)(param_1 + 0xc),bVar2);
      uVar7 = anon_segment_0_50220(uVar4,uVar1);
      iVar6 = FUN_00152efc(*(undefined4 *)(param_1 + 0xc),bVar2);
      if ((uint)(iVar6 + iVar5) <= uVar7) {
        uVar7 = iVar6 + iVar5;
      }
      uVar4 = FUN_00153014(*(undefined4 *)(param_1 + 0xc),bVar2);
      uVar1 = FUN_00152e50(*(undefined4 *)(param_1 + 0xc),bVar2);
      uVar8 = anon_segment_0_50220(uVar4,uVar1);
      if ((uVar7 & 0xff) <= uVar8) {
        uVar8 = uVar7 & 0xff;
      }
      FUN_0014f9d8(*(undefined4 *)(param_1 + 0xc),bVar2,uVar8 & 0xff);
    }
    bVar2 = bVar2 + 1;
  } while (bVar2 < 4);
  return;
}

