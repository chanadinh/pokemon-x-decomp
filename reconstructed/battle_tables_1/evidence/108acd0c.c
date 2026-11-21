/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_108acd0c(uint param_1,uint param_2,int param_3)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  int *piVar5;
  uint local_1c [2];
  
  if (1 < param_1) {
    FUN_10800394(0,0);
  }
  if (0x10 < param_2) {
    FUN_10800394(0,0);
  }
  piVar5 = &DAT_10b11c00 + param_1 * 0x44 + param_2 * 4;
  uVar1 = 0;
  uVar4 = piVar5[3];
  do {
    if ((byte)(&DAT_108d4fe8)[uVar1 * 0xc] == param_2) {
      if (uVar4 == 0) {
        uVar2 = (**(code **)(&UNK_108d4fec + uVar1 * 0xc))(local_1c);
        iVar3 = FUN_1084cfc0(2,param_2,2,0,param_1,uVar2,local_1c[0] & 0xffff);
        if (iVar3 == 0) {
          thunk_FUN_0011e864(0,0);
        }
        FUN_1084cf8c(iVar3,6,param_3);
        piVar5[2] = 0;
        piVar5[3] = 1;
        *piVar5 = iVar3;
        piVar5[1] = param_3;
        return iVar3;
      }
      if (uVar4 < (uint)(&DAT_108d4ff0)[uVar1 * 3]) {
        piVar5[3] = uVar4 + 1;
        return *piVar5;
      }
    }
    uVar1 = uVar1 + 1;
    if (0x10 < uVar1) {
      return 0;
    }
  } while( true );
}

