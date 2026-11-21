/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083ea0c(int param_1,undefined4 param_2,undefined2 param_3)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
  if (DAT_108fcb84 != 0x146f) {
    DAT_108fcb84 = 0x146f;
  }
  puVar2 = *(undefined4 **)(param_1 + 400);
  uVar1 = (**(code **)(*(int *)*puVar2 + 0x24))();
  uVar1 = FUN_10876384(uVar1,param_2);
  puVar2[0x9e] = uVar1;
  *(undefined2 *)(puVar2 + 0x9f) = param_3;
  puVar2[0xf] = 0;
  return;
}

