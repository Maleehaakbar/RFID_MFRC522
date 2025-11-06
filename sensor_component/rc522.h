#ifndef RC522_H
#define RC522_H

#include <stdint.h>

#define CommandReg      (uint8_t)0x01  //done
#define ComIEnReg       (uint8_t)0x02
#define DivIEnReg       (uint8_t)0x03
#define ComIrqReg       (uint8_t)0x04
#define DivIrqReg       (uint8_t)0x05   
#define ErrorReg        (uint8_t)0x06  //imp TODO
#define Status1Reg      (uint8_t)0x07 //imp  TODO
#define Status2Reg      (uint8_t)0x08
#define FIFODataReg     (uint8_t)0x09 //imp TODO
#define FIFOLevelReg    (uint8_t)0x0A //imp TODO
#define WaterLevelReg   (uint8_t)0x0B //imp TODO
#define ControlReg      (uint8_t)0x0C //imp TODO
#define BitFramingReg   (uint8_t)0x0D //imp
#define CollReg         (uint8_t)0x0E

#define ModeReg        (uint8_t)0x11    
#define TxModeReg      (uint8_t)0x12 //done
#define RxModeReg      (uint8_t)0x13 //done
#define TxControlReg   (uint8_t)0x14  //done
#define TxASKReg       (uint8_t)0x15
#define TxSelReg      (uint8_t)0x16   
#define RxSelReg      (uint8_t)0x17
#define RxThresholdReg (uint8_t)0x18
#define DemodReg       (uint8_t)0x19
#define MfTxReg        (uint8_t)0x1C
#define MfRxReg        (uint8_t)0x1D
#define SerialSpeedReg (uint8_t)0x1F

#define CRCResultRegH   (uint8_t)0x21 //CRC contains 2 bytes,shows MSB LSB of CRC result
#define CRCResultRegL   (uint8_t)0x22
#define ModWidthReg     (uint8_t)0x24
#define RFCfgReg       (uint8_t)0x26
#define GsNReg         (uint8_t)0x27  //define conductance of n-driver
#define CWGsPReg       (uint8_t)0x28  //define conductance of p-driver
#define ModGsPReg      (uint8_t)0x29  //define conductance of p-driver
#define TModeReg       (uint8_t)0x2A
#define TPrescalerReg  (uint8_t)0x2B
#define TReloadRegH    (uint8_t)0x2C
#define TReloadRegL    (uint8_t)0x2D
#define TCounterValueRegH (uint8_t)0x2E
#define TCounterValueRegL (uint8_t)0x2F

#define TESTSEL1Reg   (uint8_t)0x31
#define TESTSEL2Reg   (uint8_t)0x32
#define TESTPINEnReg  (uint8_t)0x33
#define TESTPINValueReg (uint8_t)0x34    
#define TESTBUSReg    (uint8_t)0x35
#define AUTOTestReg   (uint8_t)0x36
#define VersionReg    (uint8_t)0x37
#define AnalogTestReg (uint8_t)0x38
#define TestDAC1Reg   (uint8_t)0x39
#define TestDAC2Reg   (uint8_t)0x3A
#define TestADCReg    (uint8_t)0x3B

#define WRITE_REG(value)        (value << 1)& 0x7F
#define READ_REG(value)         (value<<1)| 0x80

/*rcv*/
#define rcv_off  1
#define rcv_on   0

/*command*/
#define MEM_COMMAND        0x1
#define RANDOM_ID_COMMAND  0x2



typedef struct configRF{
    uint8_t rcv;
    uint8_t command;
    uint8_t Tx2CW;            //enable modulation of energy carrier
    uint8_t Tx1RFEnable;      //eanble the antenna pin TX2
    uint8_t Tx2RFEnable;     //eanble the antenna pin TX2
    uint8_t Force100ASK;    //forces a 100 % ASK modulation
    uint8_t TxCRCEnable;
    uint8_t TxSpeed;        //data rate for transmission via antenna
    uint8_t RxCRCEnable;
    uint8_t RxSpeed;
    uint8_t Rx_multiple;
    uint8_t driver_sel;    //default 1 (miller pulse encoded), MFIN and MFOUT are unused
    uint8_t TauRcv;
}configRF;


#define IDLE  0x0
#define MEM   0x1
#define RANDOM_ID 0x2
#define CALC_CRC 0x3
#define TRANSMIT 0x4
#define NO_CMD_CHANGE 0x7
#define RECEIVE 0x8
#define TRANSCEIVE 0xC
#define RESERVED 0xD
#define MFAuthent 0xE
#define SOFT_RESET 0xF

#define ENABLE  1
#define DISABLE 0
#define RESET   DISABLE

/*Tx/RxSpeed*/
typedef enum {
    TX_RATE_106_KBD = 0,
    TX_RATE_212_KBD = 1,
    TX_RATE_424_KBD = 2,
    TX_RATE_848_KBD =3,

}tx_rx_speed;

#define BIT_NO_0  0
#define BIT_NO_1  1
#define BIT_NO_2  2 
#define BIT_NO_3  3
#define BIT_NO_4  4
#define BIT_NO_5  5
#define BIT_NO_6  6
#define BIT_NO_7  7     


//txwaitRF bit , RxWait bit ,MfTxReg

void RFID_config(configRF *config_RF);
void RFID_transceive(configRF *config_RF);
#endif // RC522_H