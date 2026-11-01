/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c1ee8(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  undefined1 *puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_1084cb78(param_2,param_3);
    puVar3 = (undefined1 *)FUN_1084c7d0(param_2,10);
    iVar1 = FUN_1084bc20(param_2,uVar2 | 0x100,puVar3);
    if (iVar1 != 0) {
      FUN_1084c948(param_2,1,param_3);
      iVar4 = FUN_10869f7c(param_2,0xd,param_3);
      *(undefined1 *)(iVar4 + 4) = 1;
      *(char *)(iVar4 + 0x16) = (char)iVar1;
      *(undefined1 *)(iVar4 + 0x17) = 0xff;
      *(byte *)(iVar4 + 0x18) = *(byte *)(iVar4 + 0x18) | 2;
      iVar5 = 0;
      do {
        iVar6 = iVar4 + iVar5;
        iVar1 = iVar1 + -1;
        iVar5 = iVar5 + 1;
        *(undefined1 *)(iVar6 + 0xc) = *puVar3;
        puVar3 = puVar3 + 1;
      } while (iVar1 != 0);
      FUN_10867b48(param_2);
      FUN_1084c948(param_2,2,param_3);
      return;
    }
  }
  return;
}

