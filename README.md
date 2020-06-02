# scale-upgrade

测试模块：
* GPIO, LED
* USART, 调试输出
* DAC, 电压输出，正弦波发生器，[音频输出]
* OPAMP，差分放大
* ADC，差分输入

DAC > OPAMP > ADC > USART
DAC和OPAMP无法同时使用。


2路差分信号 > 放大 > 采样 > 转换计算重量


模拟电子秤应用
1. 输入一组差分信号（电阻桥信号）；
2. 经过放到后输入ADC（差分信号）。
3. 输出（LCD和串口）差分电压差

数据通信：
