/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10839a0c(int param_1,int param_2)

{
  char cVar1;
  ushort uVar2;
  undefined1 uVar3;
  int iVar4;
  bool bVar5;
  
  if (DAT_108fcb84 != 0x154a) {
    DAT_108fcb84 = 0x154a;
  }
  if (*(char *)(param_1 + 0x494) == '\0') {
    iVar4 = *(int *)(param_1 + 0x6b8);
    if (iVar4 == 0) {
      FUN_1086fd18((undefined1 *)(param_1 + 0x6ca),0xff);
      *(undefined1 *)(param_1 + 0x25d) = 0;
      uVar3 = (undefined1)param_2;
      *(undefined1 *)(param_1 + 0x6c8) = uVar3;
      *(undefined1 *)(param_1 + 0x6c9) = uVar3;
      *(undefined1 *)(param_1 + 0x6ca) = uVar3;
      iVar4 = *(int *)(*(int *)(param_1 + 400) + param_2 * 4 + 0x798);
      *(undefined1 *)(iVar4 + 0x35) = 1;
      if (*(char *)(iVar4 + 0x10) != '\0') {
        *(undefined1 *)(iVar4 + 0x10) = 0;
        *(undefined1 *)(iVar4 + 0x44) = 1;
      }
      FUN_10859464(param_1,0x280);
      *(int *)(param_1 + 0x6b8) = *(int *)(param_1 + 0x6b8) + 1;
    }
    else if (iVar4 == 1) {
      iVar4 = FUN_10858cb8(param_1,0);
      if (iVar4 != 0) {
        FUN_10859464(param_1,0x275);
        FUN_10859464(param_1,0x278);
        *(int *)(param_1 + 0x6b8) = *(int *)(param_1 + 0x6b8) + 1;
      }
    }
    else if (iVar4 == 2) {
      iVar4 = FUN_10858cb8(param_1,0);
      if (iVar4 != 0) {
        FUN_108582a0(*(undefined4 *)(param_1 + 400),param_2);
        return 1;
      }
    }
    else if (iVar4 == 100) {
      return 1;
    }
  }
  else {
    iVar4 = *(int *)(param_1 + 400);
    bVar5 = *(char *)(iVar4 + 0x84d) != '\0';
    cVar1 = '\0';
    if (bVar5) {
      cVar1 = *(char *)(iVar4 + 0x84c);
    }
    if (!bVar5 || cVar1 == '\0') {
      *(undefined1 *)(param_1 + 0x494) = 0;
      uVar2 = *(ushort *)(iVar4 + 0x880);
      *(undefined1 *)(iVar4 + 0x84c) = 0;
      *(undefined2 *)(iVar4 + 0x880) = 0;
      *(undefined1 *)(iVar4 + 0x84d) = 0;
      if (uVar2 < 0x5b) {
        return 1;
      }
      *(undefined4 *)(param_1 + 0x6b8) = 0;
    }
  }
  return 0;
}

