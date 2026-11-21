/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b5a48(undefined4 param_1,undefined4 param_2,int param_3)

{
  bool bVar1;
  undefined1 uVar2;
  int iVar3;
  undefined4 *puVar4;
  int iVar5;
  uint uVar6;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 == param_3) {
    uVar2 = FUN_1086b91c(6);
    iVar3 = FUN_108544a0(param_2,uVar2);
    bVar1 = false;
    puVar4 = (undefined4 *)FUN_1084c7d0(param_2,0x1c);
    uVar6 = 0;
    do {
      iVar5 = FUN_10874420(iVar3,uVar6 + 1 & 0xff);
      puVar4[uVar6] = iVar5;
      if (iVar5 != 6) {
        bVar1 = true;
        puVar4[uVar6] = 6 - (iVar5 + -6);
      }
      uVar6 = uVar6 + 1;
    } while (uVar6 < 7);
    if (bVar1) {
      iVar5 = FUN_10869f7c(param_2,0xe,param_3);
      *(undefined1 *)(iVar5 + 4) = uVar2;
      *(char *)(iVar5 + 5) = (char)*puVar4;
      *(char *)(iVar5 + 6) = (char)puVar4[1];
      *(char *)(iVar5 + 7) = (char)puVar4[2];
      *(char *)(iVar5 + 8) = (char)puVar4[3];
      *(char *)(iVar5 + 9) = (char)puVar4[4];
      *(char *)(iVar5 + 10) = (char)puVar4[5];
      *(char *)(iVar5 + 0xb) = (char)puVar4[6];
      *(undefined1 *)(iVar5 + 0xc) = *(undefined1 *)(iVar3 + 0x155);
      FUN_10867b48(param_2,iVar5);
      iVar3 = FUN_10869f7c(param_2,3,param_3);
      FUN_1084d2d4(iVar3 + 4,2,0x499);
      FUN_1084d29c(iVar3 + 4,uVar2);
      FUN_10867b48(param_2,iVar3);
      return;
    }
  }
  return;
}

