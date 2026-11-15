/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c0578(undefined4 param_1,undefined4 param_2,int param_3)

{
  ushort uVar1;
  int iVar2;
  uint *puVar3;
  
  iVar2 = FUN_1086b91c(3);
  if ((iVar2 == param_3) && (uVar1 = FUN_1086b91c(0x16), (uVar1 & 0xff) < 0x12)) {
    puVar3 = (uint *)FUN_10869f7c(param_2,0x13,param_3);
    *(char *)((int)puVar3 + 6) = (char)param_3;
    *puVar3 = *puVar3 | 0x800000;
    *(ushort *)(puVar3 + 1) = uVar1 & 0x1f | (uVar1 & 0x1f) << 5 | 0x4800;
    FUN_10867b48(param_2);
    return;
  }
  return;
}

