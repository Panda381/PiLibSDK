
// ****************************************************************************
//
//                            I2C bus driver
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_I2CBUS		// 1=use I2C bus driver, 0=not used (drv_i2cbus.*)

// flag - I2C bus driver is enabled
Bool I2Cbus_IsUse[I2C_NUM] = {
	False, False,
#if RASPPI >= 4
	False, False, False, False, False,
#endif
};

// I2C bus
sI2Cbus	I2Cbus[I2C_NUM];

// start new transaction
static void I2Cbus_Start(sI2Cbus* c);

// stop transaction with result (spinlock must be locked)
//  Function can temporary unlock the lock, during callback()
static void I2Cbus_Stop(sI2Cbus* c, int status)
{
	// get current message
	sI2Cmsg* msg = BASEFROMLIST(ListGetFirst(&c->list), sI2Cmsg, list);

	// set result status
	msg->status = status;

	// peripheral address
	I2C_t* a = c->i2c;

	// disable interrupts
	a->CTRL = B4|B5|B15; // I2CEN enable I2C controller, disable interrupts, clear FIFO

	// clear status
	a->STAT = B1|B8|B9; // clear DONE, ERR and CLKT flag

	// remove message from the queue
	ListEntryRemove(&msg->list);

	// start new transaction (transaction must be in progress before callback)
	if (!ListIsEmpty(&c->list)) I2Cbus_Start(c);

	// callback ... lock will be temporary unlocked
	if (msg->callback != NULL)
	{
		SpinLockUnlock(&c->lock);	// unlock
		msg->callback(msg);		// callback
		SpinLockLock(&c->lock);		// lock
	}
}

// start new transaction (spinlock must be locked)
//  Function can temporary unlock the lock, during callback()
//  At this point, there must be a valid message in the queue awaiting processing.
static void I2Cbus_Start(sI2Cbus* c)
{
	// peripheral address
	I2C_t* a = c->i2c;

	// wait until the bus is free (time-out 10us)
	u32 start = Time();
	c->start = start;
	while ((a->STAT & B0) != 0)
	{
		if ((u32)(Time() - start) >= 10)
		{
			// stop transaction with result (spinlock must be locked)
			//  Function can temporary unlock the lock, during callback()
			//  - In this case, callback() must not add another 
			//    message to prevent operations from re-entering
			I2Cbus_Stop(c, I2CMSG_LOCKED);
			return;
		}
	}

	// clear old transfer and FIFO
	a->STAT = B1|B8|B9;	// clear DONE, ERR and CLKT
	a->CTRL = B4|B5|B15;	// clear FIFO

	// get current message
	sI2Cmsg* msg = BASEFROMLIST(ListGetFirst(&c->list), sI2Cmsg, list);

	// setup transfer speed
	int clk = msg->speed;
	if ((clk != 0) && (clk != I2C_LastSpeed[c->i2cinx]))
	{
		I2C_LastSpeed[c->i2cinx] = clk;
		int div = (CoreClock + clk/2) / clk; // get divider
		div &= ~1;			// round down to even number
		if (div < 2) div = 2;		// minimal value
		if (div > 65534) div = 65534;	// maximal value
		a->DIV = div;
		WaitUs(5);		// delay about 1 clock to update pre-divider
	}

	// setup transfer
	a->ADDR = msg->addr;		// setup slave address
	int n = msg->len + msg->len2;	// number of bytes
	a->DLEN = n;			// set number of bytes
	msg->status = I2CMSG_RUN;	// transfer is in progress
	
	// if write direction - fill up FIFO
	int i = 0;
	u8* s = msg->data;
	u8* s2 = msg->data2;
	if (!msg->read)
	{
		if (n > I2C_FIFO_SIZE) n = I2C_FIFO_SIZE; // limit number of bytes to the size of the FIFO
		for (; i < n; i++)
		{
			if (i < msg->len)
				a->FIFO = s[i];
			else
				a->FIFO = s2[i - msg->len];
		}
	}
	c->inx = i;			// index of the data

	// start transfer
	c->start = Time();		// start time
	if (msg->read)
		a->CTRL = B0|B7|B8|B10|B15; // start transfer in READ direction, interrupt on DONE and RXR
	else
		a->CTRL = B7|B8|B9|B15;	// start transfer in WRITE direction, interrupt on DONE and TXW
}

