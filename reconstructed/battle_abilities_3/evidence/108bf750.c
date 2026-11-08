/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bf750(undefined4 param_1,undefined4 param_2,int param_3)

{
  bool bVar1;
  undefined2 uVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  uint uVar6;
  undefined1 local_30 [12];
  
  iVar3 = FUN_1086b91c(2);
  if (iVar3 == param_3) {
    bVar1 = false;
    uVar4 = FUN_1084cb78(param_2,param_3);
    uVar4 = FUN_1084bc20(param_2,uVar4 | 0x100,local_30);
    uVar6 = 0;
    if (uVar4 != 0) {
      do {
        uVar5 = FUN_108544a0(param_2,local_30[uVar6]);
        iVar3 = FUN_10856a88(uVar5,2);
        if (iVar3 != 0) {
          if (!bVar1) {
            bVar1 = true;
            FUN_1084c948(param_2,1,param_3);
          }
          iVar3 = FUN_10869f7c(param_2,6,param_3);
          *(undefined1 *)(iVar3 + 6) = local_30[uVar6];
          uVar2 = FUN_1084d2f4(uVar5,8);
          *(undefined2 *)(iVar3 + 4) = uVar2;
          FUN_1084d2d4(iVar3 + 0xc,2,0x1e4);
          FUN_1084d29c(iVar3 + 0xc,local_30[uVar6]);
          FUN_10867b48(param_2,iVar3);
        }
        uVar6 = uVar6 + 1 & 0xff;
      } while (uVar6 < uVar4);
      if (bVar1) {
        FUN_1084c948(param_2,2,param_3);
      }
    }
  }
  return;
}

