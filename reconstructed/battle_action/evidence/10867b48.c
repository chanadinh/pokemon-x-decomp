/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10867b48(int param_1,uint *param_2)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  
  FUN_10819600();
  puVar3 = (uint *)(param_1 + 0x437c);
  uVar1 = *param_2;
  uVar2 = *puVar3;
  if (((uVar1 & 0x7fffff) >> 0xd <= (uVar2 & 0x7ffff) >> 10) &&
     ((int)param_2 + ((uVar1 & 0x7fffff) >> 0xd) + (-4 - (int)puVar3) == (uVar2 & 0x7ffff) >> 10)) {
    *puVar3 = (uVar2 >> 0x13 | uVar2 << 0xd) + (uVar1 >> 0xd) * -0x800000 >> 0xd |
              (uVar2 >> 0x13) * 0x80000;
    return;
  }
  FUN_10800394(0,0);
  return;
}

