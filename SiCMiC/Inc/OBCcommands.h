void receive_OBC_message(void);
void check_OBC_message(void);
#define EXPERIMENTSIZE sizeof(experiments)
/* Size of Transmission buffer */
#define TXBUFFERSIZE                      20
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE
static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength);
uint8_t aRxBuffer[10];
uint8_t aTxBuffer[20];