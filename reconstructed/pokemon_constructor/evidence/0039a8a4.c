/* Unreviewed Ghidra pseudocode; types require assembly review. */

int FUN_0039a8a4(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 local_60;
  undefined4 uStack_5c;
  undefined4 uStack_58;
  undefined4 uStack_54;
  undefined4 local_50;
  undefined4 uStack_4c;
  undefined2 local_48;
  undefined1 local_46;
  undefined2 local_44;
  undefined2 local_42;
  undefined2 local_40;
  undefined1 local_3e;
  undefined1 local_3d;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined1 local_2c;
  
  local_60 = 0xffffffff;
  uStack_5c = 0xffffffff;
  uStack_58 = 0xffffffff;
  uStack_54 = 3;
  local_50 = 0xffffffff;
  uStack_4c = 0xffffffff;
  local_40 = 0xffff;
  local_44 = 1;
  local_3e = 0xff;
  local_48 = (undefined2)DAT_0039a990;
  local_3d = 1;
  local_46 = 0;
  iVar2 = 0;
  local_42 = 0xff;
  local_3c = 0;
  local_38 = 0;
  local_34 = 0;
  local_2c = 0;
  uVar1 = 0;
  local_30 = 0xffff;
  do {
    iVar3 = iVar2 + 1;
    uVar1 = uVar1 + 2;
    *(undefined2 *)((int)&local_3c + iVar2 * 2) = 0xffff;
    iVar2 = iVar2 + 2;
    *(undefined2 *)((int)&local_3c + iVar3 * 2) = 0xffff;
  } while (uVar1 < 6);
  FUN_0014f324(&local_60,param_2);
  iVar2 = _static_::anon_segment_0_22920(0xe8,param_1);
  if (iVar2 != 0) {
    _static_::anon_segment_0_201a9c(iVar2,0xe8);
  }
  _static_::anon_segment_0_201a9c(iVar2,0xe8);
  FUN_0014f6b8(iVar2,&local_60);
  return iVar2;
}