// I2C interrupt handler
void I2Cbus_Handler()
{
	int i2cbus;
	for (i2cbus = 0; i2cbus < I2C_NUM; i2cbus++)
	{	
		sI2Cbus* c = &I2Cbus[i2cbus];
		if (I2Cbus_IsUse[i2cbus])
		{
			// lock bus driver
			SpinLockLock(&c->lock);

			// peripheral address
			I2C_t* a = c->i2c;

			// if transaction is valid
			if (!ListIsEmpty(&c->list))
			{
				// get current message
				sI2Cmsg* msg = BASEFROMLIST(ListGetFirst(&c->list), sI2Cmsg, list);

				// get STAT register
				u32 stat = a->STAT;

				// transfer complete "DONE", ACK error "ERR" or stretch error "CLKT"
				if ((stat & (B1|B8|B9)) != 0)
				{
					// read rest of data from FIFO
					if (msg->read)
					{
						// short time for signal settling
						WaitUs(2);

						while ((c->inx < msg->len+msg->len2) && ((a->STAT & B5) != 0)) // "RXD"
						{
							if (c->inx < msg->len)
								msg->data[c->inx] = a->FIFO;
							else
								msg->data2[c->inx - msg->len] = a->FIFO;
							c->inx++;
						}
						c->start = Time();	// start time
					}

					a->STAT = B1|B8|B9; // clear DONE, ERR and CLKT flag
					int res = I2CMSG_DONE; // result OK
					if ((stat & B9) != 0) res = I2CMSG_CLKT; // error CLKT
					if (((stat & B8) != 0) || (c->inx < msg->len+msg->len2)) res = I2CMSG_ERR; // error ERR
					// stop transaction with result (spinlock must be locked)
					//  Function can temporary unlock the lock, during callback()
					I2Cbus_Stop(c, res);
				}

				// FIFO contains received byte
				else if (msg->read && ((stat & B5) != 0)) // "RXD"
				{
					if (c->inx < msg->len+msg->len2)
					{
						if (c->inx < msg->len)
							msg->data[c->inx] = a->FIFO;
						else
							msg->data2[c->inx - msg->len] = a->FIFO;
						c->inx++;
						c->start = Time();	// start time
					}
					else
						(void)a->FIFO; // destroy data
				}

				// FIFO can accept next byte
				else if (!msg->read && ((stat & B2) != 0)) // "TXW"
				{
					if (c->inx < msg->len+msg->len2)
					{
						if (c->inx < msg->len)
							a->FIFO = msg->data[c->inx];
						else
							a->FIFO = msg->data2[c->inx - msg->len];
						c->inx++;
						if (c->inx >= msg->len+msg->len2) a->CTRL &= ~B9; // disable TXW interrupt
						c->start = Time();	// start time
					}
					else
						a->FIFO = 0xff;
				}
			}

			// list is empty
			else
			{
				// disable interrupts
				a->CTRL = B4|B5|B15; // I2CEN enable I2C controller, disable interrupts, clear FIFO

				// clear status
				a->STAT = B1|B8|B9; // clear DONE, ERR and CLKT flag
			}

			// unlock bus driver
			SpinLockUnlock(&c->lock);
		}
	}
}

// initialize I2C bus driver
// - The user code can access unused I2C bus using polled access.
void I2Cbus_Init(int i2c)
{
	// initialize I2C and setup GPIOs
	I2C_Init(i2c);

	// initialize I2Cbus
	sI2Cbus* c = &I2Cbus[i2c];	// pointer to I2C bus descriptor
	ListInit(&c->list);		// initialize list of I2C messages
	SpinLockInit(&c->lock);		// initialize spin lock
	c->i2cinx = i2c;		// I2C bus index
	c->i2c = I2C(i2c);		// setup I2C interface address

	// register handler
	IntHandler(IRQ_I2C, I2Cbus_Handler);

	// enable interrupt
	IntEnable(IRQ_I2C);

	// driver was initialized
	cb();
	I2Cbus_IsUse[i2c] = True;		// use this I2C bus
}

// terminate I2C bus driver
void I2Cbus_Term(int i2c)
{
	// not initialized
	if (!I2Cbus_IsUse[i2c]) return;
	I2Cbus_IsUse[i2c] = False;		// stop using this I2C bus

	// I2C controller
	I2C_t* a = I2C(i2c);

	// disable interrupts
	a->CTRL = B15;

	// clear old transfer and clear FIFO
	a->STAT = B1|B8|B9;	// clear DONE, ERR and CLKT
	a->CTRL = B4|B5|B15;	// clear FIFO

	// disable I2C controller
	a->CTRL = 0;

	// check if it was last working bus
	Bool ok = False;
	int i;
	for (i = 0; i < I2C_NUM; i++) if (I2Cbus_IsUse[i]) ok = True;

	// disable interrupt, if it was last bus
	if (!ok) IntDisable(IRQ_I2C);
}

