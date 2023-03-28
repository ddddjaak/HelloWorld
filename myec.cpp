#include "include_h.h"
#include <QLibrary>

#pragma comment(lib,"WinIox64.lib")    // For 64bit

BYTE Old_SSTDeviceID;
BYTE Old_DeviceID;

MyEC::MyEC(QWidget *parent, unsigned char ec_addr_port, unsigned char ec_data_port) : QWidget(parent), EC_ADDR_PORT(ec_addr_port), EC_DATA_PORT(ec_data_port)
{
    this->IsInitFail = 0;
    this->My_Init();
}

MyEC::~MyEC()
{
    ShutdownWinIo();
    delete this;
}

void MyEC:: My_Init()
{
    if(!InitializeWinIo())
    {
        //qDebug() << "Fail";
        ShutdownWinIo();        

        this->IsInitFail = 1;
    }
    else {
        //qDebug() <<"Success";
    }
}

unsigned char MyEC:: ECRamReadExt_Direct(unsigned short Addr)
{
    DWORD data = 0x00;

    SetPortVal(EC_ADDR_PORT, 0x2E, 1);
    SetPortVal(EC_DATA_PORT, 0x11, 1);
    SetPortVal(EC_ADDR_PORT, 0x2F, 1);
    SetPortVal(EC_DATA_PORT, Addr >> 8, 1);     //High byte

    //qDebug() << "H = " << (Addr >> 8);

    SetPortVal(EC_ADDR_PORT, 0x2E, 1);
    SetPortVal(EC_DATA_PORT, 0x10, 1);
    SetPortVal(EC_ADDR_PORT, 0x2F, 1);
    SetPortVal(EC_DATA_PORT, Addr & 0xFF, 1);   //Low byte

    //qDebug() << "L =" << (Addr & 0xFF);

    SetPortVal(EC_ADDR_PORT, 0x2E, 1);
    SetPortVal(EC_DATA_PORT, 0x12, 1);
    SetPortVal(EC_ADDR_PORT, 0x2F, 1);
    GetPortVal(EC_DATA_PORT, &data, 1);

    //qDebug() << data;

    return(data);
}

void MyEC:: ECRamWriteExt_Direct(unsigned short Addr, BYTE data)
{
    DWORD data1;
    data1 = data;
    SetPortVal(EC_ADDR_PORT, 0x2E, 1);
    SetPortVal(EC_DATA_PORT, 0x11, 1);
    SetPortVal(EC_ADDR_PORT, 0x2F, 1);
    SetPortVal(EC_DATA_PORT, Addr >> 8, 1);    // High byte

    SetPortVal(EC_ADDR_PORT, 0x2E, 1);
    SetPortVal(EC_DATA_PORT, 0x10, 1);
    SetPortVal(EC_ADDR_PORT, 0x2F, 1);
    SetPortVal(EC_DATA_PORT, Addr & 0xFF, 1);  // Low byte

    SetPortVal(EC_ADDR_PORT, 0x2E, 1);
    SetPortVal(EC_DATA_PORT, 0x12, 1);
    SetPortVal(EC_ADDR_PORT, 0x2F, 1);
    SetPortVal(EC_DATA_PORT, data1, 1);
}

void MyEC:: SelectLDN(BYTE data)
{
    SetPortVal(EC_ADDR_PORT, 0x07, 1);
    SetPortVal(EC_DATA_PORT, data, 1);
}

unsigned char MyEC:: ReadPNPCFG(BYTE index)
{
    DWORD data;
    SetPortVal(EC_ADDR_PORT, index, 1);
    GetPortVal(EC_DATA_PORT, &data, 1);

    return data;
}

unsigned char MyEC:: WritePNPCFG(BYTE index, BYTE data)
{
    SetPortVal(EC_ADDR_PORT, index, 1);
    SetPortVal(EC_DATA_PORT, data, 1);
}

