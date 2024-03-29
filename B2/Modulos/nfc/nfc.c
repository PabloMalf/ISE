#include "rc522.h"
#include <stdio.h>
#include "string.h"
#include "Driver_SPI.h"

#define	uchar	unsigned char
#define	uint	unsigned int

// MFRC522 commands. Described in chapter 10 of the datasheet.
#define PCD_IDLE					0x00	// no action, cancels current command execution
#define PCD_AUTHENT				0x0E	// performs the MIFARE standard authentication as a reader
#define PCD_RECEIVE				0x08	// activates the receiver circuits
#define PCD_TRANSMIT			0x04	// transmits data from the FIFO buffer
#define PCD_TRANSCEIVE		0x0C	// transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission
#define PCD_RESETPHASE		0x0F	// resets the MFRC522
#define PCD_CALCCRC				0x03	// activates the CRC coprocessor or performs a self-test

// Commands sent to the PICC.
#define PICC_REQIDL				0x26	// REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
#define PICC_REQALL				0x52	// Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
#define PICC_ANTICOLL			0x93	// Anti collision/Select, Cascade Level 1
#define PICC_SElECTTAG		0x93	// Anti collision/Select, Cascade Level 2
#define PICC_AUTHENT1A		0x60	// Perform authentication with Key A
#define PICC_AUTHENT1B		0x61	// Perform authentication with Key B
#define PICC_READ					0x30	// Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight.
#define PICC_WRITE				0xA0	// Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight.
#define PICC_DECREMENT		0xC0	// Decrements the contents of a block and stores the result in the internal data register.
#define PICC_INCREMENT		0xC1	// Increments the contents of a block and stores the result in the internal data register
#define PICC_RESTORE			0xC2	// Reads the contents of a block into the internal data register.
#define PICC_TRANSFER			0xB0	// Writes the contents of the internal data register to a block.
#define PICC_HALT					0x50	// HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT.

// Success or error code is returned when communication
#define MI_OK							0
#define MI_NOTAGERR				1
#define MI_ERR						2

// MFRC522 registers. Described in chapter 9 of the datasheet.
#define	Reserved00				0x00
#define	CommandReg				0x01
#define	CommIEnReg				0x02
#define	DivlEnReg					0x03
#define	CommIrqReg				0x04
#define	DivIrqReg					0x05
#define	ErrorReg					0x06
#define	Status1Reg				0x07
#define	Status2Reg				0x08
#define	FIFODataReg				0x09
#define	FIFOLevelReg			0x0A
#define	WaterLevelReg			0x0B
#define	ControlReg				0x0C
#define	BitFramingReg			0x0D
#define	CollReg						0x0E
#define	Reserved01				0x0F

#define	Reserved10				0x10
#define	ModeReg						0x11
#define	TxModeReg					0x12
#define	RxModeReg					0x13
#define	TxControlReg			0x14
#define	TxAutoReg					0x15
#define	TxSelReg					0x16
#define	RxSelReg					0x17
#define	RxThresholdReg		0x18
#define	DemodReg					0x19
#define	Reserved11				0x1A
#define	Reserved12				0x1B
#define	MifareReg					0x1C
#define	Reserved13				0x1D
#define	Reserved14				0x1E
#define	SerialSpeedReg		0x1F

#define	Reserved20				0x20
#define	CRCResultRegH			0x21
#define	CRCResultRegL			0x22
#define	Reserved21				0x23
#define	ModWidthReg				0x24
#define	Reserved22				0x25
#define	RFCfgReg					0x26
#define	GsNReg						0x27
#define	CWGsPReg					0x28
#define	ModGsPReg					0x29
#define	TModeReg					0x2A
#define	TPrescalerReg			0x2B
#define	TReloadRegH				0x2C
#define	TReloadRegL				0x2D
#define	TCounterValueRegH	0x2E
#define	TCounterValueRegL	0x2F

#define	Reserved30				0x30
#define	TestSel1Reg				0x31
#define	TestSel2Reg				0x32
#define	TestPinEnReg			0x33
#define	TestPinValueReg		0x34
#define	TestBusReg				0x35
#define	AutoTestReg				0x36
#define	VersionReg				0x37
#define	AnalogTestReg			0x38
#define	TestDAC1Reg				0x39
#define	TestDAC2Reg				0x3A
#define	TestADCReg				0x3B
#define	Reserved31				0x3C
#define	Reserved32				0x3D
#define	Reserved33				0x3E
#define	Reserved34				0x3F

#define MAX_LEN 16 //Maximum length of the array

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