// auto-initialize I2C bus driver, if not initialized
void I2Cbus_AutoInit(int i2c)
{
	// if this i2c is not used
	if (!I2Cbus_IsUse[i2c]) I2Cbus_Init(i2c);
}

// I2C bus driver watchdog (called from systick handler)
void I2Cbus_Watchdog()
{
	int i2c;
	for (i2c = 0; i2c < I2C_NUM; i2c++)
	{
		// pointer to bus driver
		sI2Cbus* c = &I2Cbus[i2c];
		if (I2Cbus_IsUse[i2c])	// use this I2C bus?
		{
			// lock bus driver
			IRQ_LOCK;
			SpinLockLock(&c->lock);

			// if message list is not empty - check time-out (500ms)
			if (!ListIsEmpty(&c->list) && ((u32)(Time() - c->start) > 500000))
			{
				// stop transaction with result (spinlock must be locked)
				//  Function can temporary unlock the lock, during callback()
				//  - In this case, callback() must not add another 
				//    message to prevent operations from re-entering
				I2Cbus_Stop(c, I2CMSG_TIMEOUT);
			}

			// unlock bus driver
			SpinLockUnlock(&c->lock);
			IRQ_UNLOCK;
		}
	}
}

// add I2C message to message queue
//    i2c ... I2C index 0 or 1
//  Before the transmission begins, the following items must be initialized in the msg structure:
//    callback ... pointer to the callback function called after the transfer is complete (NULL=not used)
//    data ... pointer to data buffer to receive or send data, part 1 (ignored if len=0)
//    len ... length of data buffer, part 1 (number of data bytes to transfer)
//    data2 ... pointer to data buffer to receive or send data, part 2 (ignored if len2=0)
//    len2 ... length of data buffer, part 2 (number of data bytes to transfer)
//    speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
//    addr ... device address on I2C bus
//    read ... True=read data, False=write data
//  optionaly: cookie ... user's data
// Only "list" and "status" entries will be modified in the I2C bus driver.
// The status of the operation can be viewed in the "status" entry.
// The structure of the message must remain valid until the transmission is complete.
// This function can also be called from an IRQ interrupt handler, but it must not be called from a FIQ fast interrupt.
// This function can call callback() if transaction cannot start (on error I2CMSG_LOCKED).
// I2C bus driver is auto-initialized, if not initialized yet.
void I2Cbus_Add(int i2c, sI2Cmsg* msg)
{
	// auto-initialize I2C bus driver, if not initialized
	I2Cbus_AutoInit(i2c);

	// pointer to bus driver
	sI2Cbus* c = &I2Cbus[i2c];

	// lock bus driver
	IRQ_LOCK;
	SpinLockLock(&c->lock);

	// check if list is empty
	Bool empty = ListIsEmpty(&c->list);

	// add message to the queue
	ListAddLast(&c->list, &msg->list);
	msg->status = I2CMSG_PENDING;	// waiting for transaction

	// start first message
	//  Function can temporary unlock the lock, during callback()
	if (empty) I2Cbus_Start(c);

	// unlock bus driver
	SpinLockUnlock(&c->lock);
	IRQ_UNLOCK;
}

// read data from I2C, using I2C bus driver, and wait (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  data ... data buffer, part 1 (can be NULL if len = 0)
//  len ... number of bytes 0..65535, part 1
//  data2 ... data buffer, part 2 (can be NULL if len2 = 0)
//  len2 ... number of bytes 0..65535, part 2
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
// Do not check device presence reading 0 bytes, it does not work.
// I2C bus driver is auto-initialized, if not initialized yet.
Bool I2Cbus_Read(int i2c, int addr, void* data, int len, void* data2 /* = NULL */, int len2 /* = 0 */, int speed /* = 0 */)
{
	// prepare message
	sI2Cmsg msg;
	msg.callback = NULL;
	msg.data = (u8*)data;
	msg.len = len;
	msg.data2 = (u8*)data2;
	msg.len2 = len2;
	msg.speed = speed;
	msg.addr = addr;
	msg.read = True;

	// send message
	I2Cbus_Add(i2c, &msg);

	// wait for operation
	while (!I2Cbus_IsDone(&msg)) {};

	// check result
	return msg.status == I2CMSG_DONE;
}

