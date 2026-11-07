/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c0f60(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  char cVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 local_1c;
  
  iVar4 = FUN_1086b91c(4);
  if (((iVar4 == param_3) && (iVar4 = FUN_1086b91c(0x47), iVar4 == 0)) &&
     (iVar4 = FUN_10865c48(param_2), iVar4 == 0)) {
    uVar5 = FUN_108544a0(param_2,param_3);
    uVar1 = FUN_1086b91c(3);
    uVar6 = FUN_108544a0(param_2,uVar1);
    cVar2 = FUN_10874420(uVar5,0x12);
    cVar3 = FUN_10874420(uVar6,0x12);
    if ((cVar2 != '\x02') && (cVar3 != '\x02' && cVar2 != cVar3)) {
      FUN_1084c438(7,uVar5,&local_1c);
      FUN_1084ad50(param_2,param_3,7,local_1c,0x1e);
    }
  }
  return;
}