static osThreadId_t id_Th_nfc;
static osMessageQueueId_t id_MsgQueue_nfc;

static int Init_MsgQueue_nfc(void);
static void callback_spi(uint32_t event);

static void Th_nfc(void *arg);

// Mid level Funcionts
uint8_t RC522_SPI_Transfer(uchar data);
void Write_MFRC522(uchar addr, uchar val);
uchar Read_MFRC522(uchar addr);
void SetBitMask(uchar reg, uchar mask);
void ClearBitMask(uchar reg, uchar mask);
void AntennaOn(void);
void AntennaOff(void);
void MFRC522_Reset(void);
uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen);
void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData);

// High Level Functions
void MFRC522_Init(void);
uchar MFRC522_Request(uchar reqMode, uchar *TagType);
uchar MFRC522_Anticoll(uchar *serNum);
uchar MFRC522_SelectTag(uchar *serNum);
uchar MFRC522_Read(uchar blockAddr, uchar *recvData);
uchar MFRC522_Write(uchar blockAddr, uchar *writeData);				
uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum);
void MFRC522_Halt(void);

// Os Functions
int init_Th_nfc(void){
	id_Th_nfc = osThreadNew(Th_nfc, NULL, NULL);
	if(id_Th_nfc == NULL)
		return(-1);
	return(Init_MsgQueue_nfc());
}

osMessageQueueId_t get_id_MsgQueue_nfc(void){
	return id_MsgQueue_nfc;
}

static int Init_MsgQueue_nfc(void){
	id_MsgQueue_nfc = osMessageQueueNew(MSGQUEUE_OBJECTS_NFC, sizeof(MSGQUEUE_OBJ_NFC), NULL);
	if(id_MsgQueue_nfc == NULL)
		return (-1); 
	return(0);
}

static void callback_spi(uint32_t event){
	osThreadFlagsSet(id_Th_nfc, event);
}

// Mid level Functions
uint8_t RC522_SPI_Transfer(uchar data){
	uint32_t flags;
	int error;
	uchar rx_data;
	//HAL_SPI_TransmitReceive(HSPI_INSTANCE, &data, &rx_data, 1, 100);
	error = SPIdrv->Transfer(&data, &rx_data, 1);
	flags = osThreadFlagsWait(0xFF, osFlagsWaitAny, osWaitForever);
	printf("Error: %d \tFlags: %d \tDataTX: %x \tDataRX: %x \n", error, flags, data, rx_data);
	return rx_data;
}

void Write_MFRC522(uchar addr, uchar val){
	HAL_GPIO_WritePin(MFRC522_CS_PORT,MFRC522_CS_PIN,GPIO_PIN_RESET);	// CS LOW
		// even though we are calling transfer frame once, we are really sending
		// two 8-bit frames smooshed together-- sending two 8 bit frames back to back
		// results in a spike in the select line which will jack with transactions
		// - top 8 bits are the address. Per the spec, we shift the address left
		//	 1 bit, clear the LSb, and clear the MSb to indicate a write
		// - bottom 8 bits are the data bits being sent for that address, we send them
	RC522_SPI_Transfer((addr<<1)&0x7E);	
	RC522_SPI_Transfer(val);
	HAL_GPIO_WritePin(MFRC522_CS_PORT, MFRC522_CS_PIN, GPIO_PIN_SET);	// CS HIGH
}

uchar Read_MFRC522(uchar addr){
	uchar val;
	HAL_GPIO_WritePin(MFRC522_CS_PORT, MFRC522_CS_PIN, GPIO_PIN_RESET);	// CS LOW
		// even though we are calling transfer frame once, we are really sending
		// two 8-bit frames smooshed together-- sending two 8 bit frames back to back
		// results in a spike in the select line which will jack with transactions
		// - top 8 bits are the address. Per the spec, we shift the address left
		//	 1 bit, clear the LSb, and set the MSb to indicate a read
		// - bottom 8 bits are all 0s on a read per 8.1.2.1 Table 6
	RC522_SPI_Transfer(((addr<<1)&0x7E) | 0x80);
	val = RC522_SPI_Transfer(0x00);
	HAL_GPIO_WritePin(MFRC522_CS_PORT,MFRC522_CS_PIN,GPIO_PIN_SET);	// CS HIGH
	return val;	
}

void SetBitMask(uchar reg, uchar mask){
		uchar tmp;
		tmp = Read_MFRC522(reg);
		Write_MFRC522(reg, tmp | mask);	// set bit mask
}

