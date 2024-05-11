#include "nfc.h"
#include <stdio.h>
#include "Driver_SPI.h"

#define NFC_FLAG_OFF	1U << 1

#define MSGQUEUE_OBJECTS_NFC 4

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

typedef enum {MI_OK = 0, MI_NOTAGERR, MI_ERR} Status_t;

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

static osThreadId_t id_Th_nfc;
static osMessageQueueId_t id_MsgQueue_nfc;
static osTimerId_t tim_id_auto_off;

//Os
int init_Th_nfc(void);
osMessageQueueId_t get_id_MsgQueue_nfc(void);
static int Init_MsgQueue_nfc(void);
static void callback_spi(uint32_t event);
static void Timer_Callback(void);
static void Init_Tims (void);

//
static void			Init(void);
static Status_t	Check(uint8_t* id);
//
static uint8_t	RC522_SPI_Transfer(uint8_t data);
static void			Write_Reg(uint8_t addr, uint8_t val);
static uint8_t	Read_Reg(uint8_t addr);
static void			SetBitMask(uint8_t reg, uint8_t mask);
static void			ClearBitMask(uint8_t reg, uint8_t mask);
static void			AntennaOn(void);
static void			Reset(void);
static Status_t	Request(uint8_t reqMode, uint8_t* TagType);
static Status_t	ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint16_t *backLen);
static Status_t	Anticoll(uint8_t *serNum);
static void			CalulateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData);
static void			Halt(void);
//Thread
static void Th_nfc(void *arg);

//Os
int init_Th_nfc(void){
	id_Th_nfc = osThreadNew(Th_nfc, NULL, NULL);
	if(id_Th_nfc == NULL)
		return(-1);
	return(Init_MsgQueue_nfc());
}

osThreadId_t get_id_Th_nfc(void){
	return id_Th_nfc;
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


static void Timer_Callback(void){
	osThreadFlagsSet(tim_id_auto_off, NFC_FLAG_OFF);
}


static void Init_Tims (void){
	uint32_t exec = 1U;
  tim_id_auto_off = osTimerNew((osTimerFunc_t)&Timer_Callback, osTimerOnce, &exec, NULL);
}

//
static uint8_t RC522_SPI_Transfer(uint8_t data){
	uint32_t flags;
	uint8_t rx_data;
	int error;
	
	error = SPIdrv->Transfer(&data, &rx_data, 1);
	flags = osThreadFlagsWait(0xFF, osFlagsWaitAny, osWaitForever);
	if((error != 0) || (flags != 0x01)){ // kkk 
		printf("Error: %d \tFlags: %d \tDataTX: %x \tDataRX: %x \n", error, flags, data, rx_data);
	}
	
	return rx_data;
}

static void Init(void){
	static GPIO_InitTypeDef GPIO_InitStruct;
	Init_Tims(); //osTimers
	/*CS*/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);

	/*Reset*/ //o lo ponemos a 3v3
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); 
	
	SPIdrv->Initialize(callback_spi);
  SPIdrv->PowerControl(ARM_POWER_FULL);
  SPIdrv->Control(ARM_SPI_MODE_MASTER |
									ARM_SPI_CPOL0_CPHA0 | 
									ARM_SPI_MSB_LSB | 
									ARM_SPI_DATA_BITS (8), 10000000);

	Reset();
	
	//Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
	Write_Reg(TModeReg, 0x8D);		//Tauto=1; f(Timer) = 6.78MHz/TPreScaler
	Write_Reg(TPrescalerReg, 0x3E);	//TModeReg[3..0] + TPrescalerReg
	Write_Reg(TReloadRegL, 30);					 
	Write_Reg(TReloadRegH, 0);
	
	Write_Reg(RFCfgReg, 0x70); // 48dB gain
	
	Write_Reg(TxAutoReg, 0x40);		// force 100% ASK modulation
	Write_Reg(ModeReg, 0x3D);		// CRC Initial value 0x6363

	AntennaOn();
}

static Status_t Check(uint8_t* id){
	Status_t status;
	//Find cards, return card type
	status = Request(PICC_REQIDL, id);	
	if (status == MI_OK) {
		//Card detected
		//Anti-collision, return card serial number 4 bytes
		status = Anticoll(id);	
	}
	Halt();			//Command card into hibernation 

	return status;
}

//
static void			Write_Reg(uint8_t addr, uint8_t val){
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);	// CS LOW
	RC522_SPI_Transfer((addr<<1)&0x7E);										//send ADDR
	RC522_SPI_Transfer(val); 															//send data
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);	// CS HIGH
}

static uint8_t	Read_Reg(uint8_t addr){
	uint8_t val;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);	// CS LOW
	RC522_SPI_Transfer(((addr<<1)&0x7E) | 0x80);
	val = RC522_SPI_Transfer(0x00);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);	// CS HIGH
	return val;	
}

