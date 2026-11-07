/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c05ec(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  uint *puVar4;
  
  iVar2 = FUN_1086b91c(4);
  if (((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x47), iVar2 == 0)) &&
     (iVar2 = FUN_1086b91c(0x46), iVar2 != 0)) {
    uVar3 = FUN_108544a0(param_2,param_3);
    iVar2 = FUN_1084b82c(uVar3,1);
    if (0 < iVar2) {
      puVar4 = (uint *)FUN_10869f7c(param_2,0xd,param_3);
      *(undefined1 *)(puVar4 + 1) = 1;
      uVar1 = FUN_1084b82c(uVar3,1);
      *(char *)(puVar4 + 3) = (char)param_3;
      *(undefined1 *)((int)puVar4 + 0x17) = uVar1;
      *(undefined1 *)((int)puVar4 + 0x16) = 1;
      *(byte *)(puVar4 + 6) = (byte)puVar4[6] | 3;
      *puVar4 = *puVar4 | 0x800000;
      FUN_1084d2d4(puVar4 + 7,2,0x1e1);
      FUN_1084d29c(puVar4 + 7,param_3);
      FUN_10867b48(param_2,puVar4);
      return;
    }
  }
  return;
}

