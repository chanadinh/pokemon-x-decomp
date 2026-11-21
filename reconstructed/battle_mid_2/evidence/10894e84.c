/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10894e84(int param_1)

{
  char cVar1;
  int iVar2;
  
  thunk_FUN_003356dc(*(undefined4 *)(param_1 + 0xec));
  if (*(char *)(param_1 + 0xf4) == '\0') {
    FUN_10894a7c();
  }
  else if ((*(char *)(param_1 + 0xf4) == '\x01') &&
          ((0x5d < *(byte *)(param_1 + 0xf6) ||
           (iVar2 = thunk_FUN_0049558c(param_1 + 4,*(undefined4 *)(param_1 + 0xe4)), iVar2 != 0))))
  {
    thunk_FUN_0034bc00(param_1 + 4,*(undefined4 *)(param_1 + 0xe4),*(undefined1 *)(param_1 + 0xf6));
    cVar1 = *(char *)(param_1 + 0xf5);
    if (cVar1 == '\0') {
      thunk_FUN_00334174(*(undefined4 *)(param_1 + 0xec));
    }
    else if (cVar1 == '\x01' || cVar1 == '\x02') {
      thunk_FUN_00334048(*(undefined4 *)(param_1 + 0xec));
    }
    *(char *)(param_1 + 0xf4) = cVar1;
    *(undefined1 *)(param_1 + 0xf5) = 3;
  }
  if (*(char *)(param_1 + 0xf4) == '\x02') {
    if (*(char *)(param_1 + 0xd9) == '\x03') {
      FUN_10800394(0,0);
    }
    *(undefined1 *)(param_1 + 0x24) = 1;
  }
  return;
}