void ClearBitMask(uchar reg, uchar mask){
		uchar tmp;
		tmp = Read_MFRC522(reg);
		Write_MFRC522(reg, tmp &(~mask));	// clear bit mask
} 

void AntennaOn(void){
	Read_MFRC522(TxControlReg);
	SetBitMask(TxControlReg, 0x03);
}

void AntennaOff(void){
	ClearBitMask(TxControlReg, 0x03);
}

void MFRC522_Reset(void){
	Write_MFRC522(CommandReg, PCD_RESETPHASE);
}

uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen){
	uchar status = MI_ERR;
	uchar irqEn = 0x00;
	uchar waitIRq = 0x00;
	uchar lastBits;
	uchar n;
	uint i;

	switch(command){
		case PCD_AUTHENT:		// Certification cards close
			irqEn = 0x12;
			waitIRq = 0x10;
		break;
		
		case PCD_TRANSCEIVE:	// Transmit FIFO data
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		default: break;
	}
	 
	Write_MFRC522(CommIEnReg, irqEn|0x80);	// Interrupt request
	ClearBitMask(CommIrqReg, 0x80);			// Clear all interrupt request bit
	SetBitMask(FIFOLevelReg, 0x80);			// FlushBuffer=1, FIFO Initialization
		
	Write_MFRC522(CommandReg, PCD_IDLE);	// NO action; Cancel the current command

	// Writing data to the FIFO
	for(i=0; i<sendLen; i++){	 
		Write_MFRC522(FIFODataReg, sendData[i]);		
	}

	// Execute the command
	Write_MFRC522(CommandReg, command);
	if(command == PCD_TRANSCEIVE){		
		SetBitMask(BitFramingReg, 0x80);		// StartSend=1,transmission of data starts
	}	 
		
	// Waiting to receive data to complete
	i = 2000;	// i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms
	do {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = Read_MFRC522(CommIrqReg);
		i--;
	}while((i!=0) && !(n&0x01) && !(n&waitIRq));

	ClearBitMask(BitFramingReg, 0x80);			//StartSend=0
	
	if(i != 0){		
		if(!(Read_MFRC522(ErrorReg) & 0x1B)){	//BufferOvfl Collerr CRCErr ProtecolErr
			status = MI_OK;
			if(n & irqEn & 0x01){	 
				status = MI_NOTAGERR;
			}
			if(command == PCD_TRANSCEIVE){
				n = Read_MFRC522(FIFOLevelReg);
				lastBits = Read_MFRC522(ControlReg) & 0x07;
				if(lastBits){	 
					*backLen =(n-1)*8 + lastBits;	 
				}
				else{	 
					*backLen = n*8;	 
				}
				if(n == 0){	 
					n = 1;		
				}
				if(n > MAX_LEN){	 
					n = MAX_LEN;	 
				}
				// Reading the received data in FIFO
				for(i=0; i<n; i++){	 
					backData[i] = Read_MFRC522(FIFODataReg);		
				}
			}
		}
		else{	 
			status = MI_ERR;	
		}	
	}
	
	//SetBitMask(ControlReg,0x80);					 //timer stops
	//Write_MFRC522(CommandReg, PCD_IDLE); 
	return status;
}

void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData){
	uchar i, n;

	ClearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
	SetBitMask(FIFOLevelReg, 0x80);			//Clear the FIFO pointer

	//Writing data to the FIFO
	for(i=0; i<len; i++){	 
		Write_MFRC522(FIFODataReg, *(pIndata+i));	 
	}
	Write_MFRC522(CommandReg, PCD_CALCCRC);

	//Wait CRC calculation is complete
	i = 0xFF;
	do{
		n = Read_MFRC522(DivIrqReg);
		i--;
	}while((i!=0) && !(n&0x04));			//CRCIrq = 1

	//Read CRC calculation result
	pOutData[0] = Read_MFRC522(CRCResultRegL);
	pOutData[1] = Read_MFRC522(CRCResultRegH);
}

// High Level Functions
void MFRC522_Init(void){
	SPIdrv->Initialize(callback_spi);
  SPIdrv-> PowerControl(ARM_POWER_FULL);
  SPIdrv-> Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS (8), 20000000);
	
	HAL_GPIO_WritePin(MFRC522_CS_PORT,MFRC522_CS_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(MFRC522_RST_PORT,MFRC522_RST_PIN,GPIO_PIN_SET);
	MFRC522_Reset();
	 	
	//Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
	Write_MFRC522(TModeReg, 0x8D);		//Tauto=1; f(Timer) = 6.78MHz/TPreScaler
	Write_MFRC522(TPrescalerReg, 0x3E);	//TModeReg[3..0] + TPrescalerReg
	Write_MFRC522(TReloadRegL, 30);					 
	Write_MFRC522(TReloadRegH, 0);
	
	Write_MFRC522(TxAutoReg, 0x40);		// force 100% ASK modulation
	Write_MFRC522(ModeReg, 0x3D);		// CRC Initial value 0x6363

	AntennaOn();
}

