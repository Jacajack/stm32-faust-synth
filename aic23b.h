#ifndef AIC23B_H
#define AIC23B_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <i2c.h>

	
/*
	\todo rename macros
*/
	
	
#define CODEC_ADDR              (0x1a << 1)
#define CODEC_REG_LLIN_VOL           (0 << 1) // Line IN L - volume
#define CODEC_REG_RLIN_VOL           (1 << 1) // Line IN R - volume
#define CODEC_REG_LHP_VOL            (2 << 1) // Headphones L - volume
#define CODEC_REG_RHP_VOL            (3 << 1) // Headphones R - volume
#define CODEC_REG_ANALOG_AUDIO_PATH  (4 << 1)
#define CODEC_REG_DIGITAL_AUDIO_PATH  (5 << 1)
#define CODEC_REG_POWER_DOWN         (6 << 1)
#define CODEC_REG_DIGITAL_AUDIO_FMT   (7 << 1)
#define CODEC_REG_SAMPLE_RATE        (8 << 1)
#define CODEC_REG_DIGITAL_IF_ACT    (9 << 1)
#define CODEC_REG_RESET               (10 << 1)

#define CODEC_MIC_BOOST        (1 << 0)
#define CODEC_MIC_MUTE         (1 << 1)
#define CODEC_ADC_INSEL        (1 << 2)
#define CODEC_BYPASS           (1 << 3)
#define CODEC_DAC_SEL          (1 << 4)
#define CODEC_SIDETONE_ENABLED (1 << 5)
#define CODEC_SIDETONE         (1 << 6)


#define CODEC_FMT_MASTER (1<<6)
#define CODEC_FMT_LRSWAP (1<<5)
#define CODEC_FMT_LRP (1<<4)
#define CODEC_FMT_LEN16 (0<<2)
#define CODEC_FMT_LEN20 (1<<2)
#define CODEC_FMT_LEN24 (2<<2)
#define CODEC_FMT_LEN32 (3<<2)
#define CODEC_FMT_DSP (3<<0)
#define CODEC_FMT_I2S (2<<0)
#define CODEC_FMT_MSB_LEFT (1<<0)
#define CODEC_FMT_MSB_RIGHT (0<<0)

#define CODEC_SR_BOSR (1<<1)
#define CODEC_SR_SR0 (1<<2)
#define CODEC_SR_SR1 (1<<3)
#define CODEC_SR_SR2 (1<<4)
#define CODEC_SR_SR3 (1<<5)
#define CODEC_SR_CLKIN (1<<6)
#define CODEC_SR_CLKOUT (1<<7)
#define CODEC_SR_NORMAL (0<<0)
#define CODEC_SR_USB (1<<0)

#define CODEC_RESET_MAGIC     0

extern void aic23b_write_reg( I2C_HandleTypeDef *h, uint8_t addr, uint8_t reg, uint16_t data, int timeout );
	
#ifdef __cplusplus
}
#endif

#endif
