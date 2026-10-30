/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

ulonglong FUN_00175c78(int param_1,undefined4 param_2)

{
  uint uVar1;
  
  switch(param_2) {
  case 0:
    return CONCAT44(param_2,(uint)**(byte **)(param_1 + 8));
  case 1:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 1));
  case 2:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 2));
  case 3:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 3));
  case 4:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 4));
  case 5:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 5));
  case 6:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 6));
  case 7:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 7));
  case 8:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 8));
  case 9:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 0x22));
  case 10:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 10)) & 0xffffffff00000003;
  case 0xb:
    return CONCAT44(param_2,(*(ushort *)(*(int *)(param_1 + 8) + 10) & 0xf) >> 2);
  case 0xc:
    uVar1 = (uint)*(ushort *)(*(int *)(param_1 + 8) + 10) << 0x1a;
    break;
  case 0xd:
    uVar1 = (uint)*(ushort *)(*(int *)(param_1 + 8) + 10) << 0x18;
    break;
  case 0xe:
    uVar1 = (uint)*(ushort *)(*(int *)(param_1 + 8) + 10) << 0x16;
    break;
  case 0xf:
    uVar1 = (uint)*(ushort *)(*(int *)(param_1 + 8) + 10) << 0x14;
    break;
  case 0x10:
    uVar1 = (uint)*(ushort *)(*(int *)(param_1 + 8) + 10) << 0x13;
    goto LAB_00175f0c;
  case 0x11:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 0xc));
  case 0x12:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 0xe));
  case 0x13:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 0x10));
  case 0x14:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x12));
  case 0x15:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x13));
  case 0x16:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x14));
  case 0x17:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x15));
  case 0x18:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x16));
  case 0x19:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x17));
  case 0x1a:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x18));
  case 0x1b:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x19));
  case 0x1c:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x1a));
  case 0x1d:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x1b));
  case 0x1e:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 0x1c));
  case 0x1f:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 0x1e));
  case 0x20:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x20));
  case 0x21:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x21)) & 0xffffffff0000003f;
  case 0x22:
    uVar1 = (uint)*(byte *)(*(int *)(param_1 + 8) + 0x21) << 0x19;
LAB_00175f0c:
    return CONCAT44(param_2,uVar1 >> 0x1f);
  case 0x23:
    return CONCAT44(param_2,(uint)(*(byte *)(*(int *)(param_1 + 8) + 0x21) >> 7));
  case 0x24:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 9));
  case 0x25:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 0x24));
  case 0x26:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 0x26));
  case 0x27:
    return CONCAT44(param_2,*(undefined4 *)(*(int *)(param_1 + 8) + 0x28));
  case 0x28:
    return CONCAT44(param_2,*(undefined4 *)(*(int *)(param_1 + 8) + 0x2c));
  case 0x29:
    return CONCAT44(param_2,*(undefined4 *)(*(int *)(param_1 + 8) + 0x30));
  case 0x2a:
    return CONCAT44(param_2,*(undefined4 *)(*(int *)(param_1 + 8) + 0x34));
  case 0x2b:
    return CONCAT44(param_2,*(undefined4 *)(*(int *)(param_1 + 8) + 0x38));
  case 0x2c:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(param_1 + 8) + 0x3c));
  case 0x2d:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(param_1 + 8) + 0x3e));
  default:
    return 0;
  }
  return CONCAT44(param_2,uVar1 >> 0x1e);
}

