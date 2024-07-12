#ifndef UTILS_H
#define UTILS_H
#include <glm/glm.hpp>
enum ValueType {
  STR,
  DEX,
  CON,
  INT,
  WIS,
  CHA,
  HP, // 生命值
  MP, // 法力
  // 物理类型
  CUT,
  BASH,
  THRUST,
  // 四大元素
  FIRE,
  ICE,
  THUNDER,
  ELECTRIC,
  // 自然
  POISON,
  ACID,
  LIGHT,
  SHADOW,
  // 超自然
  POS,
  NEG,
  MIND,
  // 动作
  STD,
  // 标准动作，表示豁免就是每次做出标准动作进行豁免，表示消耗就是消耗一个标准动作进行解除
  BONUS, // 附赠动作
  REACT, // 反应动作
  // 特殊
  DISTANCE,   // 移动距离
  DARKSIGHT,  // 黑暗视距
  TURN,       // 回合开始进行豁免
  ROUND,      // 每轮自己回合开始进行豁免
  HURT,       // 每次受伤进行豁免
  NONE,       // 占位初始值。
  LAST_FIX,   // 固定值
  LAST_LEVEL, // 施法者等级
  LAST_ATTR,  // 主属性相关
};
enum MulState {
  T, // 肯定
  F, // 否定
  Z, // 高阻，特殊情形
  X, // 任意，一般情况
};
enum SchoolType {
  DEADTH,     // 死亡领域
  PROTECTION, // 防护领域
  EVOCATION,  // 塑能领域
  ILLUSION,   // 幻术领域
};
enum ObjectType {
  // creature
  SELF,
  ENEMY,
  FRIEND,
  TEAMATE, // 可招募的队友
  // item
  WEAPON,
  ARMOR,
  HELMIT,
  RING,
  NECKLACE,
  L_HAND,
  R_HAND,
  BOOT,
  OTHERS, // 杂物，比如家具之类的东西，钱币也行
  // skill
  SKILL_MOVE,    // 跳跃
  SKILL_ATTACK,  // 主手攻击
  SKILL_DEFENSE, // 盾牌防御
  // spell
  SPELL_MOVE,    // 迷踪步
  SPELL_ATTACK,  // 解除敌人增益魔法算作攻击类。
  SPELL_BUFF,    // 增益魔法
  SPELL_DEBUFF,  // 减益魔法
  SPELL_DEFENSE, // 防护魔法，比如法师护甲
  SPELL_DISPELL, // 解除魔法，用来解除状态的一类魔法
  // terrain
  TRAP,
  FLOOR,
  WALL,
  DOOR,
  WINDOW,
  UP_STAIRS,
  // 只能上楼梯，不能下楼梯，举个例子，一段完整的上下楼梯由下半截和上半截组成，你只能从楼梯进行移动，爬了半截楼梯发现没有后半截了，你可以进行跳跃攀爬之类的完成后半截。但是如果没有那半截楼梯，你不能跳跃或者攀爬。比较抽象，但是矮人要塞也这么干了
  DOWN_STAIRS,
  // 只能下楼梯，怎么理解呢，直接掉下去不能从这里爬上来，下去后头顶上一个半截楼梯。
  UD_STAIRS, // 上下楼梯
};
enum SelectType {
  POINT,
  CIRCLE,
  CRONE,
  CHAIN,
  LINE,
};
enum StatusEffect {
  VALUE,
  DISABLE, // 禁止操作
  NO_STD_ACT,
  NO_BONUS_ACT,
  NO_REACT,
  HALF_DISTANCE,
  NO_DISTANCE,
  NO_MOVE_ACT,
  HALF_DAMAGE,
  NO_DAMAGE,
};
class Dice {
  glm::ivec2 value;
  // 如果骰子大小是1,那么不要用随即数生成来计算大小，因为那样会用循环。如果骰子数量是负数那么总结果s是负数
public:
  Dice() {}
  Dice(glm::ivec2 dice) : value(dice) {}
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