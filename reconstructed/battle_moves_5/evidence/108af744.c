/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108af744(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (iVar1 = FUN_1084c374(param_2,2), iVar1 == 0)) {
    iVar1 = FUN_10869f7c(param_2,0x1c,param_3);
    *(undefined1 *)(iVar1 + 4) = 2;
    uVar2 = FUN_1084be3c(5);
    *(undefined4 *)(iVar1 + 8) = uVar2;
    *(undefined2 *)(iVar1 + 0xc) = 0x7b;
    *(ushort *)(iVar1 + 0xe) = *(ushort *)(iVar1 + 0xe) & 0xff80 | 1;
    FUN_10867b48(param_2,iVar1);
    FUN_1084c948(param_2,0x36,param_3);
    return;
  }
  return;
}