BYTE MyEC:: Read_EFlash_1Byte(BYTE Addr_H, BYTE Addr_M, BYTE Addr_L)
{
    BYTE data;
    this->ECRamWriteExt_Direct(ECINDAR3, 0x40);     // select e-flash
    this->ECRamWriteExt_Direct(ECINDAR2, Addr_H);   // select Addr_H
    this->ECRamWriteExt_Direct(ECINDAR1, Addr_M);   // select Addr_M
    this->ECRamWriteExt_Direct(ECINDAR0, Addr_L);   // select Addr_L

    // Don't konw why can't get the data directly, but after writing a data to ECINDDR, the data will be refreshed.
    this->ECRamWriteExt_Direct(ECINDDR, 0xFF);
    data = this->ECRamReadExt_Direct(ECINDDR);      // Read data
    this->ECRamWriteExt_Direct(ECINDAR3, 0x00);     // select SPI

    return data;
}

QByteArray MyEC:: Read_EFlash_256Byte(BYTE Addr_H, BYTE Addr_M, BYTE Addr_L)
{
    QByteArray data_list;

    this->ECRamWriteExt_Direct(ECINDAR3, 0x40);     // select e-flash
    this->ECRamWriteExt_Direct(ECINDAR2, Addr_H);   // select Addr_H
    this->ECRamWriteExt_Direct(ECINDAR1, Addr_M);   // select Addr_M

    for(int i = 0; i <= 255; i++)
    {
        this->ECRamWriteExt_Direct(ECINDAR0, Addr_L + i);   // select Addr_L

        // Don't konw why can't get the data directly, but after writing a data to ECINDDR, the data will be refreshed.
        this->ECRamWriteExt_Direct(ECINDDR, 0xFF);
        data_list += this->ECRamReadExt_Direct(ECINDDR);      // Read data
    }

    this->ECRamWriteExt_Direct(ECINDAR3, 0x00);     // select S

    return data_list;
}

QByteArray MyEC:: Read_SPI_64K(BYTE Addr_H, BYTE Addr_M, BYTE Addr_L)
{
    BYTE temp;
    QByteArray data_list;

    this->ECRamWriteExt_Direct(ECINDAR3, 0x0F);     // select e-flash
    //Enter follow mode
    this->ECRamWriteExt_Direct(ECINDAR2, 0xFF);
    this->ECRamWriteExt_Direct(ECINDAR1, 0xFE);
    this->ECRamWriteExt_Direct(ECINDDR, 0x00);
    //Send SPI command
    this->ECRamWriteExt_Direct(ECINDAR1, 0xFD);
    this->ECRamWriteExt_Direct(ECINDDR, SPICmd_ReadStatus);

    while (1)
    {
        qDebug() << "111";
        temp = this->ECRamReadExt_Direct(ECINDDR);
        if((temp & SPIStatus_BUSY) == 0x00)
        {
            break;
        }
        else
        {
            qDebug() << temp;
        }
    }

    this->ECRamWriteExt_Direct(ECINDAR1, 0xFD);
    this->ECRamWriteExt_Direct(ECINDDR, SPICmd_HighSpeedRead);
    for(WORD index = 0x00; index < 256; index++)
    {
        this->ECRamWriteExt_Direct(ECINDDR, 0xFF);
        data_list += this->ECRamReadExt_Direct(ECINDDR);
    }

    //Exit follow mode
    this->ECRamWriteExt_Direct(ECINDAR3, 0x00);
    this->ECRamWriteExt_Direct(ECINDAR2, 0x00);

    //Enter follow mode
    this->ECRamWriteExt_Direct(ECINDAR3, 0x0F);
    this->ECRamWriteExt_Direct(ECINDAR2, 0xFF);
    this->ECRamWriteExt_Direct(ECINDAR1, 0xFE);
    this->ECRamWriteExt_Direct(ECINDDR, 0x00);

    //Send SPI Command
    this->ECRamWriteExt_Direct(ECINDAR1, 0xFD);
    this->ECRamWriteExt_Direct(ECINDDR, SPICmd_ReadStatus);

    while (1)
    {
        qDebug() << "333";
        temp = this->ECRamReadExt_Direct(ECINDDR);
        if((temp & SPIStatus_BUSY) == 0x00)
        {
            break;
        }
        else
        {
            qDebug() << temp;
        }
    }

    //Exit follow mode
    this->ECRamWriteExt_Direct(ECINDAR3, 0x00);
    this->ECRamWriteExt_Direct(ECINDAR2, 0x00);

    qDebug() << "444";

    return data_list;
}

