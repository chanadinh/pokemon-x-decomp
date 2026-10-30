/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_10826264(int param_1,int param_2,int param_3,int param_4,int param_5)

{
  uint uVar1;
  bool bVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  uint local_2c;
  
  bVar2 = false;
  if (*(char *)(param_4 + 6) != '\x12') {
    uVar3 = FUN_10870994(param_3);
    uVar6 = uVar3 & 0x1f;
    local_2c = FUN_1086094c(param_1,param_2,param_3,*(undefined1 *)(param_4 + 6),uVar6);
    if (((uVar3 & 0x1f) != (uVar3 & 0x3ff) >> 5) || ((uVar3 & 0x7c00) != 0x4800)) {
      uVar1 = (uVar3 & 0x3ff) >> 5;
      if (uVar1 != 0x12 && uVar1 != uVar6) {
        uVar4 = FUN_1086094c(param_1,param_2,param_3,*(undefined1 *)(param_4 + 6),uVar1);
        local_2c = FUN_10860b80(local_2c,uVar4);
      }
      uVar3 = (uVar3 & 0x7fff) >> 10;
      if ((uVar3 != 0x12 && uVar3 != uVar6) && uVar3 != uVar1) {
        uVar4 = FUN_1086094c(param_1,param_2,param_3,*(undefined1 *)(param_4 + 6),uVar3);
        local_2c = FUN_10860b80(local_2c,uVar4);
      }
    }
    if (local_2c != 0) {
      if (((*(char *)(param_4 + 6) == '\x04') && (*(int *)(*(int *)(param_1 + 0x1c) + 0x27c) == 0))
         && (iVar5 = FUN_108651d8(param_1,param_3,0), iVar5 != 0)) {
        FUN_1086bb84();
        FUN_1086bac0(3,*(undefined1 *)(param_2 + 0x19));
        FUN_1086bac0(4,*(undefined1 *)(param_3 + 0x19));
        FUN_1086b9fc(0x42,0);
        FUN_1086b9a4(param_1,0x46);
        iVar5 = FUN_1086b91c(0x42);
        if (iVar5 == 0) {
          FUN_1086b880();
          bVar2 = true;
          goto code_r0x10826438;
        }
        FUN_1086b880();
      }
      return local_2c;
    }
    if ((*(char *)(param_4 + 6) != '\x04') ||
       ((*(int *)(*(int *)(param_1 + 0x1c) + 0x27c) == 0 &&
        (iVar5 = FUN_108651d8(param_1,param_3,1), iVar5 != 0)))) {
code_r0x10826438:
      FUN_1086bb84();
      FUN_1086bac0(3,*(undefined1 *)(param_2 + 0x19));
      FUN_1086bac0(4,*(undefined1 *)(param_3 + 0x19));
      FUN_1086bac0(0x16,*(undefined1 *)(param_4 + 6));
      FUN_1086b9fc(0x39,0);
      FUN_1086b9a4(param_1,0x45);
      uVar3 = FUN_1086b91c(0x39);
      FUN_1086b880();
      if (((uVar3 & 0xff) == 0) && (param_5 != 0)) {
        if (bVar2) {
          uVar4 = FUN_1086b284();
          FUN_1086bb84();
          FUN_1086bac0(2,*(undefined1 *)(param_3 + 0x19));
          FUN_1086b9a4(param_1,0x1b);
          FUN_1086b880();
          if (((*(uint *)(param_1 + 0x437c) & 0x40000000) == 0) ||
             ((*(uint *)(param_1 + 0x437c) & 0x20000000) == 0)) {
            FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0xd2,*(undefined1 *)(param_3 + 0x19),
                         0xffff0000);
          }
          *(undefined4 *)(param_1 + 0x437c) = uVar4;
        }
        else {
          FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0xd2,*(undefined1 *)(param_3 + 0x19),
                       0xffff0000);
        }
      }
      return uVar3 & 0xff;
    }
  }
  return 7;
}

