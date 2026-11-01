/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

/* WARNING: Removing unreachable block (ram,0x10865918) */

undefined4 FUN_10865824(undefined4 param_1,int param_2,int param_3,int param_4)

{
  byte bVar1;
  undefined2 uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int local_18;
  
  local_18 = param_4;
  iVar3 = FUN_1086b91c(4);
  if (((iVar3 == param_2) && (iVar3 = FUN_1086b91c(3), iVar3 != param_2)) &&
     (iVar3 = FUN_1086b91c(0x16), iVar3 == param_3)) {
    uVar2 = FUN_1086b91c(0x12);
    iVar3 = thunk_FUN_0039dcf0(uVar2);
    uVar5 = DAT_10b07350;
    if ((iVar3 != 2) || (uVar4 = thunk_FUN_0039df08(uVar2), uVar5 = DAT_10b07350, uVar4 == 0)) {
LAB_108658f0:
      do {
        if ((0x5f < uVar5) || ((&DAT_10b07354)[uVar5] == 0)) {
          FUN_10800394(0,0);
          uVar5 = 0xffffffff;
LAB_10865948:
          if (-1 < (int)uVar5) {
            bVar1 = (&DAT_10b07894)[uVar5];
            if (bVar1 < 2) {
              (&DAT_10b07354)[uVar5] = 0x41;
              (&DAT_10b07414)[uVar5] = 1;
              if ((&DAT_10b07894)[uVar5] == '\x01') {
                (&DAT_10b07894)[uVar5] = 2;
              }
              return 1;
            }
            if (bVar1 == 4 || bVar1 == 3) {
              FUN_10800394(0,0);
            }
          }
          return 0;
        }
        if ((&DAT_10b07354)[uVar5] == 0x41) goto LAB_10865948;
        uVar5 = uVar5 + 1;
      } while( true );
    }
    uVar6 = 0;
    do {
      thunk_FUN_0039ddd4(uVar2,uVar6,&local_18);
      uVar5 = DAT_10b07350;
      if (local_18 < 0) goto LAB_108658f0;
      uVar6 = uVar6 + 1;
    } while (uVar6 < uVar4);
  }
  return 0;
}

