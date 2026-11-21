/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10888a88(int param_1)

{
  char cVar1;
  char cVar2;
  uint uVar3;
  uint uVar4;
  undefined4 uVar5;
  int iVar6;
  
  uVar3 = *(uint *)(param_1 + 0x60);
  if ((uVar3 & 7) == 0) goto LAB_10888afc;
  cVar1 = FUN_10871408();
  cVar2 = FUN_1086b91c(0x15);
  if (cVar1 != cVar2) goto LAB_10888afc;
  uVar4 = uVar3 & 7;
  if (uVar4 == 1 || uVar4 == 3) {
LAB_10888ae0:
    uVar3 = (uVar3 & 0x2000000) >> 0x19;
LAB_10888ae8:
    if (uVar3 == 0) goto LAB_10888b20;
    uVar5 = 0x4d;
  }
  else {
    if (uVar4 == 4) {
      uVar3 = uVar3 >> 0x1f;
      goto LAB_10888ae8;
    }
    if (uVar4 == 2) goto LAB_10888ae0;
LAB_10888b20:
    uVar5 = 0x4c;
  }
  FUN_108658d4(uVar5,1);
LAB_10888afc:
  if (((*(uint *)(param_1 + 0x70) & 7) != 0) && (iVar6 = FUN_1086b91c(0x16), iVar6 == 4)) {
    FUN_108658d4(0x4c,1);
  }
  if (((*(uint *)(param_1 + 0x98) & 7) != 0) && (iVar6 = FUN_1086b91c(0x16), iVar6 == 4)) {
    FUN_108658d4(0x4c,1);
    return;
  }
  return;
}

