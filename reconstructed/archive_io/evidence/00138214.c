/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_00138214(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 local_1c [2];
  undefined4 local_14;
  undefined4 local_10;
  
  local_10 = 0;
  FUN_00127ccc(param_1,param_2,local_1c);
  FUN_00127c5c(param_1,local_14,local_1c[0],param_3,&local_10);
  return local_10;
}

