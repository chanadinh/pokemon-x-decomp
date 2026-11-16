/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b7db8(undefined4 param_1,undefined4 param_2,int param_3)

{
  ushort uVar1;
  int iVar2;
  ushort *puVar3;
  uint uVar4;
  ushort *puVar5;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    FUN_108544a0(param_2,param_3);
    puVar3 = (ushort *)FUN_1084c7fc();
    uVar4 = 0;
    while( true ) {
      puVar5 = &DAT_108d4b28 + uVar4;
      uVar1 = *puVar5;
      if ((ushort *)(uint)uVar1 != puVar3) {
        puVar5 = (ushort *)(uint)(ushort)(&DAT_108d4b2a)[uVar4];
      }
      if ((ushort *)(uint)uVar1 == puVar3 || puVar5 == puVar3) break;
      uVar4 = uVar4 + 2;
      if (7 < uVar4) {
        iVar2 = FUN_10869f7c(param_2,0x27,param_3);
        *(undefined1 *)(iVar2 + 5) = 3;
        *(char *)(iVar2 + 4) = (char)param_3;
        *(undefined1 *)(iVar2 + 6) = 0;
        FUN_10867b48(param_2);
        return;
      }
    }
  }
  return;
}

