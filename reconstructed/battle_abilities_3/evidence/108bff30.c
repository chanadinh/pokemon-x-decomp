/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bff30(undefined4 param_1,undefined4 param_2,uint param_3)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  uint *puVar4;
  uint uVar5;
  byte local_28 [8];
  
  uVar2 = FUN_1086b91c(2);
  if (uVar2 == param_3) {
    uVar2 = FUN_10873158(param_2,param_3);
    uVar2 = FUN_1084bc20(param_2,uVar2 | 0x400,local_28);
    uVar5 = 0;
    if (uVar2 != 0) {
      do {
        if (local_28[uVar5] != param_3) {
          FUN_108544a0(param_2);
          iVar3 = FUN_1086d8ac();
          if ((iVar3 != 0) &&
             ((iVar3 = FUN_1084c0f8(0x1e), iVar3 != 0 ||
              (iVar3 = FUN_1084c0ec(param_2,1), iVar3 != 0)))) {
            puVar4 = (uint *)FUN_10869f7c(param_2,10,param_3);
            *(undefined1 *)((int)puVar4 + 0x11) = 1;
            *puVar4 = *puVar4 | 0x2800000;
            bVar1 = local_28[uVar5];
            *(undefined1 *)(puVar4 + 1) = 0x26;
            *(byte *)((int)puVar4 + 5) = bVar1;
            FUN_10867b48(param_2);
          }
        }
        uVar5 = uVar5 + 1 & 0xff;
      } while (uVar5 < uVar2);
    }
  }
  return;
}

