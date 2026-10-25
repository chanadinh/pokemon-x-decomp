/* Unreviewed Ghidra pseudocode obtained through live REA MCP; types require assembly review. */

ulonglong move_resource_field(int param_1,uint param_2)

{
  char cVar1;
  uint uVar2;
  
  switch(param_2) {
  case 0:
    uVar2 = (uint)**(byte **)(param_1 + 8);
    goto LAB_001763c4;
  case 1:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 1));
  case 2:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 2));
  case 3:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 3));
  case 4:
    uVar2 = (uint)*(byte *)(*(int *)(param_1 + 8) + 4);
    if (uVar2 == 0x65) {
      uVar2 = 100;
    }
    return CONCAT44(param_2,uVar2);
  case 5:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 5));
  case 6:
    return CONCAT44(param_2,(int)*(char *)(*(int *)(param_1 + 8) + 6));
  case 7:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0xe));
  case 8:
    return CONCAT44(param_2,(uint)(*(byte *)(*(int *)(param_1 + 8) + 7) >> 4));
  case 9:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 7)) & 0xffffffff0000000f;
  case 10:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0xf));
  case 0xb:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 8));
  case 0xc:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 10));
  case 0xd:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0xb));
  case 0xe:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0xc));
  case 0xf:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0xd));
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x16:
  case 0x17:
  case 0x18:
    return CONCAT44(param_2 + 5,(uint)*(byte *)(*(int *)(param_1 + 8) + param_2 + 5));
  case 0x13:
  case 0x14:
  case 0x15:
    return CONCAT44(param_2 + 5,(int)*(char *)(*(int *)(param_1 + 8) + param_2 + 5));
  case 0x19:
    cVar1 = *(char *)(*(int *)(param_1 + 8) + 0x12);
    break;
  case 0x1a:
    cVar1 = *(char *)(*(int *)(param_1 + 8) + 0x13);
    break;
  case 0x1b:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x14));
  case 0x1c:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 0x10));
  default:
    param_2 = 0;
    goto LAB_00176414;
  case 0x1e:
    cVar1 = *(char *)(*(int *)(param_1 + 8) + 0x12);
    goto joined_r0x0017651c;
  case 0x1f:
    cVar1 = *(char *)(*(int *)(param_1 + 8) + 0x13);
joined_r0x0017651c:
    if (cVar1 < 0) {
      return CONCAT44(param_2,-(int)cVar1);
    }
    goto LAB_00176414;
  case 0x20:
    if (*(char *)(*(int *)(param_1 + 8) + 4) != 'e') goto LAB_00176414;
    uVar2 = 1;
    goto LAB_001763c4;
  }
  uVar2 = (uint)cVar1;
  if (0 < (int)uVar2) {
LAB_001763c4:
    return CONCAT44(param_2,uVar2);
  }
LAB_00176414:
  return (ulonglong)param_2 << 0x20;
}

