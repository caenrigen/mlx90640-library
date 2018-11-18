/**
 * @copyright (C) 2017 Melexis N.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef _MLX640_API_H_
#define _MLX640_API_H_
    
  typedef struct
    {
        int16_t kVdd;
        int16_t vdd25;
        float KvPTAT;
        float KtPTAT;
        uint16_t vPTAT25;
        float alphaPTAT;
        int16_t gainEE;
        float tgc;
        float cpKv;
        float cpKta;
        uint8_t resolutionEE;
        uint8_t calibrationModeEE;
        float KsTa;
        float ksTo[4];
        int16_t ct[4];
        float alpha[768];    
        int16_t offset[768];    
        float kta[768];    
        float kv[768];
        float cpAlpha[2];
        int16_t cpOffset[2];
        float ilChessC[3]; 
        uint16_t brokenPixels[5];
        uint16_t outlierPixels[5];  
    } paramsMLX90640;

    // Register addresses
    #define MLX90640_SR_ADDR 0x8000
    #define MLX90640_CR1_ADDR 0x800D
    #define MLX90640_I2CCR_ADDR 0x800F

    // Control Register 1 configuration macros
    #define MLX90640_CR1_SUBPAGE_MODE_ON                 0b1111111111111111
    #define MLX90640_CR1_SUBPAGE_MODE_OFF                0b1111111111111110

    #define MLX90640_CR1_TRANSFER_TO_RAM_ON              0b1111111111111011
    #define MLX90640_CR1_TRANSFER_TO_RAM_IF_OVERWRITE_EN 0b1111111111111111

    #define MLX90640_CR1_SUBPAGE_REPEAT_ON               0b1111111111111111 // Automatically toggle between subpages
    #define MLX90640_CR1_SUBPAGE_REPEAT_OFF              0b1111111111110111

    #define MLX90640_CR1_SELECT_SUBPAGE_0                0b1111111111101111
    #define MLX90640_CR1_SELECT_SUBPAGE_1                0b1111111111111111

    #define MLX90640_CR1_REFRESH_HALF_HZ                 0b1111110001111111
    #define MLX90640_CR1_REFRESH_1_HZ                    0b1111110011111111
    #define MLX90640_CR1_REFRESH_2_HZ                    0b1111110101111111
    #define MLX90640_CR1_REFRESH_4_HZ                    0b1111110111111111
    #define MLX90640_CR1_REFRESH_8_HZ                    0b1111111001111111
    #define MLX90640_CR1_REFRESH_16_HZ                   0b1111111011111111
    #define MLX90640_CR1_REFRESH_32_HZ                   0b1111111101111111
    #define MLX90640_CR1_REFRESH_64_HZ                   0b1111111111111111

    #define MLX90640_CR1_ADC_RESOLUTION_16BIT            0b1111001111111111
    #define MLX90640_CR1_ADC_RESOLUTION_17BIT            0b1111011111111111
    #define MLX90640_CR1_ADC_RESOLUTION_18BIT            0b1111101111111111
    #define MLX90640_CR1_ADC_RESOLUTION_19BIT            0b1111111111111111

    #define MLX90640_CR1_CHESS_MODE                      0b1111111111111111
    #define MLX90640_CR1_TV_MODE                         0b1110111111111111

    int MLX90640_InterpolateOutliers(uint16_t *frameData, uint16_t *eepromData);
    int MLX90640_GetFullFrameData(uint8_t slaveAddr, uint16_t *frameData);
    int MLX90640_ConfReg1(uint8_t slaveAddr, uint16_t regValue);
    
    int MLX90640_DumpEE(uint8_t slaveAddr, uint16_t *eeData);
    int MLX90640_GetFrameData(uint8_t slaveAddr, uint16_t *frameData);
    int MLX90640_ExtractParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
    float MLX90640_GetVdd(uint16_t *frameData, const paramsMLX90640 *params);
    float MLX90640_GetTa(uint16_t *frameData, const paramsMLX90640 *params);
    void MLX90640_GetImage(uint16_t *frameData, const paramsMLX90640 *params, float *result);
    void MLX90640_CalculateTo(uint16_t *frameData, const paramsMLX90640 *params, float emissivity, float tr, float *result);
    int MLX90640_SetResolution(uint8_t slaveAddr, uint8_t resolution);
    int MLX90640_GetCurResolution(uint8_t slaveAddr);
    int MLX90640_SetRefreshRate(uint8_t slaveAddr, uint8_t refreshRate);   
    int MLX90640_GetRefreshRate(uint8_t slaveAddr);  
    int MLX90640_GetSubPageNumber(uint16_t *frameData);
    int MLX90640_GetCurMode(uint8_t slaveAddr); 
    int MLX90640_SetInterleavedMode(uint8_t slaveAddr);
    int MLX90640_SetChessMode(uint8_t slaveAddr);
    
#endif
