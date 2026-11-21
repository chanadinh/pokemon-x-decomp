/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084a944(undefined4 param_1,undefined4 param_2,undefined4 param_3,uint param_4,uint param_5
                 )

{
  int iVar1;
  uint *puVar2;
  
  iVar1 = FUN_108544a0(param_2,param_3);
  if (*(byte *)(iVar1 + 0x153) != param_4) {
    puVar2 = (uint *)FUN_10869f7c(param_2,0x3a,param_3);
    *(char *)((int)puVar2 + 5) = (char)param_4;
    *(char *)(puVar2 + 1) = (char)param_3;
    *puVar2 = *puVar2 & 0xff7fffff | (param_5 & 1) << 0x17;
    FUN_1084d2d4(puVar2 + 2,2,0xde);
    FUN_1084d29c(puVar2 + 2,param_3);
    FUN_10867b48(param_2,puVar2);
    return;
  }
  return;
}