static void			SetBitMask(uint8_t reg, uint8_t mask){
		Write_Reg(reg, Read_Reg(reg) | mask);	// set bit mask
}

static void			ClearBitMask(uint8_t reg, uint8_t mask){
		Write_Reg(reg, Read_Reg(reg) &(~mask));	// clear bit mask
} 

static void			AntennaOn(void){
	uint8_t temp;
	
	temp = Read_Reg(TxControlReg);
	if(!(temp & 0x03)){
		SetBitMask(TxControlReg, 0x03);
	}
}

static void			Reset(void){
	Write_Reg(CommandReg, PCD_RESETPHASE);
}

static Status_t	Request(uint8_t reqMode, uint8_t* TagType){
	Status_t status;	
	uint16_t backBits;			 // The received data bits

	Write_Reg(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]
	
	TagType[0] = reqMode;
	status = ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if((status != MI_OK) ||(backBits != 0x10)){
		status = MI_ERR;
	}		
	return status;
}

static Status_t	ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint16_t *backLen){
	Status_t status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;
	uint8_t n;
	uint16_t i;

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
	 
	Write_Reg(CommIEnReg, irqEn|0x80);	// Interrupt request
	ClearBitMask(CommIrqReg, 0x80);			// Clear all interrupt request bit
	SetBitMask(FIFOLevelReg, 0x80);			// FlushBuffer=1, FIFO Initialization
		
	Write_Reg(CommandReg, PCD_IDLE);	// NO action; Cancel the current command

	// Writing data to the FIFO
	for(i=0; i<sendLen; i++){	 
		Write_Reg(FIFODataReg, sendData[i]);		
	}

	// Execute the command
	Write_Reg(CommandReg, command);
	if(command == PCD_TRANSCEIVE){		
		SetBitMask(BitFramingReg, 0x80);		// StartSend=1,transmission of data starts
	}	 
		
	// Waiting to receive data to complete
	i = 4700;	// i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms 
	do {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = Read_Reg(CommIrqReg);

		i--;
	}while((i!=0) && !(n&0x01) && !(n&waitIRq));

	ClearBitMask(BitFramingReg, 0x80);			//StartSend=0
	
	if(i != 0){		
		if(!(Read_Reg(ErrorReg) & 0x1B)){	//BufferOvfl Collerr CRCErr ProtecolErr
			status = MI_OK;
			if(n & irqEn & 0x01){	 
				status = MI_NOTAGERR;
			}
			if(command == PCD_TRANSCEIVE){
				n = Read_Reg(FIFOLevelReg);
				lastBits = Read_Reg(ControlReg) & 0x07;
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
					backData[i] = Read_Reg(FIFODataReg);		
				}
			}
		}
		else status = MI_ERR;
	}
	
	return status;
}

static Status_t	Anticoll(uint8_t *serNum){
	Status_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	uint16_t unLen;
		
	Write_Reg(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]
 
	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

	if(status == MI_OK){ //Check card serial number
		for(i=0; i<4; i++){	 
			serNumCheck ^= serNum[i];
		}
		if(serNumCheck != serNum[i]) status = MI_ERR;
	}
	return status;
} 

static void			CalulateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData){
	uint8_t i, n;

	ClearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
	SetBitMask(FIFOLevelReg, 0x80);			//Clear the FIFO pointer

	//Writing data to the FIFO
	for(i=0; i<len; i++){	 
		Write_Reg(FIFODataReg, *(pIndata+i));	 
	}
	Write_Reg(CommandReg, PCD_CALCCRC);

	//Wait CRC calculation is complete
	i = 0xFF;
	do{
		n = Read_Reg(DivIrqReg);
		i--;
	}while((i!=0) && !(n&0x04));			//CRCIrq = 1

	//Read CRC calculation result
	pOutData[0] = Read_Reg(CRCResultRegL);
	pOutData[1] = Read_Reg(CRCResultRegH);
}

static void			Halt(void){
	uint16_t unLen;
	uint8_t buff[4]; 

	buff[0] = PICC_HALT;
	buff[1] = 0;
	CalulateCRC(buff, 2, &buff[2]);
 
	ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}

//Thread
static void Th_nfc(void *arg){
	uint32_t flags;
	MSGQUEUE_OBJ_NFC msg;
	Init();

	while(1){
		osThreadFlagsWait(NFC_FLAG_ON, osFlagsWaitAny, osWaitForever);
		osTimerStart(tim_id_auto_off, NFC_TIMEOUT_MS);

		while((Check(msg.sNum) != MI_OK) && !(flags & NFC_FLAG_OFF)){
			flags = osThreadFlagsGet();
		}
		
		if(!(flags & NFC_FLAG_OFF)){
			osMessageQueuePut(id_MsgQueue_nfc, &msg, 0U, 0U);
		}
		
		flags = 0x00;
	}
}

