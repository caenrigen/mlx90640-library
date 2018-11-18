#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <string> 
#include <fstream>
#include <chrono>
#include <thread>
#include "headers/MLX90640_API.h"
#include "headers/MLX90640_RPI_I2C_Driver.h"

#define MLX_I2C_ADDR 0x33
#define REFRESH_RATE MLX90640_CR1_REFRESH_16_HZ // Subpages rate is half

// WAIT_TIME_MILLISECONDS = 2*0.68*1000/fps
// 2: there are 2 subframes
// 0.68: wait 68% (a bit less then 80% from the datasheet)
// 1000: s -> ms
#define WAIT_TIME_MILLISECONDS 85

int main(int argc, char *argv[]){
    static uint16_t eeMLX90640[832];
    float emissivity = 1;
    uint16_t frame[834];
    static float image[768];
    float eTa;
    static uint16_t data[768*sizeof(float)];

    int error;
    int64_t max_elapsed=0;
    int64_t max_calcs=0;
    int16_t CtrlReg1 = 0;
    static int64_t t_start=0,\
    t_end=0,\
    elapsed = 0;
    static float mlx90640To[768];

    std::ofstream f;
    std::ofstream fTimings;
    std::ofstream fReadErrors;
    std::string base_path="/home/pi/IoT-Seminar/data/";

    std::string thermalVals = "";
    
    printf("Starting I2C...\n");
    error = MLX90640_I2CInit();
    printf("Error: %d\n", error);
    printf("Setting I2C freq...\n");
    MLX90640_I2CFreqSet(1000000); // Hz
    
    // Configure Control Register 1
    CtrlReg1 =  MLX90640_CR1_SUBPAGE_MODE_ON &\
    MLX90640_CR1_TRANSFER_TO_RAM_ON &\
    MLX90640_CR1_SUBPAGE_REPEAT_OFF &\
    REFRESH_RATE &\
    MLX90640_CR1_ADC_RESOLUTION_16BIT &\
    MLX90640_CR1_CHESS_MODE;
    printf("Setting Control Register 1...\n");
    error = MLX90640_ConfReg1(MLX_I2C_ADDR, CtrlReg1);
    printf("Error: %d\n", error);

    paramsMLX90640 mlx90640;
    printf("Damping EE...\n");
    error = MLX90640_DumpEE(MLX_I2C_ADDR, eeMLX90640);
    printf("Error: %d\n", error);
    printf("Extracting parameters...\n");
    error = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
    printf("Error: %d\n", error);

    if (argc>1)
    {
        base_path += argv[1];
        system(("mkdir " + base_path).c_str());
    }
    else 
        base_path += "pippo";
    base_path+="/";
    printf("Starting Loop...\r\n");
    while(1){
        do{
            t_start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            error = MLX90640_GetFullFrameData(MLX_I2C_ADDR, frame);
            if(error) std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (error != 0 && error != -100){
                fReadErrors.open(base_path + "acqLog.txt", std::ios_base::app);
                fReadErrors << "[" << t_start <<"]: " <<  error << "\n";
                fReadErrors.close();
            }
        }while(error);

        MLX90640_InterpolateOutliers(frame, eeMLX90640);

        frame[833] = 0;
        eTa = MLX90640_GetTa(frame, &mlx90640);
        MLX90640_CalculateTo(frame, &mlx90640, emissivity, eTa, mlx90640To);

        frame[833] = 1;
        eTa = MLX90640_GetTa(frame, &mlx90640);
        MLX90640_CalculateTo(frame, &mlx90640, emissivity, eTa, mlx90640To);

        f.open(base_path + std::to_string(t_start));
        f << std::setprecision(1) << std::fixed;
        for(int y = 23; y > -1; y--){
         for(int x = 31; x > -1; x--){
             float val = mlx90640To[32 * (23-y) + x];
             f << val << " ";
             mlx90640To[32 * (23-y) + x] = 0;
         }
         f << std::endl;
     }
     f.close();

     t_end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
     elapsed = t_end - t_start;

     if( elapsed < WAIT_TIME_MILLISECONDS){
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_MILLISECONDS-elapsed));
    };
};
MLX90640_I2CEnd();
return 0;
}