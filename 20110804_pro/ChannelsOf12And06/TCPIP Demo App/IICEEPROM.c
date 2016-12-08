#include "TCPIP Stack/TCPIP.h"

void IICEEPROM_Init(void)
{
	//here is the I2C setup from the Seeval32 code.
	IIC_SCL_TRIS = 1;	//Configure SCL
						//as Input
	IIC_SDA_TRIS = 1;	//Configure SDA
						//as Input
	SSP2STAT = 0x80;		//Disable SMBus &
						//Slew Rate Control
	SSP2CON1 = 0x28;		//Enable MSSP Master
	SSP2ADD = 0x68;		//Should be 0x18
						//for 100kHz
	SSP2CON2 = 0x00;		//Clear MSSP Conrol Bits
}

/************************************************************************
*     Function Name:    LDByteWriteI2C                                  *
*     Parameters:       EE memory ControlByte, address and data         *
*     Description:      Writes data one byte at a time to I2C EE        *
*                       device. This routine can be used for any I2C    *
*                       EE memory device, which only uses 1 byte of     *
*                       address data as in the 24LC01B/02B/04B/08B/16B. *
*                                                                       *
************************************************************************/

unsigned char LDByteWriteI2C( unsigned char ControlByte, unsigned char address, unsigned char data )
{
	IdleI2C();                          // ensure module is idle
	StartI2C();                         // initiate START condition
	while ( SSP2CON2bits.SEN );          // wait until start condition is over 
	WriteI2C( ControlByte );            // write 1 byte - R/W bit should be 0
	IdleI2C();                          // ensure module is idle
	WriteI2C( address );                 // write address byte to EEPROM
	IdleI2C();                          // ensure module is idle
	WriteI2C ( data );                  // Write data byte to EEPROM
	IdleI2C();                          // ensure module is idle
	StopI2C();                          // send STOP condition
	while ( SSP2CON2bits.PEN );          // wait until stop condition is over 
	while (EEAckPolling(ControlByte));  //Wait for write cycle to complete
	return ( 0 );                       // return with no error
}

/********************************************************************
*     Function Name:    LDByteReadI2C                               *
*     Parameters:       EE memory ControlByte, address, pointer     *
*                                                     				*
*     Description:      Reads data string from I2C EE memory        *
*                       device. This routine can be used for any I2C*
*                       EE memory device, which only uses 1 byte of *
*                       address data as in the 24LC01B/02B/04B/08B. *
*                                                                   *  
********************************************************************/

unsigned char LDByteReadI2C( unsigned char ControlByte, unsigned char address, unsigned char *data )
{
	IdleI2C();                      // ensure module is idle
	StartI2C();                     // initiate START condition
	while ( SSP2CON2bits.SEN );      // wait until start condition is over 
	WriteI2C( ControlByte );        // write 1 byte 
	IdleI2C();                      // ensure module is idle
	WriteI2C( address );            // WRITE word address to EEPROM
	IdleI2C();                      // ensure module is idle
	RestartI2C();                   // generate I2C bus restart condition
	while ( SSP2CON2bits.RSEN );     // wait until re-start condition is over 
	WriteI2C( ControlByte | 0x01 ); // WRITE 1 byte - R/W bit should be 1 for read
	IdleI2C();                      // ensure module is idle
	*data = ReadI2C();				// save byte received
	while ( SSP2CON2bits.RCEN );		// check that receive sequence is over 
	NotAckI2C();                    // send not ACK condition
	while ( SSP2CON2bits.ACKEN );    // wait until ACK sequence is over 
	StopI2C();                      // send STOP condition
	while ( SSP2CON2bits.PEN );      // wait until stop condition is over 
	return ( 0 );                   // return with no error
}

/********************************************************************
*     Function Name:    LDSequentialWriteI2C                              *
*     Parameters:       EE memory ControlByte, address and pointer  *
*     Description:      Writes data string to I2C EE memory         *
*                       device. This routine can be used for any I2C*
*                       EE memory device, which uses 2 bytes of     *
*                       address data as in the 24LC32A/64/128/256.  *
*                                                                   *  
********************************************************************/

unsigned char LDSequentialWriteI2C( unsigned char ControlByte, unsigned char address, unsigned char *wrptr , unsigned char length)
{
	IdleI2C();                      // ensure module is idle
	StartI2C();                     // initiate START condition
	while ( SSP2CON2bits.SEN );      // wait until start condition is over 
	WriteI2C( ControlByte );        // write 1 byte - R/W bit should be 0
	IdleI2C();                      // ensure module is idle
	WriteI2C( address );             // write LowAdd byte to EEPROM
	IdleI2C();                      // ensure module is idle
	putstringI2C ( wrptr, length );         // pointer to data for page write
	IdleI2C();                      // ensure module is idle
	StopI2C();                      // send STOP condition
	while ( SSP2CON2bits.PEN );      // wait until stop condition is over 
	return ( 0 );                   // return with no error
}

