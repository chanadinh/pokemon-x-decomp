/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c10a0(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  short sVar2;
  int iVar3;
  undefined4 uVar4;
  uint *puVar5;
  uint uVar6;
  undefined4 uVar7;
  
  iVar3 = FUN_1086b91c(4);
  if (((iVar3 == param_3) && (iVar3 = FUN_1086b91c(0x47), iVar3 == 0)) &&
     (iVar3 = FUN_10865c48(param_2), iVar3 == 0)) {
    uVar1 = FUN_1086b91c(3);
    uVar4 = FUN_108544a0(param_2,uVar1);
    iVar3 = FUN_10856a88(uVar4,0xd);
    if (iVar3 == 0) {
      sVar2 = FUN_1086b91c(0x14);
      if (((sVar2 != 0) && (sVar2 != 0xa5)) &&
         ((iVar3 = FUN_1084c808(sVar2), iVar3 == 0 &&
          ((iVar3 = FUN_1084c0f8(0x1e), iVar3 != 0 || (iVar3 = FUN_1084c0ec(param_2,1), iVar3 != 0))
          )))) {
        uVar7 = 4;
        iVar3 = FUN_1084c818(uVar4,1);
        if (iVar3 != 0) {
          uVar7 = 5;
        }
        puVar5 = (uint *)FUN_10869f7c(param_2,0xb,param_3);
        *(undefined2 *)(puVar5 + 1) = 0xd;
        uVar6 = FUN_1084beac(uVar7,sVar2);
        puVar5[2] = uVar6;
        *(undefined1 *)((int)puVar5 + 0xf) = uVar1;
        *puVar5 = *puVar5 | 0x800000;
        FUN_10867b48(param_2,puVar5);
        return;
      }
    }
  }
  return;
}

