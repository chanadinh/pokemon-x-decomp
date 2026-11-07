/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c1784(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(4);
  if (((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x1b), iVar1 == 1)) &&
     (iVar1 = FUN_1086b91c(0x47), iVar1 == 0)) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_108722e8(uVar2,2,0xffffffff);
    if (((iVar1 != 0) || (iVar1 = FUN_108722e8(uVar2,5,1), iVar1 != 0)) &&
       (iVar1 = FUN_1085f844(uVar2), iVar1 == 0)) {
      FUN_1084c948(param_2,1,param_3);
      iVar1 = FUN_10869f7c(param_2,0xd,param_3);
      *(undefined1 *)(iVar1 + 0x16) = 1;
      *(undefined1 *)(iVar1 + 4) = 2;
      *(undefined1 *)(iVar1 + 0x17) = 0xff;
      *(byte *)(iVar1 + 0x18) = *(byte *)(iVar1 + 0x18) | 2;
      *(char *)(iVar1 + 0xc) = (char)param_3;
      FUN_10867b48(param_2);
      iVar1 = FUN_10869f7c(param_2,0xd,param_3);
      *(undefined1 *)(iVar1 + 0x16) = 1;
      *(undefined1 *)(iVar1 + 0x17) = 1;
      *(undefined1 *)(iVar1 + 4) = 5;
      *(byte *)(iVar1 + 0x18) = *(byte *)(iVar1 + 0x18) | 2;
      *(char *)(iVar1 + 0xc) = (char)param_3;
      FUN_10867b48(param_2);
      FUN_1084c948(param_2,2,param_3);
      return;
    }
  }
  return;
}

