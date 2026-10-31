/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10869dd4(int param_1,uint param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint local_24;
  
  uVar3 = 0;
  do {
    if ((ushort)(&DAT_108d3ddc)[uVar3 * 4] == param_2) {
      uVar1 = *(undefined1 *)(param_1 + 0x19);
      uVar4 = 0;
      iVar2 = FUN_1084d3c0(0,uVar1);
      while( true ) {
        if (iVar2 == 0) {
          iVar2 = (*(code *)(&DAT_108d3de0)[uVar3 * 2])(&local_24);
          if (iVar2 == 0) {
            return 0;
          }
          FUN_1084cfc0(0,param_2,0,param_3,uVar1,iVar2,local_24 & 0xffff);
          return 1;
        }
        if (*(ushort *)(iVar2 + 0x38) == param_2) break;
        uVar4 = uVar4 + 1;
        if (8 < uVar4) {
          return 0;
        }
        iVar2 = FUN_1084d370(iVar2);
      }
      return 0;
    }
    uVar3 = uVar3 + 1;
  } while (uVar3 < 0x128);
  return 0;
}

