#ifndef MYEC_H
#define MYEC_H

#include <QWidget>
#include <windows.h>
#include <QProgressBar>

#define KBC_TIME_OUT 80000
#define KBC_OBF     0x01
#define KBC_IBF     0x02
#define EC_66_CMD_PORT      0x66
#define EC_62_DATA_PORT     0x62
#define EC_EXTRA_CMD_PORT   0x6C
#define EC_EXTRA_DATA_PORT  0x68

// TI PD Config
#define DefAddr1    0x40
#define DefAddr2    0x48
#define DefAddr3    0x42
#define REG_DATA1   0x09
#define REG_CMD1    0x08
#define CONV_4CC_TO_WORD(_A_, _B_, _C_, _D_)    ((_D_ << 24) | (_C_ << 16) | (_B_ << 8) | (_A_))
#define nCMD    CONV_4CC_TO_WORD('!', 'C', 'M', 'D')
#define FLrr    CONV_4CC_TO_WORD('F', 'L', 'r', 'r')
#define FLem    CONV_4CC_TO_WORD('F', 'L', 'e', 'm')
#define FLad    CONV_4CC_TO_WORD('F', 'L', 'a', 'd')
#define FLwd    CONV_4CC_TO_WORD('F', 'L', 'w', 'd')
#define FLvy    CONV_4CC_TO_WORD('F', 'L', 'v', 'y')
#define GAID    CONV_4CC_TO_WORD('G', 'A', 'I', 'D')

//SPI
#define SPICmd_WRSR             0x01        //Write Status Register
#define SPICmd_BYTEProgram      0x02        //To program one byte data
#define SPICmd_WRDI             0x04        //Write disable
#define SPICmd_ReadStatus       0x05        //Read Status Register
#define SPICmd_WREN             0x06        //Write enable
#define SPICmd_HighSpeedRead    0x0B        //High-speed read
#define SPICmd_EWSR             0x50        //Enable Write Status Register
#define SPICmd_ChipEraseITE     0x60        //Chip Erase
#define SPICmd_DeviceID         0x9F        //Manufacture ID command
#define SPICmd_RDID             0xAB        //Read ID
#define SPICmd_AAIWordProgram   0xAD        //Auto Address Increment programming (word)
#define SPICmd_AAIProgram       0xAF        //Auto Address Increment programming (byte)
#define SPICmd_01KBByteE        0xD7        //Erase 1 KByte block of memory array
#define SPICmd_64KBByteE        0xD8        //Erase 64 KByte block of memory array

#define SSTID       0xBF
#define WinbondID   0xEF
#define AtmelID     0x9F
#define STID        0x20
#define SpansionID  0x01
#define MXICID      0xC2
#define AMICID      0x37
#define EONID       0x1C
#define ESMTID      0x8C
#define PMCID       0x7F
#define ITEID       0xFF

#define SPIStatus_BUSY  BIT(0)

#define ECINDDR     0x103F
#define ECINDAR3    0x103E
#define ECINDAR2    0x103D
#define ECINDAR1    0x103C
#define ECINDAR0    0x103B

#define SMB_STA_OK      0x80
//#define SMB_Prtc_Addr   0x1C
//#define SMB_STS_Addr    0x1D
//#define SMB_ADDR_Addr   0x1E
//#define SMB_CMD_Addr    0x1F
//#define SMB_DATA_Addr   0x20
//#define SMB_BCNT_Addr   0x40

struct aACPI_SMB{
    BYTE SMB_PRTC_Addr;
    BYTE SMB_STS_Addr;
    BYTE SMB_SLAVE_Addr;
    BYTE SMB_CMD_Addr;
    BYTE SMB_DATA_Addr;
    BYTE SMB_BCNT_Addr;
    BYTE SMB_ALRA_Addr;
    BYTE SMB_ALRA0_Addr;
    BYTE SMB_ALRA1_Addr;
};

