/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4
FUN_10860e74(int param_1,uint param_2,int param_3,undefined4 param_4,undefined4 param_5,
            undefined4 param_6,undefined4 param_7,undefined4 param_8,int param_9,undefined4 param_10
            ,int param_11,int param_12)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  undefined8 uVar5;
  
  uVar5 = CONCAT44(param_10,param_7);
  iVar1 = FUN_10865c4c();
  if (iVar1 == 0) {
    FUN_1086bb84();
    FUN_1086bac0(2,*(undefined1 *)(param_3 + 0x19));
    FUN_1086bac0(3,param_6);
    FUN_1086bac0(0x20,param_4);
    FUN_1086b9fc(0x21,param_5);
    FUN_1086b9a4(param_1,99);
    iVar1 = FUN_1086b91c(0x21);
    FUN_1086b880();
    iVar2 = FUN_108722e8(param_3,param_4,iVar1);
    if (iVar2 == 0) {
      if (param_9 != 0) {
        if (iVar1 < 1) {
          FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0xae,*(undefined1 *)(param_3 + 0x19),
                       param_4,0xffff0000);
        }
        else {
          FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0x99,*(undefined1 *)(param_3 + 0x19),
                       param_4,0xffff0000);
        }
        *(byte *)(param_1 + 0x1093) = *(byte *)(param_1 + 0x1093) | 0x10;
      }
    }
    else {
      if (((*(short *)(param_3 + 0x208) == 0) || (param_12 != 0)) ||
         (param_2 == *(byte *)(param_3 + 0x19))) {
        FUN_1086bb84();
        FUN_1086bac0(2,*(undefined1 *)(param_3 + 0x19));
        FUN_1086bac0(3,param_2);
        FUN_1086bac0(0x20,param_4);
        FUN_1086bac0(0x21,iVar1);
        FUN_1086bac0(0x1a,param_8);
        FUN_1086b9fc(0x42,0);
        FUN_1086b9a4(param_1,100);
        iVar2 = FUN_1086b91c(0x42);
        FUN_1086b880();
        puVar4 = (undefined4 *)(param_1 + 0x437c);
        if (iVar2 != 0) {
          if (param_9 != 0) {
            *(byte *)(param_1 + 0x1093) = *(byte *)(param_1 + 0x1093) | 0x10;
            uVar3 = FUN_1086b284(puVar4);
            FUN_1086bb84();
            FUN_1086bac0(2,*(undefined1 *)(param_3 + 0x19));
            FUN_1086bac0(0x1a,param_8);
            FUN_1086b9a4(param_1,0x65);
            FUN_1086b880();
            *puVar4 = uVar3;
          }
          return 0;
        }
        if (param_11 != 0) {
          FUN_1086bbe8(param_1,param_11);
        }
        FUN_1081c0a0(param_1,param_3,param_4,iVar1,(int)uVar5,(int)((ulonglong)uVar5 >> 0x20));
        uVar3 = FUN_1086b284(puVar4);
        FUN_1086bb84();
        FUN_1086bac0(3,param_2);
        FUN_1086bac0(2,*(undefined1 *)(param_3 + 0x19));
        FUN_1086bac0(0x20,param_4);
        FUN_1086bac0(0x21,iVar1);
        FUN_1086b9a4(param_1,0x66);
        FUN_1086b880();
        *puVar4 = uVar3;
        return 1;
      }
      if (param_9 != 0) {
        FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0xd2,(uint)*(byte *)(param_3 + 0x19),
                     0xffff0000);
      }
    }
  }
  return 0;
}

