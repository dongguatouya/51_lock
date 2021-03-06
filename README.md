## 智能电子锁

基于51芯片开发的简易六位密码锁

### 1.基本功能介绍

- [x] 能够从键盘中输入密码，并相应地在显示器上显示"*"

-  [x] 能够判断密码是否正确，正确则开锁，错误则输出相应信息
-  [x] 能够实现密码的修改
-  [x] 断电或者单片机复位后能够保存之前的操作，比如密码的修改
-  [x] 在操作错误到达一定次数后能够报警
-  [x] 备用密码我们改为，初始密码，在功能上实用性更强，利用中断按钮清除用户密码，恢复初始密码

### 2.所需硬件资源

- 安装Keil C51
- AT89C51
- 蜂鸣器
- LCD1602液晶显示屏
- 制作矩阵键盘

### 3.硬件整体架构

> 用Word画的硬件示意图位于包含本文件的文件夹

### 4.程序流程图展示

####                     4.1 密码处理函数流程示意图

```mermaid
graph TD
A(密码处理函数)-->B{判断按键}
B--begin被按下-->C[LCD显示pleaseinput]
B--set被按下-->D[LCD显示inputpassword]
C-->E[输入密码]-->G{判断密码是否正确}
G--YES-->F[password right]
G--NO-->H[password wrong]-->I{aa=4?}--yes-->J[报警灯闪烁]
D-->K[输入密码]-->L{判断密码是否正确}
L--NO-->M[LCD显示password]
L--YES-->N[输入新密码]-->O[再次输入]-->a{判断两次输入是否一致}
a--yes-->b[修改成功]
a--no-->c[password wrong]

```

####                          4.2    矩阵键盘工作流程图 

```mermaid
graph TD
A(矩阵按键扫描子函数)-->B[键盘扫描]
B-->C[延时去抖]-->D{是否有键按下}
D--NO-->B
D--YES-->E[查找键值]-->F[输出键值]-->G[等待下次按键]

```

####                    4.3 LCD1602工作流程

```mermaid
graph TD
a(开始)-->b[调用LCD1602_init函数对液晶显示屏进行初始化]-->c[调用24c02函数对总线进行初始化]-->d[D1=1 TMOD=0X01 EA=1 ET0=1 EX0=1 IT0=1 TR0=0 TH0=0X4c TL0=0x00]
d-->e{while==1?}--yes-->f[调用sfj2函数对密码进行处理修改]
f-->e
e--no-->g(结束)
```

####           4.4外部中断函数流程图

```mermaid
graph TD
a(外部中断函数)-->b[选择中断方式 中断号]
b-->c{K1==0?}--yes-->d[显示器关闭]-->e[清除修改后的密码]
e-->f[写入初始密码]-->g[显示器显示最初内容]-->h(返回)
c--no-->h
```



####           4.5定时器中断子函数

```mermaid
graph TD
a(开始)-->c[TH0=0x4c TL0=0x00]
c-->d[bb++]-->e{bb==200?}
e--yes-->f[bb=0 cc=0]-->g(返回)
e--no-->g
```





### 5.外部按钮

复位按钮按下或断电重启后，单片机重新初始化，密码不发生变化，做到断电不丢失。

中断按钮按下后，用户密码清除，恢复初始密码，方便用户忘记密码后，重设密码

### 6.操作密码说明

初始密码和管理员密码都为六位密码080808

### 7.致谢

刘老师给与了我们最大的关心和支持，给我们提供了良好的学习场地和充足的耐心。

还有郑老师，文老师，杨老师珍贵的指导意见。

为了保护各位老师和同学的隐私，不再显示全名

### 关于作者

> 黄同学        
>
> 袁同学    
>
> 王同学