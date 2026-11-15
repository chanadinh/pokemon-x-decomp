/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b95a0(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  undefined4 *puVar3;
  char local_1c [8];
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    puVar3 = (undefined4 *)FUN_1086b91c(0x40);
    DAT_10b11be0 = *puVar3;
    DAT_10b11be4 = puVar3[1];
    DAT_10b11be8 = puVar3[2];
    DAT_10b11bec = puVar3[3];
    DAT_10b11bf0 = puVar3[4];
    cVar1 = FUN_1086b91c(6);
    local_1c[0] = '\0';
    if ((cVar1 != '\x1f') &&
       (iVar2 = FUN_1081cb44(param_2,param_3,cVar1,&DAT_10b11be0,local_1c), iVar2 != 0)) {
      iVar2 = FUN_108658d4(0x53,1);
      if (iVar2 == 0) {
        return;
      }
      iVar2 = FUN_10869f7c(param_2,3,param_3);
      FUN_1084d2d4(iVar2 + 4,2,0x464);
      FUN_1084d29c(iVar2 + 4,param_3);
      FUN_1084d29c(iVar2 + 4,cVar1);
      FUN_10867b48(param_2,iVar2);
      return;
    }
    FUN_108658d4(0x42,1);
    if (local_1c[0] != '\0') {
      FUN_108658d4(0x50,1);
      return;
    }
  }
  return;
}