UINT32 MyEC:: Wait_PM_IBE(WORD CmdPort)
{
    UINT32 index;
    DWORD temp = 0x00;

    for(index = 0; index < KBC_TIME_OUT; index++)
    {
        GetPortVal(CmdPort, &temp, 1);
        if((temp & KBC_IBF) == 0x00)
        {
            return ERROR_SUCCESS;
        }
    }

    return ERROR_NOT_READY;
}

UINT32 MyEC:: Wait_PM_OBF(WORD CmdPort)
{
    UINT32 index;
    DWORD temp = 0x00;

    for(index = 0; index < KBC_TIME_OUT; index++)
    {
        GetPortVal(CmdPort, &temp, 1);
        if((temp & KBC_OBF) == 0x01)
        {
            return ERROR_SUCCESS;
        }
    }

    return ERROR_NOT_READY;
}

UINT32 MyEC:: Send_Cmd_by_PM(WORD CmdPort, BYTE Cmd)
{
    UINT32 status;
    status =  Wait_PM_IBE(CmdPort);

    if(status)
    {
        return status;
    }

    SetPortVal(CmdPort, Cmd, 1);
    return ERROR_SUCCESS;
}

UINT32 MyEC:: Send_Data_by_PM(WORD CmdPort, WORD DataPort, BYTE Data)
{
    UINT32 status;
    status = Wait_PM_IBE(CmdPort);

    if(status)
    {
        return status;
    }

    SetPortVal(DataPort, Data, 1);
    return ERROR_SUCCESS;
}

UINT32 MyEC:: Read_Data_from_PM(WORD CmdPort, WORD DataPort, BYTE * Data)
{
    UINT32 status;
    DWORD data;

    status =  Wait_PM_OBF(CmdPort);

    if(status)
    {
        return status;
    }

    GetPortVal(DataPort, &data, 1);
    *Data = data;
    return ERROR_SUCCESS;
}

UINT32 MyEC:: ReadITEECRAM(BYTE addr, BYTE * Data)
{
    UINT32 status;
    status =  this->Send_Cmd_by_PM(EC_EXTRA_CMD_PORT, 0x80);
    if(status)
    {
        return status;
    }

    status = this->Send_Data_by_PM(EC_EXTRA_CMD_PORT, EC_EXTRA_DATA_PORT, addr);
    if(status)
    {
        return status;
    }

    return this->Read_Data_from_PM(EC_EXTRA_CMD_PORT, EC_EXTRA_DATA_PORT, Data);
}

UINT32 MyEC:: WriteITEECRAM(BYTE addr, BYTE value)
{
    UINT32 status;
    status = this->Send_Cmd_by_PM(EC_EXTRA_CMD_PORT, 0x81);
    if(status)
    {
        return status;
    }

    status = this->Send_Data_by_PM(EC_EXTRA_CMD_PORT, EC_EXTRA_DATA_PORT, addr);
    if(status)
    {
        return status;
    }

    return this->Send_Data_by_PM(EC_EXTRA_CMD_PORT, EC_EXTRA_DATA_PORT, value);
}

UINT8 MyEC:: WaitProtoclTransEnd(void)
{
    UINT32 status;
    BYTE    Data;

    do{
        status = ReadITEECRAM(ACPI_SMBUS.SMB_STS_Addr, &Data);

        if(!status && Data)
        {
            break;
        }
    }while(true);

    return Data;
}

