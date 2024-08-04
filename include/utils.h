#ifndef UTILS_H
#define UTILS_H

enum MulState {
  T, // 肯定
  F, // 否定
  Z, // 高阻，特殊情形
  X, // 任意，一般情况
};

enum Position {
  // 用来定义组建在view里面的相对位置，取代过去的数值定义位置
  LU_CORNER, // left up corner
  LD_CORNER,
  RU_CORNER,
  RD_CORNER,
  L_SIDE,
  R_SIDE,
  U_SIDE,
  D_SIDE,
  CENTER
};
#endif
