/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b176c(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  uint *puVar4;
  
  uVar2 = FUN_1084be3c(1);
  uVar3 = FUN_1084bfc0(param_3);
  FUN_1084bcb0(param_1,param_2,param_3,param_4,uVar3,10,uVar2,0xa8);
  uVar2 = FUN_108544a0(param_2,param_3);
  puVar4 = (uint *)FUN_10869f7c(param_2,0x27,param_3);
  cVar1 = FUN_1084bb2c(uVar2,3);
  *(char *)(puVar4 + 1) = (char)param_3;
  *(undefined1 *)((int)puVar4 + 5) = 3;
  *(char *)((int)puVar4 + 6) = cVar1 + '\x01';
  *puVar4 = *puVar4 | 0x1000000;
  FUN_10867b48(param_2,puVar4);
  return;
}

