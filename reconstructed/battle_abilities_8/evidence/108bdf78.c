/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bdf78(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined2 uVar1;
  undefined1 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint *puVar6;
  
  uVar2 = FUN_1086b91c(2);
  iVar3 = FUN_1084c5a8(param_3,uVar2);
  if (iVar3 != 0) {
    iVar3 = FUN_108544a0(param_2,param_3);
    iVar4 = FUN_108544a0(param_2,uVar2);
    if ((((*(short *)(iVar3 + 0x12) != 0) && (*(short *)(iVar4 + 0x12) == 0)) &&
        (iVar5 = FUN_1084c818(iVar3,0xf), iVar5 == 0)) &&
       (iVar5 = FUN_1084c818(iVar3,0x11), iVar5 == 0)) {
      uVar1 = *(undefined2 *)(iVar3 + 0x12);
      iVar4 = FUN_1085441c(param_2,*(undefined2 *)(iVar4 + 0xc),uVar1);
      if ((iVar4 == 0) &&
         (iVar3 = FUN_1085441c(param_2,*(undefined2 *)(iVar3 + 0xc),uVar1), iVar3 == 0)) {
        puVar6 = (uint *)FUN_10869f7c(param_2,0x25,param_3);
        *(undefined1 *)(puVar6 + 1) = uVar2;
        *puVar6 = *puVar6 | 0x800000;
        FUN_1084d2d4(puVar6 + 2,2,0x4c3);
        FUN_1084d29c(puVar6 + 2,param_3);
        FUN_1084d29c(puVar6 + 2,uVar1);
        FUN_1084d29c(puVar6 + 2,uVar2);
        FUN_10867b48(param_2,puVar6);
        return;
      }
    }
  }
  return;
}

