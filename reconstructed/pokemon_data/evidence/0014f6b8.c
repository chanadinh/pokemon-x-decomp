/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined1 * FUN_0014f6b8(undefined4 param_1,undefined4 *param_2)

{
  int *piVar1;
  undefined4 uVar2;
  undefined1 auStack_20 [20];
  
  FUN_0014ff4c(auStack_20);
  FUN_00168548(auStack_20,param_1,0);
  if (*(short *)(param_2 + 6) == 0) {
    return auStack_20;
  }
  FUN_00168360(auStack_20,*param_2);
  FUN_001686e8(auStack_20,param_2[4]);
  FUN_00168028(auStack_20,param_2[2]);
  FUN_001687f8(auStack_20,*(undefined2 *)(param_2 + 6));
  FUN_00168780(auStack_20,*(undefined1 *)((int)param_2 + 0x1a));
  FUN_0014fd04(auStack_20,*(ushort *)(param_2 + 7) & 0xff);
  FUN_00167ffc(auStack_20,*(undefined2 *)(param_2 + 8));
  FUN_00168740(auStack_20,*(undefined2 *)((int)param_2 + 0x1e));
  FUN_00168054(auStack_20,*(ushort *)(param_2 + 7) & 0xff);
  FUN_00168568(auStack_20,0);
  FUN_0016830c(auStack_20,param_2[0xc] & 0xff);
  piVar1 = DAT_0014f8c8;
  FUN_001687cc(auStack_20,*(undefined1 *)(*DAT_0014f8c8 + 0x68));
  FUN_001682a4(auStack_20,*(undefined1 *)(*piVar1 + 0x69));
  FUN_00168594(auStack_20,*(undefined1 *)(*piVar1 + 0x6a));
  FUN_00168404(auStack_20,*(undefined1 *)(*piVar1 + 0x6b));
  FUN_00168090(auStack_20,*(ushort *)(param_2 + 9) & 0xff);
  FUN_0016810c(auStack_20,*(ushort *)((int)param_2 + 0x26) & 0xff);
  FUN_0016814c(auStack_20,*(ushort *)(param_2 + 10) & 0xff);
  FUN_00168384(auStack_20,*(ushort *)((int)param_2 + 0x2a) & 0xff);
  FUN_001683c4(auStack_20,*(ushort *)(param_2 + 0xb) & 0xff);
  FUN_001680cc(auStack_20,*(ushort *)((int)param_2 + 0x2e) & 0xff);
  uVar2 = FUN_001688c8(*(undefined2 *)(param_2 + 6),*(undefined1 *)((int)param_2 + 0x1a),
                       *(undefined1 *)((int)param_2 + 0x22));
  FUN_00168278(auStack_20,uVar2);
  FUN_00168430(auStack_20,*(char *)((int)param_2 + 0x22) == '\0');
  FUN_00168468(auStack_20,*(char *)((int)param_2 + 0x22) == '\x01');
  FUN_001684a0(auStack_20,*(char *)((int)param_2 + 0x22) == '\x02');
  _static_::anon_segment_0_67db0(*(undefined2 *)(param_2 + 6),*(undefined1 *)((int)param_2 + 0x1a));
  uVar2 = _static_::anon_segment_0_67f98(*(ushort *)(param_2 + 7) & 0xff);
  FUN_00168714(auStack_20,uVar2);
  return auStack_20;
}

