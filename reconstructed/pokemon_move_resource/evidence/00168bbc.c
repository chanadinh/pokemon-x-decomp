/* Unreviewed Ghidra pseudocode obtained through live REA MCP; types require assembly review. */

void move_resource_get(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = DAT_005d188c;
  iVar2 = move_cache_lookup(DAT_005d1890,param_1,DAT_005d188c);
  if (iVar2 == 0) {
    move_resource_load(uVar1,param_1);
    move_cache_store(DAT_005d1890,uVar1);
  }
  move_resource_field(DAT_005d188c,param_2);
  return;
}