uchar MFRC522_Request(uchar reqMode, uchar *TagType){
	uchar status;	
	uint backBits;			 // The received data bits

	Write_MFRC522(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]
	
	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if((status != MI_OK) ||(backBits != 0x10)) return MI_ERR;		
	return status;
}

uchar MFRC522_Anticoll(uchar *serNum){
	uchar status;
	uchar i;
	uchar serNumCheck = 0;
	uint unLen;
		
	Write_MFRC522(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]
 
	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

	if(status == MI_OK){ //Check card serial number
		for(i=0; i<4; i++){	 
			serNumCheck ^= serNum[i];
		}
		if(serNumCheck != serNum[i]) return MI_ERR;
	}
	return status;
} 

uchar MFRC522_SelectTag(uchar *serNum){
	uchar i;
	uchar status;
	uchar size;
	uint recvBits;
	uchar buffer[9]; 

	//ClearBitMask(Status2Reg, 0x08);			//MFCrypto1On=0

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for(i=0; i<5; i++){
		buffer[i+2] = *(serNum+i);
	}
	CalulateCRC(buffer, 7, &buffer[7]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
		
	if((status == MI_OK) &&(recvBits == 0x18)){	 
		size = buffer[0]; 
	}
	else{	 
		size = 0;		
	}
	return size;
}

uchar MFRC522_Read(uchar blockAddr, uchar *recvData){
	uchar status;
	uint unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	CalulateCRC(recvData,2, &recvData[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

	if((status != MI_OK) ||(unLen != 0x90))	return MI_ERR;
	return status;
}

uchar MFRC522_Write(uchar blockAddr, uchar *writeData){
	uchar status;
	uint recvBits;
	uchar i;
	uchar buff[18]; 
		
	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	CalulateCRC(buff, 2, &buff[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
	
	if((status != MI_OK) ||(recvBits != 4) ||((buff[0] & 0x0F) != 0x0A)) return MI_ERR;

	if(status == MI_OK){
		for(i=0; i<16; i++){		//Data to the FIFO write 16Byte
			buff[i] = *(writeData+i);	 
		}
		CalulateCRC(buff, 16, &buff[16]);
		status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);	
		if((status != MI_OK) ||(recvBits != 4) ||((buff[0] & 0x0F) != 0x0A)) return MI_ERR; 
	}
	return status;
}

uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum){
	uchar status;
	uint recvBits;
	uchar i;
	uchar buff[12]; 

	//Verify the command block address + sector + password + card serial number
	buff[0] = authMode;
	buff[1] = BlockAddr;
	for(i=0; i<6; i++){		
		buff[i+2] = *(Sectorkey+i);	 
	}
	for(i=0; i<4; i++){		
		buff[i+8] = *(serNum+i);	 
	}
	status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
	
	if((status != MI_OK) ||(!(Read_MFRC522(Status2Reg) & 0x08))) return MI_ERR;
	return status;
}

void MFRC522_Halt(void){
	uint unLen;
	uchar buff[4]; 

	buff[0] = PICC_HALT;
	buff[1] = 0;
	CalulateCRC(buff, 2, &buff[2]);
 
	MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}

//Thread
static void Th_nfc(void *arg){
	MSGQUEUE_OBJ_NFC msg_nfc;
	uint8_t status;
	uint8_t str[MAX_LEN]; // Max_LEN = 16
	uint32_t cnt = 0;
	uint8_t sNum[5];
	
	MFRC522_Init();

	while(1){
		printf("%d\n", cnt);
		cnt++;
		osDelay(1000);
//		status = MFRC522_Request(PICC_REQIDL, str);
//		status = MFRC522_Anticoll(str);
//		memcpy(sNum, str, 5);
//		osDelay(100);
		/*
		if((str[0]==115) && (str[1]==93) && (str[2]==75) && (str[3]==22) && (str[4]==115)){
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
			osDelay(100);
    }
  	else if((str[0]==199) && (str[1]==102) && (str[2]==209) && (str[3]==215) && (str[4]==167)){
    	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
    	 osDelay(2000);
  	 }
  	 else{
    	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
  	 }
		*/
	}
}
