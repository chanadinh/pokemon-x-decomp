/* Unreviewed Ghidra pseudocode; types require assembly review. */

void _static_::anon_segment_0_50064(int param_1,int param_2)

{
  byte bVar1;
  undefined1 uVar2;
  undefined1 uVar3;
  int iVar4;
  undefined4 uVar5;
  char cVar6;
  
  bVar1 = 0;
  do {
    iVar4 = FUN_00153014(*(undefined4 *)(param_1 + 0xc),bVar1);
    if (iVar4 == param_2) {
      return;
    }
    bVar1 = bVar1 + 1;
  } while (bVar1 < 4);
  bVar1 = 0;
  do {
    iVar4 = FUN_00153014(*(undefined4 *)(param_1 + 0xc),bVar1);
    if (iVar4 == 0) {
      if (3 < bVar1) {
        anon_segment_0_4f220(0,0);
        return;
      }
      uVar2 = anon_segment_0_68bbc(param_2,5);
      FUN_00168824(*(undefined4 *)(param_1 + 0xc),bVar1,param_2);
      FUN_001684f4(*(undefined4 *)(param_1 + 0xc),bVar1,0);
      FUN_0014f9d8(*(undefined4 *)(param_1 + 0xc),bVar1,uVar2);
      return;
    }
    bVar1 = bVar1 + 1;
  } while (bVar1 < 4);
  bVar1 = 1;
  do {
    uVar5 = FUN_00153014(*(undefined4 *)(param_1 + 0xc),bVar1);
    uVar2 = FUN_00152efc(*(undefined4 *)(param_1 + 0xc),bVar1);
    uVar3 = FUN_00152e50(*(undefined4 *)(param_1 + 0xc),bVar1);
    cVar6 = bVar1 - 1;
    FUN_00168824(*(undefined4 *)(param_1 + 0xc),cVar6,uVar5);
    FUN_0014f9d8(*(undefined4 *)(param_1 + 0xc),cVar6,uVar2);
    FUN_001684f4(*(undefined4 *)(param_1 + 0xc),cVar6,uVar3);
    bVar1 = bVar1 + 1;
  } while (bVar1 < 4);
  uVar2 = anon_segment_0_68bbc(param_2,5);
  FUN_00168824(*(undefined4 *)(param_1 + 0xc),3,param_2);
  FUN_001684f4(*(undefined4 *)(param_1 + 0xc),3,0);
  FUN_0014f9d8(*(undefined4 *)(param_1 + 0xc),3,uVar2);
  return;
}

