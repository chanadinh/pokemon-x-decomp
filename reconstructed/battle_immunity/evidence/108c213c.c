/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c213c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  uint *puVar3;
  
  iVar1 = FUN_1086b91c(4);
  if (iVar1 == param_3) {
    uVar2 = FUN_1086b91c(0x12);
    uVar2 = uVar2 & 0xffff;
    iVar1 = 0;
    if (uVar2 != 0x10f) {
      iVar1 = uVar2 - 0x100;
    }
    if ((uVar2 == 0x10f || iVar1 == 0x9f) && (iVar1 = FUN_108658d4(0x41,1), iVar1 != 0)) {
      puVar3 = (uint *)FUN_10869f7c(param_2,3,param_3);
      *puVar3 = *puVar3 | 0x800000;
      FUN_1084d2d4(puVar3 + 1,2,0xd2);
      FUN_1084d29c(puVar3 + 1,param_3);
      FUN_10867b48(param_2,puVar3);
      return;
    }
  }
  return;
}

