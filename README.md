# mlx90640-library
MLX90640 library functions for use on Raspberry Pi

Important Note: This code includes big changes to the API that were necessary for a project developed during the "IN4398 Internet of Things Seminar" course at TU Delft University. This repository will not be maintained and doesn't include the latest improvements of the original library.

We hope that the code examples provided here might be of use for someone else :) 

## Dependencies
This code requires the BCM2835 Library to be installed. You can download the latest version from:
```
http://www.airspayce.com/mikem/bcm2835/
```
This code was tested using the bcm2835-1.57 library.

## Getting started
To compile the code examples run:
```
make
```
## thermalCam
This program allows to display the sensor output in the terminal. It must be run as sudo:
```
sudo ./thermalCam
```

## acquisition
This program is used to acquire thermal frames continuously. You have to press `Ctrl+c` to stop it. Usage example:
```
sudo ./acquisition folderForFrames
```

## API changes
The main changes consist of a different paradigm for the configuration of Register 1 and a distinct function meant to read the frame data only when both subpages have been acquired.

## Acknowledgment
The code found here contains some inspirations from the experimental fork at `https://github.com/pimoroni/mlx90640-library`.
