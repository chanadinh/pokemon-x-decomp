/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bc2c0(undefined4 param_1,undefined4 param_2)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  
  bVar1 = FUN_1086b91c(4);
  uVar5 = (uint)bVar1;
  iVar3 = FUN_1084bf24(param_2,uVar5);
  if (((iVar3 == 0) && (iVar3 = FUN_1084af30(param_2,uVar5), iVar3 == 0)) &&
     (((uVar4 = FUN_1086b91c(4), uVar4 == uVar5 &&
       ((sVar2 = FUN_1086b91c(0x1e), sVar2 == 2 && (iVar3 = FUN_108658d4(0x42,1), iVar3 != 0)))) ||
      ((uVar4 = FUN_1086b91c(4), uVar4 == uVar5 &&
       ((sVar2 = FUN_1086b91c(0x1e), sVar2 == 0xe && (iVar3 = FUN_108658d4(0x42,1), iVar3 != 0))))))
     )) {
    iVar3 = FUN_10869f7c(param_2,0x17,uVar5);
    *(byte *)(iVar3 + 5) = bVar1;
    *(undefined1 *)(iVar3 + 4) = 0x10;
    FUN_10867b48(param_2);
    return;
  }
  return;
}

