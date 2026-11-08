/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108beb00(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    iVar2 = FUN_108544a0(param_2,param_3);
    sVar1 = *(short *)(iVar2 + 0x14);
    if ((((sVar1 != 0) && (iVar3 = thunk_FUN_0039f0bc(), iVar3 != 0)) &&
        (*(short *)(iVar2 + 0x12) == 0)) &&
       (((iVar2 = FUN_1084be58(param_2), iVar2 == 1 || (iVar2 = FUN_1084c0f8(0x32), iVar2 != 0)) ||
        (iVar2 = FUN_1084c0ec(param_2,1), iVar2 != 0)))) {
      puVar4 = (uint *)FUN_10869f7c(param_2,0x21,param_3);
      *(short *)(puVar4 + 1) = sVar1;
      *(char *)((int)puVar4 + 6) = (char)param_3;
      *(undefined1 *)((int)puVar4 + 7) = 1;
      *puVar4 = *puVar4 | 0x800000;
      FUN_1084d2d4(puVar4 + 3,2,0x1db);
      FUN_1084d29c(puVar4 + 3,param_3);
      FUN_1084d29c(puVar4 + 3,sVar1);
      FUN_10867b48(param_2,puVar4);
      return;
    }
  }
  return;
}

