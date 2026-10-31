/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_108a5aac(uint param_1,undefined4 param_2,undefined4 param_3,int param_4,uint param_5)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  uint local_2c [2];
  
  if (4 < param_1) {
    FUN_10800394(0,0);
  }
  uVar3 = 0;
  do {
    if ((byte)(&DAT_108d6810)[uVar3 * 8] == param_1) {
      iVar1 = FUN_1084d3c0(1,param_2);
      while( true ) {
        if (iVar1 == 0) {
          uVar2 = (*(code *)(&DAT_108d6814)[uVar3 * 2])(local_2c);
          iVar1 = FUN_1084cfc0(1,param_1,1,0,param_2,uVar2,local_2c[0] & 0xffff);
          if (iVar1 != 0) {
            uVar3 = 0;
            if (param_5 != 0) {
              do {
                FUN_1084cf8c(iVar1,uVar3 & 0xff,*(undefined4 *)(param_4 + uVar3 * 4));
                uVar3 = uVar3 + 1;
              } while (uVar3 < param_5);
            }
            FUN_1084cf8c(iVar1,6,param_3);
          }
          return iVar1;
        }
        if (*(ushort *)(iVar1 + 0x38) == param_1) break;
        iVar1 = FUN_1084d370(iVar1);
      }
      return 0;
    }
    uVar3 = uVar3 + 1;
  } while (uVar3 < 5);
  return 0;
}

