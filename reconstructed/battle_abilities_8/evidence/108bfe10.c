/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bfe10(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar5;
  int iVar6;
  
  iVar2 = FUN_10865c48(param_2);
  if (iVar2 == 0) {
    iVar6 = 0;
    iVar2 = FUN_1086b91c(3);
    if (iVar2 == param_3) {
      uVar3 = FUN_1086b91c(5);
      uVar5 = 0;
      if (uVar3 != 0) {
        do {
          uVar1 = FUN_1086b91c(uVar5 + 6 & 0xff);
          FUN_108544a0(param_2,uVar1);
          iVar2 = FUN_1085f844();
          if (iVar2 != 0) {
            iVar6 = iVar6 + 1;
          }
          uVar5 = uVar5 + 1;
        } while (uVar5 < uVar3);
        if (iVar6 != 0) {
          puVar4 = (uint *)FUN_10869f7c(param_2,0xd,param_3);
          *(char *)(puVar4 + 3) = (char)param_3;
          *(undefined1 *)((int)puVar4 + 0x16) = 1;
          *puVar4 = *puVar4 | 0x800000;
          *(undefined1 *)(puVar4 + 1) = 1;
          *(char *)((int)puVar4 + 0x17) = (char)iVar6;
          FUN_10867b48(param_2);
          return;
        }
      }
    }
  }
  return;
}

