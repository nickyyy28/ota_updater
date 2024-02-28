# Bootloader启动流程

## 1. 检查Program Flag
### 1.1 检查Program Flag是否为可编程标志
    如果在Flash地址0x08040100中可编程标志为0xA5A5A5A5, 则进入升级状态,进入串口升级模式
    升级完成后进行APP2 CRC32全包校验,校验完成后清除Program Flag,跳转APP2

### 1.2 Program Flag不为可编程状态,进行APP2验证
    进行对APP2和版本信息分区的CRC32验证
## 2. 检查APP2区CRC校验
## 3. 跳转APP
