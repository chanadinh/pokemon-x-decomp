/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108c59b4(int param_1,int param_2,undefined4 param_3)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  uVar4 = (uint)*(byte *)(param_2 + 0x3b);
  iVar3 = 0;
  if (uVar4 != 0x1f) {
    iVar3 = *(int *)(param_1 + 8);
    if (0x17 < uVar4) {
      FUN_10800394(0,0);
    }
    iVar3 = iVar3 + uVar4 * 4;
    if (*(int *)(iVar3 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar3 = *(int *)(iVar3 + 0x84);
  }
  iVar2 = DAT_108fca60;
  if (*(char *)(param_2 + 0x10) == '\x04') {
    if ((iVar3 == 0) || ((*(uint *)(iVar3 + 0x5c) & 7) == 0)) goto joined_r0x108c5a84;
  }
  else if ((*(char *)(param_2 + 0x10) != '\x05') ||
          ((*(int *)(*(int *)(param_1 + 0x1c) + 0x290) == 0 &&
           ((iVar3 == 0 ||
            (((*(uint *)(iVar3 + 0x68) & 7) == 0 && ((*(byte *)(iVar3 + 0x167) & 2) == 0)))))))) {
joined_r0x108c5a84:
    do {
      if (iVar2 == 0) {
        return 0;
      }
      if (*(int *)(iVar2 + 0xc) != 0) {
        if (*(char *)(iVar2 + 0x10) == '\x04') {
          bVar1 = *(byte *)(iVar2 + 0x3b);
          iVar3 = *(int *)(param_1 + 8);
          if (0x17 < bVar1) {
            FUN_10800394(0,0);
          }
          iVar3 = iVar3 + (uint)bVar1 * 4;
          if (*(int *)(iVar3 + 0x84) == 0) {
            FUN_10800394(0,0);
          }
          iVar3 = *(int *)(iVar3 + 0x84);
          if ((iVar3 != 0) && ((*(uint *)(iVar3 + 0x5c) & 7) != 0)) goto LAB_108c5b3c;
        }
        if (((*(uint *)(iVar2 + 0x18) & 0x40000000) == 0) &&
           (iVar3 = (**(code **)(iVar2 + 0xc))
                              (iVar2,param_1,*(undefined1 *)(param_2 + 0x10),param_3,
                               *(undefined2 *)(param_2 + 0x38),*(undefined1 *)(param_2 + 0x3a)),
           iVar3 != 0)) {
          return 1;
        }
      }
LAB_108c5b3c:
      iVar2 = *(int *)(iVar2 + 4);
    } while( true );
  }
  return 1;
}

