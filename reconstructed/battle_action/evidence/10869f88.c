/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint * FUN_10869f88(uint *param_1,uint param_2,uint param_3)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  int iVar4;
  uint uVar5;
  
  uVar1 = 0;
  do {
    if ((byte)(&DAT_108d1ed8)[uVar1 * 2] == param_2) {
      uVar2 = (uint)(byte)(&DAT_108d1ed9)[uVar1 * 2];
      if (uVar2 != 0) {
        if (((&DAT_108d1ed9)[uVar1 * 2] & 3) != 0) {
          do {
            uVar2 = uVar2 + 1;
          } while ((uVar2 & 3) != 0);
        }
        uVar1 = *param_1;
        if (uVar2 + ((uVar1 & 0x7ffff) >> 10) < 0x1f5) {
          puVar3 = (uint *)((int)param_1 + ((uVar1 & 0x7ffff) >> 10) + 4);
          if (uVar2 != 0) {
            uVar5 = uVar2 & 1;
            if (uVar5 == 1) {
              *(undefined1 *)((int)param_1 + ((uVar1 & 0x7ffff) >> 10) + 4) = 0;
            }
            uVar1 = (uint)(uVar5 == 1);
            for (; uVar5 < uVar2; uVar5 = uVar5 + 2) {
              *(undefined1 *)((int)param_1 + uVar1 + ((*param_1 & 0x7ffff) >> 10) + 4) = 0;
              iVar4 = uVar1 + ((*param_1 & 0x7ffff) >> 10);
              uVar1 = uVar1 + 2;
              *(undefined1 *)((int)param_1 + iVar4 + 5) = 0;
            }
          }
          *(char *)puVar3 = (char)param_2;
          *puVar3 = *puVar3 & 0xff0000ff | (uVar2 & 0x3ff) << 0xd | (param_3 & 0x1f) << 8 |
                    0x4000000;
          uVar1 = *param_1 >> 0x13;
          *param_1 = (uVar1 | *param_1 << 0xd) + uVar2 * 0x800000 >> 0xd | uVar1 << 0x13;
          return puVar3;
        }
      }
      break;
    }
    uVar1 = uVar1 + 1;
  } while (uVar1 < 0x3d);
  FUN_10800394(0,0);
  return (uint *)0x0;
}

