/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b2eb8(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  uint *puVar6;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 == param_3) {
    uVar4 = FUN_108544a0(param_2,param_3);
    uVar5 = FUN_1084bb2c(uVar4,0);
    if (uVar5 < 3) {
      iVar3 = FUN_10869f7c(param_2,0x27,param_3);
      uVar1 = (undefined1)param_3;
      *(undefined1 *)(iVar3 + 4) = uVar1;
      *(undefined1 *)(iVar3 + 5) = 0;
      *(char *)(iVar3 + 6) = (char)uVar5 + '\x01';
      FUN_10867b48(param_2);
      iVar3 = FUN_10869f7c(param_2,3,param_3);
      FUN_1084d2d4(iVar3 + 4,2,0x2d1);
      FUN_1084d29c(iVar3 + 4,param_3);
      FUN_1084d29c(iVar3 + 4,uVar5 + 1);
      FUN_10867b48(param_2,iVar3);
      iVar3 = FUN_10869f7c(param_2,0xd,param_3);
      *(undefined1 *)(iVar3 + 0xc) = uVar1;
      *(undefined1 *)(iVar3 + 0x16) = 1;
      *(undefined1 *)(iVar3 + 4) = 2;
      *(undefined1 *)(iVar3 + 0x17) = 1;
      *(byte *)(iVar3 + 0x18) = *(byte *)(iVar3 + 0x18) | 6;
      FUN_10867b48(param_2);
      puVar6 = (uint *)FUN_10869f7c(param_2,0x27,param_3);
      *(undefined1 *)(puVar6 + 1) = uVar1;
      *(undefined1 *)((int)puVar6 + 5) = 1;
      *puVar6 = *puVar6 | 0x1000000;
      cVar2 = FUN_1084bb2c(uVar4,1);
      *(char *)((int)puVar6 + 6) = cVar2 + '\x01';
      FUN_10867b48(param_2,puVar6);
      iVar3 = FUN_10869f7c(param_2,0xd,param_3);
      *(undefined1 *)(iVar3 + 0x16) = 1;
      *(undefined1 *)(iVar3 + 0x17) = 1;
      *(undefined1 *)(iVar3 + 4) = 4;
      *(undefined1 *)(iVar3 + 0xc) = uVar1;
      *(byte *)(iVar3 + 0x18) = *(byte *)(iVar3 + 0x18) | 6;
      FUN_10867b48(param_2);
      puVar6 = (uint *)FUN_10869f7c(param_2,0x27,param_3);
      *(undefined1 *)(puVar6 + 1) = uVar1;
      *(undefined1 *)((int)puVar6 + 5) = 2;
      *puVar6 = *puVar6 | 0x1000000;
      cVar2 = FUN_1084bb2c(uVar4,2);
      *(char *)((int)puVar6 + 6) = cVar2 + '\x01';
      FUN_10867b48(param_2,puVar6);
      return;
    }
  }
  return;
}

