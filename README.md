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

测试OPAMP功能
1. 电压跟随


模拟电子秤应用
1. 使用2个模拟输入作为差分输入，模拟电阻桥信号
2. 差分放大