// write data to I2C, using I2C bus driver, and wait (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  data ... data buffer, part 1 (can be NULL if len = 0)
//  len ... number of bytes 0..65535, part 1
//  data2 ... data buffer, part 2 (can be NULL if len2 = 0)
//  len2 ... number of bytes 0..65535, part 2
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
// The presence of a device can be tested by writing a data length of 0.
// I2C bus driver is auto-initialized, if not initialized yet.
Bool I2Cbus_Write(int i2c, int addr, const void* data, int len, const void* data2 /* = NULL */, int len2 /* = 0 */, int speed /* = 0 */)
{
	// prepare message
	sI2Cmsg msg;
	msg.callback = NULL;
	msg.data = (u8*)data;
	msg.len = len;
	msg.data2 = (u8*)data2;
	msg.len2 = len2;
	msg.speed = speed;
	msg.addr = addr;
	msg.read = False;

	// send message
	I2Cbus_Add(i2c, &msg);

	// wait for operation
	while (!I2Cbus_IsDone(&msg)) {};

	// check result
	return msg.status == I2CMSG_DONE;
}

// check presence of the I2C device, using I2C bus driver (to scan devices on the bus; returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
// I2C bus driver is auto-initialized, if not initialized yet.
Bool I2Cbus_Check(int i2c, int addr, int speed /* = I2C_DEF_SPEED */)
{
	return I2Cbus_Write(i2c, addr, NULL, 0, NULL, 0, speed);
}

// read registers with 8-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 8-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_ReadReg8(int i2c, int addr, int inx, u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	// write address
	u8 b[1];
	b[0] = (u8)inx;
	if (!I2Cbus_Write(i2c, addr, b, 1, NULL, 0, speed)) return False;

	// read data
	return I2Cbus_Read(i2c, addr, reg, num, NULL, 0, speed);
}

// read registers with 16-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 16-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_ReadReg16(int i2c, int addr, int inx, u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	// write address
	u8 b[2];
	b[0] = (u8)(inx >> 8);
	b[1] = (u8)inx;
	if (!I2Cbus_Write(i2c, addr, b, 2, NULL, 0, speed)) return False;

	// read data
	return I2Cbus_Read(i2c, addr, reg, num, NULL, 0, speed);
}

// read registers with 24-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 24-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_ReadReg24(int i2c, int addr, int inx, u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	// write address
	u8 b[3];
	b[0] = (u8)(inx >> 16);
	b[1] = (u8)(inx >> 8);
	b[2] = (u8)inx;
	if (!I2Cbus_Write(i2c, addr, b, 3, NULL, 0, speed)) return False;

	// read data
	return I2Cbus_Read(i2c, addr, reg, num, NULL, 0, speed);
}

// read registers with 32-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 32-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_ReadReg32(int i2c, int addr, int inx, u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	// write address
	u8 b[4];
	b[0] = (u8)(inx >> 24);
	b[1] = (u8)(inx >> 16);
	b[2] = (u8)(inx >> 8);
	b[3] = (u8)inx;
	if (!I2Cbus_Write(i2c, addr, b, 4, NULL, 0, speed)) return False;

	// read data
	return I2Cbus_Read(i2c, addr, reg, num, NULL, 0, speed);
}

// write registers with 8-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 8-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_WriteReg8(int i2c, int addr, int inx, const u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	u8 b[1];
	b[0] = (u8)inx;
	return I2Cbus_Write(i2c, addr, b, 1, reg, num, speed);
}

// write registers with 16-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 16-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_WriteReg16(int i2c, int addr, int inx, const u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	u8 b[2];
	b[0] = (u8)(inx >> 8);
	b[1] = (u8)inx;
	return I2Cbus_Write(i2c, addr, b, 2, reg, num, speed);
}

// write registers with 24-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 24-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_WriteReg24(int i2c, int addr, int inx, const u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	u8 b[3];
	b[0] = (u8)(inx >> 16);
	b[1] = (u8)(inx >> 8);
	b[2] = (u8)inx;
	return I2Cbus_Write(i2c, addr, b, 3, reg, num, speed);
}

// write registers with 32-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 32-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_WriteReg32(int i2c, int addr, int inx, const u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	u8 b[4];
	b[0] = (u8)(inx >> 24);
	b[1] = (u8)(inx >> 16);
	b[2] = (u8)(inx >> 8);
	b[3] = (u8)inx;
	return I2Cbus_Write(i2c, addr, b, 4, reg, num, speed);
}

#endif // USE_I2CBUS
