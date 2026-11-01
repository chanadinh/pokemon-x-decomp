/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084d29c(int param_1,undefined4 param_2)

{
  ushort uVar1;
  
  uVar1 = *(ushort *)(param_1 + 2);
  if ((uVar1 & 0x3fff) >> 7 < 8) {
    *(ushort *)(param_1 + 2) =
         (ushort)(((uint)(uVar1 >> 0xe) | (uint)uVar1 << 0x12) + 0x2000000 >> 0x12) |
         (uVar1 >> 0xe) << 0xe;
    *(undefined4 *)((uVar1 >> 5 & 0x1fc) + param_1 + 4) = param_2;
  }
  return;
}

