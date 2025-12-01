#include "stepmotor.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "delay.h"



// 电机参数定义
#define STEPS_PER_REV 4096  // 28BYJ-48电机在8步模式下的步数/转(64*64)
#define MOTOR_PIN_1 GPIO_Pin_4
#define MOTOR_PIN_2 GPIO_Pin_5
#define MOTOR_PIN_3 GPIO_Pin_6
#define MOTOR_PIN_4 GPIO_Pin_7
#define MOTOR_PORT GPIOA

// 旋转方向定义
typedef enum {
    MOTOR_DIR_CW = 0,    // 顺时针方向
    MOTOR_DIR_CCW = 1    // 逆时针方向
} MotorDirection;

// 8步序列(半步模式)
const uint8_t stepSequence[8] = {
    0x01,  // 0001 - IN1
    0x03,  // 0011 - IN1+IN2
    0x02,  // 0010 - IN2
    0x06,  // 0110 - IN2+IN3
    0x04,  // 0100 - IN3
    0x0C,  // 1100 - IN3+IN4
    0x08,  // 1000 - IN4
    0x09   // 1001 - IN4+IN1
};

// 全局变量
static int32_t currentPosition = 0;  // 当前绝对位置(步数)
static uint8_t currentStep = 0;      // 当前步序(0-7)

// GPIO初始化(沿用之前的)
void Motor_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = MOTOR_PIN_1 | MOTOR_PIN_2 | MOTOR_PIN_3 | MOTOR_PIN_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_PORT, &GPIO_InitStructure);
}

// 执行单步运动
static void Motor_Step(int8_t direction) {
    // 更新步序索引
    if(direction == 1) {
        currentStep = (currentStep + 1) % 8;  // 正向
    } else {
        currentStep = (currentStep - 1 + 8) % 8;  // 反向
    }
    
    // 设置引脚状态
    GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN_1, (stepSequence[currentStep] & 0x01) ? Bit_SET : Bit_RESET);
    GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN_2, (stepSequence[currentStep] & 0x02) ? Bit_SET : Bit_RESET);
    GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN_3, (stepSequence[currentStep] & 0x04) ? Bit_SET : Bit_RESET);
    GPIO_WriteBit(MOTOR_PORT, MOTOR_PIN_4, (stepSequence[currentStep] & 0x08) ? Bit_SET : Bit_RESET);
    
    // 更新当前位置
    currentPosition += direction;
    
    // 步间延时控制速度(2ms可根据需要调整)
    Delay_ms(2);
}

// 旋转指定角度(带方向控制)
void Motor_Rotate(float angle, int dir) {
    // 计算需要移动的步数
    int32_t stepsToMove = (int32_t)(angle / 360.0 * STEPS_PER_REV);
    
    // 根据方向确定步进方向
    int8_t stepDirection = (dir == MOTOR_DIR_CW) ? 1 : -1;
    
    // 执行步进
    for(int32_t i = 0; i < stepsToMove; i++) {
        Motor_Step(stepDirection);
    }
}

// 旋转到绝对角度(0-360度)
void Motor_RotateTo(float targetAngle) {
    // 计算目标位置
    int32_t targetPosition = (int32_t)(targetAngle / 360.0 * STEPS_PER_REV);
    int32_t stepsToMove = targetPosition - currentPosition;
    
    // 自动选择最短路径
    if(stepsToMove > STEPS_PER_REV/2) {
        stepsToMove -= STEPS_PER_REV;
    } else if(stepsToMove < -STEPS_PER_REV/2) {
        stepsToMove += STEPS_PER_REV;
    }
    
    // 确定方向
    int8_t direction = (stepsToMove > 0) ? 1 : -1;
    stepsToMove = (stepsToMove > 0) ? stepsToMove : -stepsToMove;
    
    // 执行步进
    for(int32_t i = 0; i < stepsToMove; i++) {
        Motor_Step(direction);
    }
}

//// 主函数示例
//int main(void) {
//    // 初始化
//    Motor_GPIO_Init();
//    Delay_Init();
//    
//    while(1) {
//        // 示例1: 顺时针旋转90度
//        Motor_Rotate(90.0, MOTOR_DIR_CW);
//        DelayMs(1000);
//        
//        // 示例2: 逆时针旋转180度
//        Motor_Rotate(180.0, MOTOR_DIR_CCW);
//        DelayMs(1000);
//        
//        // 示例3: 旋转到绝对位置0度
//        Motor_RotateTo(0.0);
//        DelayMs(1000);
//        
//        // 示例4: 旋转到绝对位置270度
//        Motor_RotateTo(270.0);
//        DelayMs(1000);
//    }
//}



