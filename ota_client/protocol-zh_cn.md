# 设备bootloader和用户客户端之间的协议设计

**由于bootloader和上位机进行通信的过程中，不仅要包含设备本身的状态，还要包括固件包的下载，为此设计一套通信协议是非常有必要的**

## 一. 协议格式

**通信方式为串口/USB-CDC, 在设备端还是上位机端，双方都面向字节流进行协议解析，协议包括控制消息和数据消息，控制消息包括设备对上位机端的检测，上位机对设备的回应，对数据包的请求，应答等等，而数据消息则是具体要传输的固件分包，或者设备已有的固件消息等等**

### 1-1 传输帧头

| 帧头frame_header | 传输类型transport_type | 数据长度data_length | CRC8  |
| :--------------: | :--------------------: | :-----------------: | :---: |
|      2字节       |         1字节          |        4字节        | 1字节 |

```c
struct FRAME_HEAD {
    uint8_t header1;		//0xAB
    uint8_t header2;		//0xCD
    uint8_t transport_type;	//0为控制消息，1为数据消息
    uint32_t data_length;	//数据帧
    uint8_t crc8;			//帧头CRC8
}__attribute__((packed));
```

### 1-2 数据帧

| 序列号SEQ | 消息ID msg_id | 具体的消息体 | 帧尾  | 整包CRC16 |      |
| :-------: | :-----------: | :----------: | :---: | :-------: | :--: |
|   2字节   |     2字节     |    n字节     | 2字节 |   2字节   |      |

```c
struct DATA_HEAD {
    uint16_t SEQ;           //序列号，每发送一帧自增
    uint16_t msg_id;        //消息ID
}__attribute__((packed));

struct DATA_TAIL {
    uint8_t tail1;          //0xEC
    uint8_t tail2;          //0xED
    uint16_t crc16;         //全包CRC16
}__attribute__((packed));
```



## 二. 设备到客户端

### 2.1 控制消息

#### 2-1-1 检测客户端心跳消息(0x001)

| 消息体空 |
| :------: |
|  0字节   |

**当设备上电时，bootloader持续向串口发送该消息，等待客户端回应**

#### 2-1-2 请求新固件文件信息(0x002)

| 消息体空 |
| :------: |
|  0字节   |

#### 2-1-3 请求新固件第n个分包(0x003)

| 分包序号 |
| :------: |
|  4字节   |

```c
struct MSG_REQ_FIRMWARE_INFO {
    uint32_t frimware_seq;
}__attribute__((packed));
```

#### 2-1-4 报告第n个固件分包下载成功(0x004)
| 分包序号 |
| :------: |
|  4字节   |

```c
struct MSG_REQ_FIRMWARE_INFO {
    uint32_t frimware_seq;
}__attribute__((packed));
```

#### 2-1-5 请求当前日期时间(0x005)
| 消息体空 |
| :------: |
|  0字节   |


### 2.2 数据消息

#### 2-2-1  报告当前固件文件信息(0x010)

| 设备名 | 版本  | 更新日期 | 制造商 | 固件描述 | 固件大小 | CRC16 |
| :----: | :---: | :------: | :----: | :------: | :------: | :---: |
| 20字节 | 4字节 |  4字节   | 20字节 |  30字节  |  4字节   | 2字节 |

```c
struct REPORT_CUR_FIRMWARE_INFO {       //0x010
    char device_name[20];       //设备名
    uint32_t version;           //版本信息 n.n.n.n, 每个版本号占据8位,组合成一个四字节数据
    uint32_t update_timestamp;  //更新固件时的时间戳
    char manufacturer[20];      //制造商信息
    char description[30];       //固件描述
    uint32_t size;              //固件大小 单位:Byte
    uint16_t crc16;             //固件整体CRC16
}__attribute__((packed));
```



## 三. 客户端到设备