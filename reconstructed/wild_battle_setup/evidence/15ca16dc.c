/* Unreviewed Ghidra pseudocode; types require assembly review. */

void DllField::anon_segment_0_a155c(int param_1,int param_2,undefined4 param_3)

{
  ushort uVar1;
  undefined2 *puVar2;
  undefined4 uVar3;
  undefined2 local_c0;
  undefined2 local_be;
  undefined1 local_bc;
  undefined1 local_bb;
  byte local_ba;
  byte local_b9;
  byte local_b0;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined1 local_34;
  undefined1 local_33;
  undefined1 local_32;
  undefined1 local_31;
  
  uVar3 = 0;
  FUN_15c75610(&local_c0);
  if (*(int *)(param_1 + 0x48) != 0) {
    uVar3 = _static_::anon_segment_0_3a4000
                      (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x48) + 0x158) + 0xc));
  }
  FUN_15c75434(&local_c0,param_1,1,uVar3,0xff,*(undefined1 *)(*(int *)(param_1 + 0x2c) + 0x34));
  local_50 = 1;
  local_4c = 1;
  local_54 = 1;
  puVar2 = (undefined2 *)(DAT_15ca17d8 + param_2 * 0xc);
  local_34 = *(undefined1 *)(puVar2 + 4);
  local_33 = *(undefined1 *)((int)puVar2 + 9);
  local_32 = *(undefined1 *)(puVar2 + 5);
  uVar1 = puVar2[3];
  local_31 = (undefined1)(uVar1 >> 7);
  local_c0 = *puVar2;
  local_ba = (byte)uVar1 & 3;
  local_be = puVar2[2];
  local_bc = *(undefined1 *)((int)puVar2 + 3);
  local_bb = *(undefined1 *)(puVar2 + 1);
  local_b0 = (byte)(((uint)uVar1 << 0x1c) >> 0x1e);
  local_b9 = (byte)(((uint)uVar1 << 0x19) >> 0x1d);
  _static_::anon_segment_0_2bcb44(*(undefined4 *)(param_1 + 0x18),param_1,&local_c0,param_3);
  return;
}

