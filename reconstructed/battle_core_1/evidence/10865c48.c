/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_10865c48(int param_1,undefined4 param_2,undefined4 param_3,uint param_4)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  uint uVar8;
  char cVar9;
  bool bVar10;
  uint uStack_28;
  
  uVar8 = 0;
  uStack_28 = param_4 & 0xffff0000;
  while( true ) {
    uVar3 = uVar8 & 0xff;
    if (uVar3 < 4) break;
LAB_10865d30:
    uVar8 = uVar8 + 1;
    if (3 < uVar8) {
      bVar10 = (uStack_28 & 0xff) != 0;
      uVar8 = 0;
      if (bVar10) {
        uVar8 = uStack_28 >> 8 & 0xff;
      }
      return !bVar10 || uVar8 == 0;
    }
  }
  uVar1 = 0;
LAB_10865c7c:
  uVar4 = *(int *)(param_1 + 4) + uVar1;
  bVar10 = *(byte *)(uVar4 + 0x7ec) != uVar3;
  if (bVar10) {
    uVar4 = (uint)*(byte *)(uVar4 + 0x7ed);
  }
  if (bVar10 && uVar4 != uVar3) goto code_r0x10865c94;
  iVar5 = *(int *)(param_1 + 8);
  if (3 < uVar8) {
    FUN_10800394(0,0);
  }
  iVar5 = iVar5 + uVar8 * 0x1c;
  iVar6 = 0;
  cVar9 = '\0';
  if (*(char *)(iVar5 + 0x1c) != '\0') {
    do {
      puVar7 = *(undefined4 **)(iVar5 + 4 + iVar6 * 4);
      iVar2 = thunk_FUN_004a1658(*puVar7,2);
      if ((iVar2 == 0) && (iVar2 = FUN_10874420(puVar7,0xd), iVar2 != 0)) {
        cVar9 = cVar9 + '\x01';
      }
      iVar6 = iVar6 + 1;
    } while (iVar6 < (int)(uint)*(byte *)(iVar5 + 0x1c));
  }
  if (cVar9 != '\0') {
    *(undefined1 *)((int)&uStack_28 + (uVar8 & 1)) = 1;
  }
  goto LAB_10865d30;
code_r0x10865c94:
  uVar1 = uVar1 + 2;
  if (9 < uVar1) goto LAB_10865d30;
  goto LAB_10865c7c;
}