bool MyEC:: WriteCmdToECProtocl(BYTE Protocl, BYTE reg_cmd, BYTE slave_address)
{
    WriteITEECRAM(ACPI_SMBUS.SMB_STS_Addr, 0x00);
    WriteITEECRAM(ACPI_SMBUS.SMB_SLAVE_Addr, slave_address);
    WriteITEECRAM(ACPI_SMBUS.SMB_CMD_Addr, reg_cmd);
    WriteITEECRAM(ACPI_SMBUS.SMB_PRTC_Addr, Protocl);

    if(WaitProtoclTransEnd() == SMB_STA_OK)
    {
        return true;
    }

    return false;
}

void MyEC:: WriteDataToECBuffer(BYTE * datptr, BYTE length)
{
    BYTE index;

    for(index = 0; index < length; index++)
    {
        WriteITEECRAM(ACPI_SMBUS.SMB_DATA_Addr + index, datptr[index]);
    }

    WriteITEECRAM(ACPI_SMBUS.SMB_BCNT_Addr, length);
}

bool MyEC:: ReadIICRegister(BYTE I2CSlaveAddr, BYTE registerAddr, BYTE ReadLength, BYTE * Data)
{
    UINT32 stauts;
    BYTE Buffer[65], index, Length;
    bool rev;
    BYTE Data8;

    rev = WriteCmdToECProtocl(ACPI_SMBUS_PRO_READ_BLOCK, registerAddr, I2CSlaveAddr);
    if(!rev)
    {
        return rev;
    }

    stauts = ReadITEECRAM(ACPI_SMBUS.SMB_BCNT_Addr, &Length);
    if(stauts)
    {
        return false;
    }

    Length = (Length > 32)? 32 : Length;
    Length = (Length == 0)? ReadLength : Length;


    for(index = 0; index < Length; index++)
    {
        stauts = ReadITEECRAM(ACPI_SMBUS.SMB_DATA_Addr + index, &Data8);
        Buffer[index] = Data8;
    }

    for(index = 0; index < (ReadLength < Length? ReadLength : Length); index++, Data++)
    {
        *Data = Buffer[index];
    }

    return rev;
}

bool MyEC:: WriteIICRegister(BYTE I2CSlaveAddr, BYTE registerAddr, BYTE WriteLength, BYTE * Data)
{
    bool Rev;
    BYTE Protocol;

    WriteDataToECBuffer(Data, WriteLength);

    switch(WriteLength)
    {
        case 1:
            Protocol = ACPI_SMBUS_PRO_WRITE_BYTE;
            break;

        case 2:
            Protocol = ACPI_SMBUS_PRO_WRITE_WORD;
            break;

        default:
            Protocol = ACPI_SMBUS_PRO_WRITE_BLOCK;
            break;
    }

    Rev = WriteCmdToECProtocl(Protocol, registerAddr, I2CSlaveAddr);

    return Rev;
}

bool MyEC:: fourCC_Command(UINT32 fourCC, BYTE Length, BYTE * Buffer)
{
    UINT32 Data32;

    if(!WriteIICRegister(DefAddr1, REG_DATA1, Length, Buffer))
    {
        return false;
    }

    if(!WriteIICRegister(DefAddr1, REG_CMD1, 4, (UINT8 *)&fourCC))
    {
        return false;
    }

    do
    {
        if(ReadIICRegister(DefAddr1, REG_CMD1, 4, (UINT8 *)&Data32))
        {
            if(Data32 == 0x00)
            {
                return true;
            }

            if(Data32 == nCMD)
            {
                return false;
            }
        }

    }while(true);

    return false;
}

void MyEC:: EnterSPIControl(void)
{
    this->Send_Cmd_by_PM(EC_66_CMD_PORT, 0xDC);
}

void MyEC:: ExitExitSPIControl(void)
{
    this->Send_Cmd_by_PM(EC_66_CMD_PORT, 0xFC);
}

void MyEC:: EnterFollowMode(void)
{
    this->Send_Cmd_by_PM(EC_66_CMD_PORT, 0x01);
}

