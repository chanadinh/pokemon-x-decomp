/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bc500(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  
  cVar1 = FUN_1086b91c(2);
  if (((cVar1 != '\x1f') && (iVar3 = FUN_1084bf24(param_2,cVar1), iVar3 == 0)) &&
     (iVar3 = FUN_1084af30(param_2,cVar1), iVar3 == 0)) {
    uVar4 = FUN_108544a0(param_2,cVar1);
    iVar3 = FUN_10869f7c(param_2,4,cVar1);
    *(char *)(iVar3 + 6) = cVar1;
    uVar2 = FUN_1084d2f4(uVar4,0x10);
    *(undefined2 *)(iVar3 + 4) = uVar2;
    FUN_1084d2d4(iVar3 + 8,2,0x183);
    FUN_1084d29c(iVar3 + 8,cVar1);
    FUN_10867b48(param_2,iVar3);
    return;
  }
  return;
}

