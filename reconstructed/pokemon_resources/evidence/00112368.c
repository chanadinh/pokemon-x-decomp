/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_00112368(undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  FUN_0011e884(param_1,*DAT_005d1870);
  FUN_0011e960(param_1,DAT_005d1870[1]);
  FUN_0011ecf8(param_1,DAT_005d1870[2]);
  FUN_0011e9dc(param_1,DAT_005d1870[3]);
  FUN_0011eab0(param_1,DAT_005d1870[4]);
  iVar1 = FUN_00122920(0x10,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0012acd8(iVar1,0x100,param_1);
  }
  DAT_005d1790 = uVar2;
  iVar1 = FUN_00122920(0x28,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0011e264(iVar1,DAT_005d1870[0x11],DAT_005d1870[0x12],param_1,1,5);
  }
  DAT_005d1794 = uVar2;
  iVar1 = FUN_00122920(0x28,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0011e264(iVar1,DAT_005d1870[0x11],DAT_005d1870[0x13],param_1,0,5);
  }
  DAT_005d1798 = uVar2;
  iVar1 = FUN_00122920(0x28,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0011e264(iVar1,DAT_005d1870[0x11],DAT_005d1870[0x14],param_1,0,5);
  }
  DAT_005d179c = uVar2;
  iVar1 = FUN_00122920(0xc,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0012b554(iVar1,param_1);
  }
  DAT_005d17a4 = uVar2;
  iVar1 = FUN_00122920(0xc,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0011eb08(iVar1,param_1,DAT_005d1870[7] & 0xff);
  }
  DAT_005d17a0 = uVar2;
  iVar1 = FUN_00122920(0x19c,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0012b6a4();
  }
  DAT_005d17ac = uVar2;
  iVar1 = FUN_00122920(0xc,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0011ea34(iVar1,param_1,DAT_005d1870[8]);
  }
  DAT_005d17b0 = uVar2;
  iVar1 = FUN_00122920(0xc,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0012b5d4(iVar1,param_1);
  }
  DAT_005d17b4 = uVar2;
  iVar1 = FUN_00122920(0x10,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0011eb84(iVar1,param_1,DAT_005d1870[9]);
  }
  DAT_005d17b8 = uVar2;
  iVar1 = FUN_00122920(0xc,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0012b610(iVar1,param_1);
  }
  DAT_005d17bc = uVar2;
  iVar1 = FUN_00122920(0x10,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0011ec00(iVar1,param_1,DAT_005d1870[10]);
  }
  DAT_005d17c0 = uVar2;
  iVar1 = FUN_00122920(0xc,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0012b650(iVar1,param_1);
  }
  DAT_005d17c8 = uVar2;
  iVar1 = FUN_00122920(0x10,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0011ec7c(iVar1,param_1,DAT_005d1870[0xb]);
  }
  DAT_005d17cc = uVar2;
  iVar1 = FUN_00122920(0xc,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0012b554(iVar1,param_1);
  }
  DAT_005d17a8 = uVar2;
  iVar1 = FUN_00122920(0xc,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0012b650(iVar1,param_1);
  }
  DAT_005d17c4 = uVar2;
  iVar1 = FUN_00122920(0x78,param_1);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_00128288(iVar1,param_1,DAT_005d1870[6],1);
  }
  DAT_005d17d0 = uVar2;
  iVar1 = FUN_00127bd8();
  uVar2 = FUN_0011c3cc(DAT_005d17d0,iVar1 + -1);
  DAT_005d17d4 = FUN_0012b094(param_1,uVar2);
  FUN_00127eb4(DAT_005d17d0,iVar1 + -1,0,uVar2,DAT_005d17d4);
  return;
}