void MyEC:: ExitFollowMode(void)
{
    this->Send_Cmd_by_PM(EC_66_CMD_PORT, 0x05);
}

void MyEC:: SendSPICommand(BYTE Cmd)
{
    this->Send_Cmd_by_PM(EC_66_CMD_PORT, 0x02);
    this->Send_Cmd_by_PM(EC_66_CMD_PORT, Cmd);
}

BYTE MyEC:: ReadByteFromSPI(void)
{
    BYTE data;
    this->Send_Cmd_by_PM(EC_66_CMD_PORT, 0x04);
    this->Read_Data_from_PM(EC_66_CMD_PORT, EC_62_DATA_PORT, &data);
    return data;
}

void MyEC:: SendSPIByte(BYTE index)
{
    this->Send_Cmd_by_PM(EC_66_CMD_PORT, 0x03);
    this->Send_Cmd_by_PM(EC_66_CMD_PORT, index);
}

void MyEC:: WaitSPIFree(void)
{
    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_ReadStatus);

    while(1)
    {
        if((this->ReadByteFromSPI() & 0x01) == 0x00)
        {
            break;
        }
    }

    this->ExitFollowMode();
}

void MyEC:: ReadSPIDeviceID_CmdAB(BYTE * IDArray)
{
    BYTE index;
    this->WaitSPIFree();
    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_RDID);
    this->SendSPIByte(0x00);
    this->SendSPIByte(0x00);
    this->SendSPIByte(0x00);

    for(index = 0; index < 4; index++)
    {
        IDArray[index] = this->ReadByteFromSPI();
    }

    if(IDArray[0] == SSTID)
    {
        Old_SSTDeviceID = 1;
        Old_DeviceID = 0;
    }
    else
    {
        Old_SSTDeviceID = 0;
        Old_DeviceID = 1;
    }

    this->ExitFollowMode();
}

void MyEC:: ReadSPIDeviceID(BYTE * IDArray)
{
    BYTE index = 0x00;

    this->WaitSPIFree();
    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_DeviceID);

    for(index = 0x00; index < 4; index++)
    {
        IDArray[index] = this->ReadByteFromSPI();
    }

    if(IDArray[0] == 0x00)
    {
        this->ReadSPIDeviceID_CmdAB(IDArray);
    }

    this->ExitFollowMode();
}

void MyEC:: SPIUnlockAll(BYTE spideviceid)
{
    this->WaitSPIFree();
    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_WREN);
    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_ReadStatus);

    while(1)
    {
        if((this->ReadByteFromSPI() & 0x02) != 0x00)
        {
            break;
        }
    }

    if(spideviceid == SSTID)
    {
        this->EnterFollowMode();
        this->SendSPICommand(SPICmd_EWSR);
    }

    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_WRSR);
    this->SendSPIByte(0x00);
    this->WaitSPIFree();
}

void MyEC:: SPIWriteEnable(BYTE spideviceid)
{
    this->WaitSPIFree();
    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_WREN);

    if(spideviceid == SSTID)
    {
        this->EnterFollowMode();
        this->SendSPICommand(SPICmd_EWSR);
    }

    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_ReadStatus);

    while(1)
    {
        if((this->ReadByteFromSPI() & 0x03) == 0x02)
        {
            break;
        }
    }

    this->ExitFollowMode();
}

void MyEC:: SPIWriteDisable()
{
    this->WaitSPIFree();
    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_WRDI);

    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_ReadStatus);

    while(1)
    {
        if((this->ReadByteFromSPI() & 0x02) == 0x00)
        {
            break;
        }
    }

    this->ExitFollowMode();
}

