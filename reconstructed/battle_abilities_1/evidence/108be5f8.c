/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108be5f8(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  uint *puVar4;
  
  iVar3 = FUN_1086b91c(4);
  if ((iVar3 == param_3) && (iVar3 = FUN_1086b91c(0x47), iVar3 == 0)) {
    uVar2 = FUN_1086b91c(0x12);
    iVar3 = thunk_FUN_0039df44(uVar2,0);
    if (iVar3 != 0) {
      uVar1 = FUN_1086b91c(3);
      FUN_108544a0(param_2,uVar1);
      iVar3 = FUN_1085f844();
      if (iVar3 == 0) {
        puVar4 = (uint *)FUN_10869f7c(param_2,0xd,param_3);
        *(undefined1 *)((int)puVar4 + 0x16) = 1;
        *(undefined1 *)(puVar4 + 1) = 5;
        *puVar4 = *puVar4 | 0x800000;
        *(undefined1 *)((int)puVar4 + 0x17) = 0xff;
        *(byte *)(puVar4 + 6) = (byte)puVar4[6] | 0x12;
        *(undefined1 *)(puVar4 + 3) = uVar1;
        FUN_10867b48(param_2);
        return;
      }
    }
  }
  return;
}

