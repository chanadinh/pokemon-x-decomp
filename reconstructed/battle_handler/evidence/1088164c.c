/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1088164c(void)

{
  int *piVar1;
  uint uVar2;
  int *piVar3;
  
  piVar3 = DAT_108fca60;
  while (piVar1 = piVar3, piVar1 != (int *)0x0) {
    piVar3 = (int *)piVar1[1];
    if ((((char)piVar1[4] == '\x06') && (piVar1 != (int *)0x0)) &&
       (uVar2 = piVar1[6], (uVar2 & 0x20000000) != 0)) {
      if ((uVar2 & 0x1000000) == 0) {
        if (piVar1 == DAT_108fca60) {
          DAT_108fca60 = piVar3;
        }
        if (*piVar1 != 0) {
          *(int **)(*piVar1 + 4) = piVar3;
        }
        if ((int *)piVar1[1] != (int *)0x0) {
          *(int *)piVar1[1] = *piVar1;
        }
        if (DAT_108fca64 == 0) {
          FUN_10800394(0,0);
        }
        else {
          *piVar1 = 0;
          piVar1[1] = 0;
          piVar1[2] = 0;
          piVar1[3] = 0;
          piVar1[4] = 0;
          piVar1[5] = 0;
          piVar1[6] = 0;
          piVar1[7] = 0;
          piVar1[8] = 0;
          piVar1[9] = 0;
          piVar1[10] = 0;
          piVar1[0xb] = 0;
          piVar1[0xc] = 0;
          piVar1[0xd] = 0;
          DAT_108fca64 = DAT_108fca64 + -1;
          piVar1[0xe] = 0;
          (&DAT_10b1196c)[DAT_108fca64] = piVar1;
        }
      }
      else {
        piVar1[6] = uVar2 | 0x8000000;
      }
    }
  }
  return;
}