#define ACPI_SMBUS_PRO_SEND_BYTE    0x04
#define ACPI_SMBUS_PRO_RECE_BYTE    0x05
#define ACPI_SMBUS_PRO_WRITE_BYTE   0x06
#define ACPI_SMBUS_PRO_READ_BYTE    0x07
#define ACPI_SMBUS_PRO_WRITE_WORD   0x08
#define ACPI_SMBUS_PRO_READ_WORD    0x09
#define ACPI_SMBUS_PRO_WRITE_BLOCK  0x0A
#define ACPI_SMBUS_PRO_READ_BLOCK   0x0B



    void My_Init();
    void ECRamWriteExt_Direct(unsigned short Addr, BYTE data);
    unsigned char ECRamReadExt_Direct(unsigned short Addr);
    void SelectLDN(BYTE data);
    unsigned char ReadPNPCFG(BYTE index);
    unsigned char WritePNPCFG(BYTE index, BYTE data);
    BYTE Read_EFlash_1Byte(BYTE Addr_H, BYTE Addr_M, BYTE Addr_L);
    QByteArray Read_EFlash_256Byte(BYTE Addr_H, BYTE Addr_M, BYTE Addr_L);
    QByteArray Read_SPI_64K(BYTE Addr_H, BYTE Addr_M, BYTE Addr_L);
    UINT32 Wait_PM_IBE(WORD CmdPort);
    UINT32 Wait_PM_OBF(WORD CmdPort);
    UINT32 Send_Cmd_by_PM(WORD CmdPort, BYTE Cmd);
    UINT32 Send_Data_by_PM(WORD CmdPort, WORD DataPort, BYTE Data);
    UINT32 Read_Data_from_PM(WORD CmdPort, WORD DataPort, BYTE * Data);
    UINT32 ReadITEECRAM(BYTE addr, BYTE * Data);
    UINT32 WriteITEECRAM(BYTE addr, BYTE value);
    UINT8 WaitProtoclTransEnd(void);
    bool WriteCmdToECProtocl(BYTE Protocl, BYTE reg_cmd, BYTE slave_address);
    void WriteDataToECBuffer(BYTE * datptr, BYTE length);
    bool ReadIICRegister(BYTE I2CSlaveAddr, BYTE registerAddr, BYTE ReadLength, BYTE * Data);
    bool WriteIICRegister(BYTE I2CSlaveAddr, BYTE registerAddr, BYTE WriteLength, BYTE * Data);
    bool fourCC_Command(UINT32 fourCC, BYTE Length, BYTE * Buffer);

//---------------SPI---------------------
    void EnterSPIControl(void);
    void ExitExitSPIControl(void);
    void EnterFollowMode(void);
    void ExitFollowMode(void);
    void SendSPICommand(BYTE Cmd);
    BYTE ReadByteFromSPI(void);
    void SendSPIByte(BYTE index);
    void WaitSPIFree(void);
    void ReadSPIDeviceID(BYTE * IDArray);
    void ReadSPIDeviceID_CmdAB(BYTE * IDArray);
    void SPIUnlockAll(BYTE spideviceid);
    void SPIWriteEnable(BYTE spideviceid);
    void SPIWriteDisable();
    void SPIBlockErase64KByte(BYTE addr2, BYTE addr1 ,BYTE addr0, BYTE spideviceid);
    bool SPIVerifyErase64KByte(BYTE addr2, BYTE addr1 ,BYTE addr0, BYTE spideviceid);
    void AAIProgram64KByte(BYTE addr2, BYTE addr1, BYTE addr0, BYTE * array1, BYTE * array2, BYTE spideviceid);
    void AAIWordProgram64KByte(BYTE addr2, BYTE addr1, BYTE addr0, BYTE * array1, BYTE * array2, BYTE spideviceid, QProgressBar * progressBar);
    void AAIProgram64KByte_PP32(BYTE addr2, BYTE addr1, BYTE addr0, BYTE * array1, BYTE * array2, BYTE spideviceid);
    void ByteProgram64KByte(BYTE addr2, BYTE addr1, BYTE addr0, BYTE * array1, BYTE * array2, BYTE spideviceid);
    void ByteProgram64KByte_PP32(BYTE addr2, BYTE addr1, BYTE addr0, BYTE * array1, BYTE * array2, BYTE spideviceid);
};

#endif // MYEC_H
