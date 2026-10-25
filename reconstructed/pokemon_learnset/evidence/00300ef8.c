/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined8 _static_::anon_segment_0_200ef8(uint *param_1,uint *param_2,uint param_3,byte param_4)

{
  uint *puVar1;
  uint *puVar2;
  uint *puVar3;
  byte *pbVar4;
  byte bVar5;
  undefined2 uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  byte in_r12;
  uint uVar14;
  uint uVar15;
  bool bVar16;
  bool bVar17;
  
  if (3 < param_3) {
    uVar14 = (uint)param_1 & 3;
    puVar3 = param_1;
    uVar15 = uVar14;
    if (uVar14 != 0) {
      bVar5 = (byte)*param_2;
      puVar3 = (uint *)((int)param_2 + 1);
      if (uVar14 < 3) {
        puVar3 = (uint *)((int)param_2 + 2);
        uVar15 = (uint)*(byte *)((int)param_2 + 1);
      }
      *(byte *)param_1 = bVar5;
      param_2 = puVar3;
      if (uVar14 < 2) {
        param_2 = (uint *)((int)puVar3 + 1);
        bVar5 = (byte)*puVar3;
      }
      puVar2 = (uint *)((int)param_1 + 1);
      if (uVar14 < 3) {
        puVar2 = (uint *)((int)param_1 + 2);
        *(byte *)((int)param_1 + 1) = (byte)uVar15;
      }
      param_3 = (param_3 + uVar14) - 4;
      puVar3 = puVar2;
      if (uVar14 < 2) {
        puVar3 = (uint *)((int)puVar2 + 1);
        *(byte *)puVar2 = bVar5;
      }
    }
    uVar14 = (uint)param_2 & 3;
    if (uVar14 == 0) {
      uVar15 = param_3 - 0x20;
      uVar14 = 0;
      if (0x1f < param_3) {
        uVar14 = *param_2;
        uVar7 = param_2[1];
        uVar8 = param_2[2];
        uVar9 = param_2[3];
        HintPreloadData(param_2 + 0x14);
        uVar10 = param_2[4];
        uVar11 = param_2[5];
        uVar12 = param_2[6];
        uVar13 = param_2[7];
        param_2 = param_2 + 8;
        while( true ) {
          *puVar3 = uVar14;
          puVar3[1] = uVar7;
          puVar3[2] = uVar8;
          puVar3[3] = uVar9;
          bVar16 = 0x1f < uVar15;
          uVar15 = uVar15 - 0x20;
          puVar3[4] = uVar10;
          puVar3[5] = uVar11;
          puVar3[6] = uVar12;
          puVar3[7] = uVar13;
          puVar3 = puVar3 + 8;
          if (bVar16) {
            uVar14 = *param_2;
            uVar7 = param_2[1];
            uVar8 = param_2[2];
            uVar9 = param_2[3];
            param_2 = param_2 + 4;
          }
          HintPreloadData(param_2 + 0x10);
          if (!bVar16) break;
          uVar10 = *param_2;
          uVar11 = param_2[1];
          uVar12 = param_2[2];
          uVar13 = param_2[3];
          param_2 = param_2 + 4;
        }
      }
      if ((bool)((byte)(uVar15 >> 4) & 1)) {
        uVar14 = *param_2;
        uVar7 = param_2[1];
        uVar8 = param_2[2];
        uVar9 = param_2[3];
        param_2 = param_2 + 4;
        *puVar3 = uVar14;
        puVar3[1] = uVar7;
        puVar3[2] = uVar8;
        puVar3[3] = uVar9;
        puVar3 = puVar3 + 4;
      }
      if ((int)(uVar15 << 0x1c) < 0) {
        uVar14 = *param_2;
        uVar7 = param_2[1];
        param_2 = param_2 + 2;
        *puVar3 = uVar14;
        puVar3[1] = uVar7;
        puVar3 = puVar3 + 2;
      }
      puVar1 = puVar3;
      puVar2 = param_2;
      if ((bool)((byte)(uVar15 >> 2) & 1)) {
        puVar2 = param_2 + 1;
        uVar14 = *param_2;
        puVar1 = puVar3 + 1;
        *puVar3 = uVar14;
      }
      uVar6 = (undefined2)uVar14;
      if ((uVar15 & 3) == 0) {
        return CONCAT44(puVar2,puVar1);
      }
      bVar17 = (bool)((byte)(uVar15 >> 1) & 1);
      uVar15 = uVar15 << 0x1f;
      bVar16 = (int)uVar15 < 0;
      puVar3 = puVar2;
      if (bVar17) {
        puVar3 = (uint *)((int)puVar2 + 2);
        uVar6 = (undefined2)*puVar2;
      }
      puVar2 = puVar3;
      if (bVar16) {
        puVar2 = (uint *)((int)puVar3 + 1);
        uVar15 = (uint)(byte)*puVar3;
      }
      puVar3 = puVar1;
      if (bVar17) {
        puVar3 = (uint *)((int)puVar1 + 2);
        *(undefined2 *)puVar1 = uVar6;
      }
      puVar1 = puVar3;
      if (bVar16) {
        puVar1 = (uint *)((int)puVar3 + 1);
        *(byte *)puVar3 = (byte)uVar15;
      }
      return CONCAT44(puVar2,puVar1);
    }
    while( true ) {
      in_r12 = (byte)uVar15;
      if (param_3 < 8) break;
      puVar2 = param_2 + 1;
      uVar14 = *param_2;
      param_2 = param_2 + 2;
      uVar15 = *puVar2;
      *puVar3 = uVar14;
      puVar3[1] = uVar15;
      puVar3 = puVar3 + 2;
      param_3 = param_3 - 8;
    }
    param_3 = param_3 - 4;
    param_1 = puVar3;
    if (-1 < (int)param_3) {
      uVar14 = *param_2;
      param_1 = puVar3 + 1;
      *puVar3 = uVar14;
      param_2 = param_2 + 1;
    }
    param_4 = (byte)uVar14;
  }
  bVar17 = (bool)((byte)(param_3 >> 1) & 1);
  param_3 = param_3 << 0x1f;
  bVar16 = (int)param_3 < 0;
  if (bVar17) {
    pbVar4 = (byte *)((int)param_2 + 1);
    param_4 = (byte)*param_2;
    param_2 = (uint *)((int)param_2 + 2);
    in_r12 = *pbVar4;
  }
  puVar3 = param_2;
  if (bVar16) {
    puVar3 = (uint *)((int)param_2 + 1);
    param_3 = (uint)(byte)*param_2;
  }
  if (bVar17) {
    pbVar4 = (byte *)((int)param_1 + 1);
    *(byte *)param_1 = param_4;
    param_1 = (uint *)((int)param_1 + 2);
    *pbVar4 = in_r12;
  }
  puVar2 = param_1;
  if (bVar16) {
    puVar2 = (uint *)((int)param_1 + 1);
    *(byte *)param_1 = (byte)param_3;
  }
  return CONCAT44(puVar3,puVar2);
}

