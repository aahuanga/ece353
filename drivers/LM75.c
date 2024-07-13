/**
 * @file LM75.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-09-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "i2c.h"
#include "LM75.h"

/** Read a register on the LM75AIMME/NOPB
 *
 * @param reg The reg address to read
 *
 */
static int16_t LM75_read_reg(uint8_t reg)
{
	int16_t return_value;
	cy_rslt_t rslt;

	uint8_t write_data[1] = {reg};
	uint8_t read_data[2];

	/* Use cyhal_i2c_master_write to write the required data to the device. 
	 * Send the register address and then generate a stop condition 
	 */
	rslt = cyhal_i2c_master_write(&i2c_monarch_obj, LM75_SUBORDINATE_ADDR, write_data, 1, 0, true);
	CY_ASSERT(rslt == CY_RSLT_SUCCESS); /* If the write failed, halt the CPU */

	/* Use cyhal_i2c_master_read to read the required data from the device. 
	 * The register address has already been set in the write above, so read two bytes
	 * of data.
	 */
	rslt = cyhal_i2c_master_read(&i2c_monarch_obj, LM75_SUBORDINATE_ADDR, read_data, 2, 0, true);
	CY_ASSERT(rslt == CY_RSLT_SUCCESS); /* If the read failed, halt the CPU */
	
	/* Return an 16-bit signed number that represents the temp. */
	return_value = read_data[0] << 1;
	if(read_data[1] & 0x80)
	{
		return_value = return_value | 0x1;
	}
	if (read_data[0] & 0x80){
		return_value = return_value | 0xFE00;
	}
	return return_value;
}

/** Read the value of the input port
 *
 * @param reg The reg address to read
 *
 */
float LM75_get_temp(void)
{
	int16_t raw_value = LM75_read_reg(LM75_TEMP_REG);
	float temp = (double)raw_value / 2;
	return temp;
}

static int8_t LTR_read_reg(uint8_t reg)
{
	int8_t return_value;
	cy_rslt_t rslt;

	uint8_t write_data[1] = {reg};
	uint8_t read_data[1];

	/* Use cyhal_i2c_master_write to write the required data to the device. 
	 * Send the register address and then generate a stop condition 
	 */
	rslt = cyhal_i2c_master_write(&i2c_monarch_obj, LTR_SUBORDINATE_ADDR, write_data, 1, 0, true);
	CY_ASSERT(rslt == CY_RSLT_SUCCESS); /* If the write failed, halt the CPU */

	/* Use cyhal_i2c_master_read to read the required data from the device. 
	 * The register address has already been set in the write above, so read two bytes
	 * of data.
	 */
	rslt = cyhal_i2c_master_read(&i2c_monarch_obj, LTR_SUBORDINATE_ADDR, read_data, 1, 0, true);
	CY_ASSERT(rslt == CY_RSLT_SUCCESS); /* If the read failed, halt the CPU */
	
	return_value = read_data[0];

	return return_value;
}

static void LTR_write_reg(uint8_t reg, uint8_t value)
{
	cy_rslt_t rslt;

	uint8_t write_data[2] = {reg, value};

	/* Use cyhal_i2c_master_write to write the required data to the device. 
	 * Send the register address and then generate a stop condition 
	 */
	rslt = cyhal_i2c_master_write(&i2c_monarch_obj, LTR_SUBORDINATE_ADDR, write_data, 2, 0, true);
	CY_ASSERT(rslt == CY_RSLT_SUCCESS); /* If the write failed, halt the CPU */
}

/** Read the value of the input port
 *
 * @param reg The reg address to read
 *
 */
int LTR_get_data(void)
{
	int ALS_LUX;
	int ALS_GAIN = 1;
	int ALS_INT = 1;

	// Enable ALS
	LTR_write_reg(0x80, 0x01);
	
	// ALS_DATA_CH1
	int8_t data_0 = LTR_read_reg(ALS_DATA_CH1_LOW);
	int8_t data_1 = LTR_read_reg(ALS_DATA_CH1_HIGH);	
	
	// ALS_DATA_CH0
	int8_t data_2 = LTR_read_reg(ALS_DATA_CH0_LOW);
	int8_t data_3 = LTR_read_reg(ALS_DATA_CH0_HIGH);

	int16_t ALS_CH1_ADC_Data = (data_1 << 8) | data_0;
	ALS_CH1_ADC_Data = abs(ALS_CH1_ADC_Data);
	int16_t ALS_CH0_ADC_Data = (data_3 << 8) | data_2;
	ALS_CH0_ADC_Data = abs(ALS_CH0_ADC_Data);

	double ratio = (double)ALS_CH1_ADC_Data/((double)ALS_CH0_ADC_Data + (double)ALS_CH1_ADC_Data);
	if (ratio < 0.45) {
		ALS_LUX = ((1.7743 * ALS_CH0_ADC_Data) + (1.1059 * ALS_CH1_ADC_Data)) / ALS_GAIN / ALS_INT;
	} else if (ratio < 0.64 && ratio >= 0.45) {
		ALS_LUX = ((4.2785 * ALS_CH0_ADC_Data) - (1.9548 * ALS_CH1_ADC_Data)) / ALS_GAIN / ALS_INT;
	} else if (ratio < 0.85 && ratio >= 0.64) {
		ALS_LUX = ((0.5926 * ALS_CH0_ADC_Data) + (0.1185 * ALS_CH1_ADC_Data)) / ALS_GAIN / ALS_INT;
	} else {
		ALS_LUX = 0;
	}
	return ALS_LUX;
}