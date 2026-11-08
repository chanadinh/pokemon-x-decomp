/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c0828(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  uint *puVar3;
  
  uVar1 = FUN_1086b91c(2);
  iVar2 = FUN_1084c5a8(uVar1,param_3);
  if ((iVar2 != 0) && (iVar2 = FUN_108658d4(0x53,1), iVar2 != 0)) {
    puVar3 = (uint *)FUN_10869f7c(param_2,3,param_3);
    *puVar3 = *puVar3 | 0x800000;
    FUN_1084d2d4(puVar3 + 1,1,0x4b);
    FUN_10876c28(puVar3 + 1,0xd0024);
    FUN_10867b48(param_2,puVar3);
    return;
  }
  return;
}

