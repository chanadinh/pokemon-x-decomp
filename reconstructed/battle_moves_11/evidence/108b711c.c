/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b711c(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 != param_3) {
    return;
  }
  uVar3 = FUN_1084b8d4(param_2);
  switch(uVar3) {
  default:
    uVar3 = FUN_1084b81c(param_2);
    switch(uVar3) {
    default:
      uVar3 = 0xa1;
      break;
    case 1:
      uVar3 = 0x19e;
      break;
    case 2:
      uVar3 = 0x198;
      break;
    case 3:
      goto switchD_108b7148_caseD_1;
    case 4:
      goto switchD_108b7148_caseD_4;
    case 5:
      uVar3 = 0x1aa;
      break;
    case 6:
      uVar3 = 0x20c;
      break;
    case 7:
      goto switchD_108b7148_caseD_5;
    }
    break;
  case 1:
switchD_108b7148_caseD_1:
    uVar3 = 0x19c;
    break;
  case 2:
    uVar3 = 0x249;
    break;
  case 3:
    uVar3 = 0x55;
    break;
  case 4:
switchD_108b7148_caseD_4:
    uVar3 = 0x38;
    break;
  case 5:
switchD_108b7148_caseD_5:
    uVar3 = 0x3a;
  }
  cVar1 = FUN_1086b91c(0xe);
  if (cVar1 == '\n') {
    FUN_10871bac(param_2,param_3,uVar3);
  }
  FUN_108658d4(0x12,uVar3);
  FUN_108658d4(0xd,cVar1);
  return;
}