/********************************************************************
*     Function Name:    LDSequentialReadI2C                         *
*     Parameters:       EE memory ControlByte, address, pointer and *
*                       length bytes.                               *
*     Description:      Reads data string from I2C EE memory        *
*                       device. This routine can be used for any I2C*
*                       EE memory device, which only uses 1 byte of *
*                       address data as in the 24LC01B/02B/04B/08B. *
*                                                                   *  
********************************************************************/

unsigned char LDSequentialReadI2C( unsigned char ControlByte, unsigned char address, unsigned char *rdptr, unsigned char length )
{
	IdleI2C();                      // ensure module is idle
	StartI2C();                     // initiate START condition
	while ( SSP2CON2bits.SEN );      // wait until start condition is over 
	WriteI2C( ControlByte );        // write 1 byte 
	IdleI2C();                      // ensure module is idle
	WriteI2C( address );            // WRITE word address to EEPROM
	IdleI2C();                      // ensure module is idle
	RestartI2C();                   // generate I2C bus restart condition
	while ( SSP2CON2bits.RSEN );     // wait until re-start condition is over 
	WriteI2C( ControlByte | 0x01 ); // WRITE 1 byte - R/W bit should be 1 for read
	IdleI2C();                      // ensure module is idle
	getstringI2C( rdptr, length );  // read in multiple bytes
	NotAckI2C();                    // send not ACK condition
	while ( SSP2CON2bits.ACKEN );    // wait until ACK sequence is over 
	StopI2C();                      // send STOP condition
	while ( SSP2CON2bits.PEN );      // wait until stop condition is over 
	return ( 0 );                   // return with no error
}

/********************************************************************
*     Function Name:    putstringI2C                                *
*     Return Value:     error condition status                      *
*     Parameters:       address of write string storage location    *
*     Description:      This routine writes a string to the I2C bus,*
*                       until a null character is reached. If Master*
*                       function putcI2C is called. When trans-     *
*                       mission is complete then test for ack-      *
*                       nowledge bit. If Slave transmitter wait for *
*                       null character or not ACK received from bus *
*                       device.                                     *
********************************************************************/

unsigned char putstringI2C( unsigned char *wrptr,unsigned char length)
{

	unsigned char x;
  for (x = 0; x < length; x++ ) // transmit data until PageSize  
  {
    if ( SSP2CON1bits.SSPM3 )      // if Master transmitter then execute the following
    {
      if ( WriteI2C ( *wrptr ) )   // write 1 byte
      {
        return ( -3 );            // return with write collision error
      }
      IdleI2C();                  // test for idle condition
      if ( SSP2CON2bits.ACKSTAT )  // test received ack bit state
      {
        return ( -2 );            // bus device responded with  NOT ACK
      }                           // terminateputstringI2C() function
    }
    else                          // else Slave transmitter
    {
      PIR3bits.SSP2IF = 0;         // reset SSPIF bit
      SSP2BUF = *wrptr;            // load SSPBUF with new data
      SSP2CON1bits.CKP = 1;        // release clock line 
      while ( !PIR3bits.SSP2IF );  // wait until ninth clock pulse received

      if ( ( !SSP2STATbits.R_W ) && ( !SSP2STATbits.BF ) )// if R/W=0 and BF=0, NOT ACK was received
      {
        return ( -2 );            // terminateputstringI2C() function
      }
    }
  wrptr ++;                       // increment pointer
  }                               // continue data writes until null character
  return ( 0 );
}

/********************************************************************
*     Function Name:    getstringI2C                                     *
*     Return Value:     error condition status                      *
*     Parameters:       address of read string storage location     *
*                       length of string bytes to read              *
*     Description:      This routine reads a predetemined string    *
*                       length in from the I2C bus. The routine is  *                       
*                       developed for the Master mode. The bus ACK  *
*                       condition is generated within this routine. *
********************************************************************/
unsigned char getstringI2C( unsigned char *rdptr, unsigned char length )
{
    while ( length -- )           // perform getcI2C() for 'length' number of bytes
    {
      *rdptr++ = ReadI2C();       // save byte received
      while ( SSP2CON2bits.RCEN ); // check that receive sequence is over    

      if ( PIR3bits.BCL2IF )       // test for bus collision
      {
        return ( -1 );            // return with Bus Collision error 
      }

      if ( length )               // test if 'length' bytes have been read
      {
        SSP2CON2bits.ACKDT = 0;    // set acknowledge bit state for ACK        
        SSP2CON2bits.ACKEN = 1;    // initiate bus acknowledge sequence
        while ( SSP2CON2bits.ACKEN ); // wait until ACK sequence is over 
      }
    }
    return ( 0 );                 // last byte received so don't send ACK      
}

