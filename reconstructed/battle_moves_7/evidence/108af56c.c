/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108af56c(undefined4 param_1,undefined4 param_2,int param_3,uint *param_4)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  bool bVar6;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    iVar2 = FUN_108544a0(param_2,param_3);
    uVar3 = (uint)*(ushort *)(iVar2 + 0x12);
    bVar6 = uVar3 == 0;
    if (bVar6) {
      uVar3 = *param_4;
    }
    if (bVar6 && uVar3 == 0) {
      cVar1 = FUN_1086b91c(6);
      if (((cVar1 != '\x1f') && (iVar2 = FUN_108544a0(param_2,cVar1), *(short *)(iVar2 + 0x12) != 0)
          ) && (iVar4 = FUN_1084bb5c(param_2,param_3,cVar1), iVar4 == 0)) {
        iVar4 = FUN_10869f7c(param_2,0x25,param_3);
        *(char *)(iVar4 + 4) = cVar1;
        FUN_1084d2d4(iVar4 + 8,2,0x427);
        FUN_1084d29c(iVar4 + 8,param_3);
        FUN_1084d29c(iVar4 + 8,cVar1);
        FUN_1084d29c(iVar4 + 8,*(undefined2 *)(iVar2 + 0x12));
        FUN_10867b48(param_2,iVar4);
        puVar5 = (uint *)FUN_10869f7c(param_2,0x3b,param_3);
        *(undefined1 *)(puVar5 + 1) = 1;
        *puVar5 = *puVar5 | 0x1000000;
        FUN_10867b48(param_2);
        return;
      }
    }
  }
  return;
}

