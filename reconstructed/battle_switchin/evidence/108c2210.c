/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c2210(undefined4 param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  int iVar1;
  int iVar2;
  uint *puVar3;
  char cVar4;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 != param_3) {
    return;
  }
  iVar1 = FUN_1084be58(param_2);
  iVar2 = FUN_108544a0(param_2,param_3);
  if (*(short *)(iVar2 + 0xc) != 0x15f) goto LAB_108c22f0;
  if (iVar1 == 0) {
LAB_108c2294:
    cVar4 = '\0';
  }
  else if (iVar1 == 1) {
    cVar4 = '\x01';
  }
  else if (iVar1 == 2) {
    cVar4 = '\x02';
  }
  else {
    if (iVar1 != 3) goto LAB_108c2294;
    cVar4 = '\x03';
  }
  if (cVar4 != *(char *)(iVar2 + 0x153)) {
    puVar3 = (uint *)FUN_10869f7c(param_2,0x3a,param_3);
    *(char *)((int)puVar3 + 5) = cVar4;
    *(char *)(puVar3 + 1) = (char)param_3;
    *puVar3 = *puVar3 | 0x800000;
    FUN_1084d2d4(puVar3 + 2,2,0xde);
    FUN_1084d29c(puVar3 + 2,param_3);
    FUN_10867b48(param_2,puVar3);
  }
LAB_108c22f0:
  *param_4 = 1;
  return;
}

