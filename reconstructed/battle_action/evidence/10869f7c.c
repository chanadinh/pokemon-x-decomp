/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint * FUN_10869f7c(int param_1,uint param_2,uint param_3)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  int iVar5;
  uint uVar6;
  
  puVar1 = (uint *)(param_1 + 0x437c);
  uVar2 = 0;
  do {
    if ((byte)(&DAT_108d1ed8)[uVar2 * 2] == param_2) {
      uVar3 = (uint)(byte)(&DAT_108d1ed9)[uVar2 * 2];
      if (uVar3 != 0) {
        if (((&DAT_108d1ed9)[uVar2 * 2] & 3) != 0) {
          do {
            uVar3 = uVar3 + 1;
          } while ((uVar3 & 3) != 0);
        }
        uVar2 = *puVar1;
        if (uVar3 + ((uVar2 & 0x7ffff) >> 10) < 0x1f5) {
          puVar4 = (uint *)((int)puVar1 + ((uVar2 & 0x7ffff) >> 10) + 4);
          if (uVar3 != 0) {
            uVar6 = uVar3 & 1;
            if (uVar6 == 1) {
              *(undefined1 *)((int)puVar1 + ((uVar2 & 0x7ffff) >> 10) + 4) = 0;
            }
            uVar2 = (uint)(uVar6 == 1);
            for (; uVar6 < uVar3; uVar6 = uVar6 + 2) {
              *(undefined1 *)((int)puVar1 + uVar2 + ((*puVar1 & 0x7ffff) >> 10) + 4) = 0;
              iVar5 = uVar2 + ((*puVar1 & 0x7ffff) >> 10);
              uVar2 = uVar2 + 2;
              *(undefined1 *)((int)puVar1 + iVar5 + 5) = 0;
            }
          }
          *(char *)puVar4 = (char)param_2;
          *puVar4 = *puVar4 & 0xff0000ff | (uVar3 & 0x3ff) << 0xd | (param_3 & 0x1f) << 8 |
                    0x4000000;
          uVar2 = *puVar1 >> 0x13;
          *puVar1 = (uVar2 | *puVar1 << 0xd) + uVar3 * 0x800000 >> 0xd | uVar2 << 0x13;
          return puVar4;
        }
      }
      break;
    }
    uVar2 = uVar2 + 1;
  } while (uVar2 < 0x3d);
  FUN_10800394(0,0);
  return (uint *)0x0;
}

