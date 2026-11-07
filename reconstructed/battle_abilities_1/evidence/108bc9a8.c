/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bc9a8(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  uint *puVar5;
  
  iVar3 = FUN_1086b91c(4);
  if (((iVar3 == param_3) && (iVar3 = FUN_1086b91c(0x47), iVar3 == 0)) &&
     (iVar3 = FUN_10865c48(param_2), iVar3 == 0)) {
    uVar2 = FUN_1086b91c(0x12);
    iVar3 = thunk_FUN_0039df44(uVar2,0);
    if (iVar3 != 0) {
      uVar1 = FUN_1086b91c(3);
      uVar4 = FUN_108544a0(param_2,uVar1);
      iVar3 = FUN_10874420(uVar4,0x10);
      if (iVar3 != 0x98) {
        puVar5 = (uint *)FUN_10869f7c(param_2,0x20,param_3);
        *(undefined1 *)((int)puVar5 + 6) = uVar1;
        *(undefined2 *)(puVar5 + 1) = 0x98;
        FUN_1084d2d4(puVar5 + 3,2,0x1cf);
        FUN_1084d29c(puVar5 + 3,*(undefined1 *)((int)puVar5 + 6));
        iVar3 = FUN_1084c5a8(param_3,uVar1);
        if (iVar3 == 0) {
          *puVar5 = *puVar5 | 0x800000;
        }
        FUN_10867b48(param_2,puVar5);
        return;
      }
    }
  }
  return;
}

