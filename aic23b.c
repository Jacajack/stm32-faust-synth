#include <aic23b.h>

void aic23b_write_reg( I2C_HandleTypeDef *h, uint8_t addr, uint8_t reg, uint16_t data, int timeout )
{
	uint8_t dl = data;
	uint8_t b8 = ( data >> 8 ) & 1;
	if ( HAL_I2C_Mem_Write( h, addr, reg | b8, 1, &dl, 1, timeout ) != HAL_OK ) throw h->ErrorCode;
}
