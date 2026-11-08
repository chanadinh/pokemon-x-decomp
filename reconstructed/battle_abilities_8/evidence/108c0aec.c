/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c0aec(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint *puVar6;
  uint uVar7;
  
  uVar3 = FUN_1086b91c(5);
  uVar7 = 0;
  if (uVar3 != 0) {
    do {
      iVar4 = FUN_1086b91c(uVar7 + 6 & 0xff);
      if (iVar4 == param_3) {
        uVar1 = FUN_1086b91c(3);
        iVar4 = FUN_1084bb5c(param_2,param_3,uVar1);
        if (iVar4 != 0) {
          return;
        }
        uVar2 = FUN_1086b91c(0x12);
        iVar4 = thunk_FUN_0039df44(uVar2,0);
        if (iVar4 == 0) {
          return;
        }
        iVar4 = FUN_108544a0(param_2,param_3);
        iVar5 = FUN_108544a0(param_2,uVar1);
        if (*(short *)(iVar4 + 0x12) != 0) {
          return;
        }
        if (*(short *)(iVar5 + 0x12) == 0) {
          return;
        }
        puVar6 = (uint *)FUN_10869f7c(param_2,0x25,param_3);
        *(undefined1 *)(puVar6 + 1) = uVar1;
        *puVar6 = *puVar6 | 0x800000;
        FUN_1084d2d4(puVar6 + 2,2,0x1cc);
        FUN_1084d29c(puVar6 + 2,uVar1);
        FUN_1084d29c(puVar6 + 2,*(undefined2 *)(iVar5 + 0x12));
        FUN_10867b48(param_2,puVar6);
        return;
      }
      uVar7 = uVar7 + 1;
    } while (uVar7 < uVar3);
  }
  return;
}