/********************************************************************
*     Function Name:    ReadI2C                                     *
*     Return Value:     contents of SSPBUF register                 *
*     Parameters:       void                                        *
*     Description:      Read single byte from I2C bus.              *
********************************************************************/
unsigned char ReadI2C( void )
{
	SSP2CON2bits.RCEN = 1;           // enable master for 1 byte reception
	while ( !SSP2STATbits.BF );      // wait until byte received  
	return ( SSP2BUF );              // return with read byte 
}

/********************************************************************
*     Function Name:    WriteI2C                                    *
*     Return Value:     Status byte for WCOL detection.             *
*     Parameters:       Single data byte for I2C bus.               *
*     Description:      This routine writes a single byte to the    * 
*                       I2C bus.                                    *
********************************************************************/
unsigned char WriteI2C( unsigned char data_out )
{
	SSP2BUF = data_out;           // write single byte to SSPBUF
	if ( SSP2CON1bits.WCOL )      // test if write collision occurred
		return ( -1 );              // if WCOL bit is set return negative #
	else
	{
		while( SSP2STATbits.BF );   // wait until write cycle is complete         
		return ( 0 );              // if WCOL bit is not set return non-negative #
	}
}

/********************************************************************
*     Function Name:    IdleI2C                                     *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Test and wait until I2C module is idle.     *
********************************************************************/

void IdleI2C( void )
{
	while ( ( SSP2CON2 & 0x1F ) | ( SSP2STATbits.R_W ) )
		continue;
}

/********************************************************************
*     Function Name:    StartI2C                                    *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C bus start condition.               *
********************************************************************/

void StartI2C( void )
{
	SSP2CON2bits.SEN = 1;            // initiate bus start condition
}

/********************************************************************
*     Function Name:    StopI2C                                     *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C bus stop condition.                *
********************************************************************/

void StopI2C( void )
{
	SSP2CON2bits.PEN = 1;            // initiate bus stop condition
}

/********************************************************************
*     Function Name:    RestartI2C                                  *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C bus restart condition.             *
********************************************************************/

void RestartI2C( void )
{
	SSP2CON2bits.RSEN = 1;           // initiate bus restart condition
}

/********************************************************************
*     Function Name:    NotAckI2C                                   *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Initiate NOT ACK bus condition.             *
********************************************************************/

void NotAckI2C( void )
{
	SSP2CON2bits.ACKDT = 1;          // set acknowledge bit for not ACK
	SSP2CON2bits.ACKEN = 1;          // initiate bus acknowledge sequence
}
/********************************************************************
*     Function Name:    EEAckPolling                                *
*     Return Value:     error condition status                      *
*     Parameters:       EE memory control byte                      *
*     Description:      Acknowledge polling of I2C EE memory        *
*                       device. This routine can be used for most   *
*                       I2C EE memory device which uses acknowledge *
*                       polling.                                    *
********************************************************************/
unsigned char EEAckPolling( unsigned char control )
{
	IdleI2C();                      // ensure module is idle 
	StartI2C();                     // initiate START condition
	while ( SSP2CON2bits.SEN );      // wait until start condition is over 
	if ( PIR3bits.BCL2IF )           // test for bus collision
	{
		return ( -1 );                // return with Bus Collision error 
	}

	else
	{
		if ( WriteI2C( control ) )    // write byte - R/W bit should be 0
		{
			return ( -3 );              // set error for write collision
		}

		IdleI2C();                    // ensure module is idle   
		if ( PIR3bits.BCL2IF )         // test for bus collision
		{
			return ( -1 );              // return with Bus Collision error 
		}

		while ( SSP2CON2bits.ACKSTAT ) // test for ACK condition received
		{
			RestartI2C();               // initiate Restart condition
			while ( SSP2CON2bits.RSEN ); // wait until re-start condition is over 
			if ( PIR3bits.BCL2IF )       // test for bus collision
			{
				return ( -1 );            // return with Bus Collision error 
			}

			if ( WriteI2C( control ) )  // write byte - R/W bit should be 0
			{
				return ( -3 );            // set error for write collision
			}
			IdleI2C();                  // ensure module is idle
		}
	}
         
	StopI2C();                      // send STOP condition
	while ( SSP2CON2bits.PEN );      // wait until stop condition is over         
	if ( PIR3bits.BCL2IF )           // test for bus collision
	{
		return ( -1 );              // return with Bus Collision error 
	}
	return ( 0 );                   // return with no error     
}