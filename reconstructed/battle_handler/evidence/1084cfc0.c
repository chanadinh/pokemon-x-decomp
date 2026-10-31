/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int * FUN_1084cfc0(int param_1,undefined2 param_2,uint param_3,uint param_4,undefined1 param_5,
                  int param_6,undefined1 param_7)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  
  if (0xfe < param_3) {
    FUN_10800394(0,0);
  }
  if (0xfffffe < param_4) {
    FUN_10800394(0,0);
  }
  if (DAT_108fca64 != 0x9d) {
    piVar3 = (int *)(&DAT_10b1196c)[DAT_108fca64];
    DAT_108fca64 = DAT_108fca64 + 1;
    if (piVar3 != (int *)0x0) {
      param_4 = param_4 | (7 - param_3) * 0x1000000;
      piVar3[5] = param_4;
      *(char *)(piVar3 + 4) = (char)param_1;
      *piVar3 = 0;
      piVar3[1] = 0;
      piVar3[2] = param_6;
      *(undefined1 *)((int)piVar3 + 0x1a) = param_7;
      *(undefined2 *)(piVar3 + 0xe) = param_2;
      piVar3[3] = 0;
      *(undefined1 *)((int)piVar3 + 0x3a) = param_5;
      piVar3[6] = piVar3[6] & 0x20ffffff;
      *(short *)(piVar3 + 6) = (short)DAT_108fca68;
      piVar3[6] = piVar3[6] | 0x20000000;
      if ((param_1 == 0 || param_1 == 4) || (param_1 == 5)) {
        *(undefined1 *)((int)piVar3 + 0x3b) = param_5;
      }
      else {
        *(undefined1 *)((int)piVar3 + 0x3b) = 0x1f;
      }
      piVar3[7] = 0;
      piVar3[8] = 0;
      piVar3[9] = 0;
      piVar3[10] = 0;
      piVar3[0xb] = 0;
      piVar3[0xc] = 0;
      piVar3[0xd] = 0;
      if (DAT_108fca60 != (int *)0x0) {
        if (param_4 <= (uint)DAT_108fca60[5]) {
          piVar1 = DAT_108fca60;
          piVar2 = (int *)DAT_108fca60[1];
          do {
            if (piVar2 == (int *)0x0) {
LAB_1084d150:
              piVar1[1] = (int)piVar3;
              *piVar3 = (int)piVar1;
              return piVar3;
            }
            if ((uint)piVar2[5] < param_4) {
              piVar3[1] = (int)piVar2;
              *piVar3 = *piVar2;
              if (*piVar2 != 0) {
                *(int **)(*piVar2 + 4) = piVar3;
              }
              *piVar2 = (int)piVar3;
              if (piVar2 != (int *)0x0) {
                return piVar3;
              }
              goto LAB_1084d150;
            }
            piVar1 = piVar2;
            piVar2 = (int *)piVar2[1];
          } while( true );
        }
        *DAT_108fca60 = (int)piVar3;
        piVar3[1] = (int)DAT_108fca60;
        *piVar3 = 0;
      }
      DAT_108fca60 = piVar3;
      return piVar3;
    }
  }
  FUN_10800394(0,0);
  return (int *)0x0;
}

