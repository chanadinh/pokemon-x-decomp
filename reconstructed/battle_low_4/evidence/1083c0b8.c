/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083c0b8(int param_1,undefined4 param_2)

{
  switch(param_2) {
  case 0:
    *(undefined4 *)(param_1 + 0x734) = 0;
    *(code **)(param_1 + 0x730) = FUN_10842d88;
    *(undefined1 *)(param_1 + 0x72c) = 1;
    return;
  case 1:
    goto switchD_1083c0c4_caseD_1;
  case 2:
    *(undefined1 **)(param_1 + 0x730) = &LAB_1083d21c;
    *(undefined4 *)(param_1 + 0x734) = 0;
    *(undefined1 *)(param_1 + 0x72c) = 0;
    return;
  case 3:
    *(undefined1 **)(param_1 + 0x730) = &LAB_10842e0c;
    *(undefined4 *)(param_1 + 0x734) = 0;
    *(undefined1 *)(param_1 + 0x72c) = 0;
    return;
  case 4:
    *(undefined1 **)(param_1 + 0x730) = &LAB_10842e4c;
    *(undefined4 *)(param_1 + 0x734) = 0;
    *(undefined1 *)(param_1 + 0x72c) = 0;
    return;
  case 5:
    *(code **)(param_1 + 0x730) = FUN_10843bfc;
    *(undefined4 *)(param_1 + 0x734) = 0;
    *(undefined1 *)(param_1 + 0x72c) = 0;
    return;
  case 6:
    *(code **)(param_1 + 0x730) = FUN_10843478;
    *(undefined4 *)(param_1 + 0x734) = 0;
    *(undefined1 *)(param_1 + 0x72c) = 0;
    return;
  case 7:
    *(undefined1 **)(param_1 + 0x730) = &LAB_10843b80;
    *(undefined4 *)(param_1 + 0x734) = 0;
    *(undefined1 *)(param_1 + 0x72c) = 0;
    return;
  case 8:
    if (1 < *(byte *)(param_1 + 0x82c)) {
      return;
    }
switchD_1083c0c4_caseD_1:
    *(code **)(param_1 + 0x730) = FUN_10841948;
    *(undefined4 *)(param_1 + 0x734) = 0;
    *(undefined1 *)(param_1 + 0x72c) = 0;
    return;
  default:
    return;
  }
}