void MyEC:: SPIBlockErase64KByte(BYTE addr2, BYTE addr1 ,BYTE addr0, BYTE spideviceid)
{
    BYTE counter;

    if(Old_SSTDeviceID == 1)
    {
        this->SPIWriteEnable(spideviceid);
        this->EnterFollowMode();
        this->SendSPICommand(0x52);
        this->SendSPIByte(addr2);
        this->SendSPIByte(addr1);
        this->SendSPIByte(addr0);
        this->ExitFollowMode();
        this->WaitSPIFree();

        this->SPIWriteEnable(spideviceid);
        this->EnterFollowMode();
        this->SendSPICommand(0x52);
        this->SendSPIByte(addr2);
        this->SendSPIByte(addr1 + 0x80);
        this->SendSPIByte(addr0);
        this->ExitFollowMode();
        this->WaitSPIFree();
    }
    else
    {
        for(counter = 0; counter < 64; counter++)
        {
            this->SPIWriteEnable(spideviceid);
            this->EnterFollowMode();
            this->SendSPICommand(SPICmd_01KBByteE);
            this->SendSPIByte(addr2);
            this->SendSPIByte(addr1 + counter << 2);
            this->SendSPIByte(addr0);
            this->ExitFollowMode();
            this->WaitSPIFree();
        }

        if(Old_DeviceID == 0x01)
        {
            this->SPIWriteEnable(spideviceid);
            this->EnterFollowMode();
            this->SendSPICommand(SPICmd_64KBByteE);
            this->SendSPIByte(addr2);
            this->SendSPIByte(addr1 + 0x80);
            this->SendSPIByte(addr0);
            this->ExitFollowMode();
            this->WaitSPIFree();
        }
    }
}

bool MyEC:: SPIVerifyErase64KByte(BYTE addr2, BYTE addr1 ,BYTE addr0, BYTE spideviceid)
{
    this->SPIWriteDisable();
    this->WaitSPIFree();
    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_HighSpeedRead);
    this->SendSPIByte(addr2);
    this->SendSPIByte(addr1);
    this->SendSPIByte(addr0);
    this->SendSPIByte(0x00);        // fast read dummy byte

    WORD counter2 = 0x00, counter = 0x00;

    for(counter2 = 0x00; counter2 < 2; counter2++)
    {
        for(counter = 0x00; counter < 0x8000; counter++)
        {
            if(this->ReadByteFromSPI() != 0xFF)
            {
                this->WaitSPIFree();
                return false;
            }
        }
    }

    return true;
}

void MyEC:: AAIProgram64KByte(BYTE addr2, BYTE addr1, BYTE addr0, BYTE * array1, BYTE * array2, BYTE spideviceid)
{
    this->SPIWriteEnable(spideviceid);
    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_AAIProgram);
    this->SendSPIByte(addr2);
    this->SendSPIByte(addr1);
    this->SendSPIByte(addr0);

    for(WORD counter2 = 0x00; counter2 < 2; counter2++)
    {
        for(WORD counter = 0x00; counter < 0x8000; counter++)
        {
            if(counter2 == 0x00)
            {
                this->SendSPIByte(array1[counter]);
            }
            else
            {
                this->SendSPIByte(array2[counter]);
            }

            this->WaitSPIFree();
            this->EnterFollowMode();
            this->SendSPICommand(SPICmd_AAIProgram);
        }
    }

    this->SPIWriteDisable();
    this->WaitSPIFree();
}

void MyEC:: AAIWordProgram64KByte(BYTE addr2, BYTE addr1, BYTE addr0, BYTE * array1, BYTE * array2, BYTE spideviceid, QProgressBar * progressBar)
{
    int temp = (*progressBar).value() + 1;
    this->SPIWriteEnable(spideviceid);
    this->EnterFollowMode();
    this->SendSPICommand(SPICmd_AAIWordProgram);
    this->SendSPIByte(addr2);
    this->SendSPIByte(addr1);
    this->SendSPIByte(addr0);

    BYTE index = 0x00;

    for(WORD counter2 = 0x00; counter2 < 2; counter2++)
    {
        for(WORD counter = 0x00; counter < 0x8000; counter++)
        {
            if(counter2 == 0x00)
            {
                this->SendSPIByte(array1[counter]);
            }
            else
            {
                this->SendSPIByte(array2[counter]);
            }

            index++;
            if(index == 0x02)
            {
                index = 0x00;
                this->WaitSPIFree();
                this->EnterFollowMode();
                this->SendSPICommand(SPICmd_AAIWordProgram);
            }

            (*progressBar).setValue(temp + ((float)(counter + counter2 * 0x8000) / 0x20000) * 100);
        }
    }

    this->SPIWriteDisable();
    this->WaitSPIFree();
}

void MyEC:: AAIProgram64KByte_PP32(BYTE addr2, BYTE addr1, BYTE addr0, BYTE * array1, BYTE * array2, BYTE spideviceid)
{
    for(WORD counter2 = 0x00; counter2 < 0x100; counter2++)
    {
        for(WORD counter3 = 0x00; counter3 < 0x08; counter3++)
        {
            this->SPIWriteEnable(spideviceid);
            this->EnterFollowMode();
            this->SendSPICommand(SPICmd_BYTEProgram);
            this->SendSPIByte(addr2);
            this->SendSPIByte(addr1 + (BYTE)counter2);
            this->SendSPIByte(addr0 + (BYTE)counter3 * 0x20);

            if(counter2 < 0x80)
            {
                for(WORD counter = 0x00; counter < 0x20; counter++)
                {
                    this->SendSPIByte(array1[counter + (counter2 * 256) + (counter3 * 32)]);
                }
            }
            else
            {
                for(WORD counter = 0x00; counter < 0x20; counter++)
                {
                    this->SendSPIByte(array2[counter + ((counter2 - 0x80) * 256) + (counter3 * 32)]);
                }
            }
        }

        this->WaitSPIFree();
    }

    this->SPIWriteDisable();
}

void MyEC:: ByteProgram64KByte(BYTE addr2, BYTE addr1, BYTE addr0, BYTE * array1, BYTE * array2, BYTE spideviceid)
{
    for(WORD counter2 = 0x00; counter2 < 0x100; counter2++)
    {
        this->SPIWriteEnable(spideviceid);
        this->EnterFollowMode();
        this->SendSPICommand(SPICmd_BYTEProgram);
        this->SendSPIByte(addr2);
        this->SendSPIByte(addr1 + (BYTE)counter2);
        this->SendSPIByte(addr0);

        if(counter2 < 0x80)
        {
            for(WORD counter = 0x00; counter < 0x100; counter++)
            {
                this->SendSPIByte(array1[counter + (counter2 * 256)]);
            }
        }
        else
        {
            for(WORD counter = 0x00; counter < 0x100; counter++)
            {
                this->SendSPIByte(array2[counter + ((counter2 - 80) * 256)]);
            }
        }

        this->WaitSPIFree();
    }

    this->SPIWriteDisable();
}

void MyEC:: ByteProgram64KByte_PP32(BYTE addr2, BYTE addr1, BYTE addr0, BYTE * array1, BYTE * array2, BYTE spideviceid)
{
    for(WORD counter2 = 0x00; counter2 < 0x100; counter2++)
    {
        for(WORD counter3 = 0x00; counter3 < 0x08; counter3++)
        {
            this->SPIWriteEnable(spideviceid);
            this->EnterFollowMode();
            this->SendSPICommand(SPICmd_BYTEProgram);
            this->SendSPIByte(addr2);
            this->SendSPIByte(addr1 + (BYTE)counter2);
            this->SendSPIByte(addr0 + (BYTE)counter3 * 0x20);

            if(counter2 < 0x80)
            {
                for(WORD counter = 0x00; counter < 0x20; counter++)
                {
                    this->SendSPIByte(array1[counter + (counter2 * 256) + (counter3 * 32)]);
                }
            }
            else
            {
                for(WORD counter = 0x00; counter < 0x20; counter++)
                {
                    this->SendSPIByte(array2[counter + ((counter2 - 80) * 256) + (counter3 * 32)]);
                }
            }
        }

        this->WaitSPIFree();
    }

    this->SPIWriteDisable();
}
