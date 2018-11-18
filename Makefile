I2C_MODE = RPI
I2C_LIBS = -lbcm2835

all: acquisition thermalCam

libMLX90640_API.so: functions/MLX90640_API.o functions/MLX90640_$(I2C_MODE)_I2C_Driver.o
	$(CXX) -fPIC -shared $^ -o $@ $(I2C_LIBS)

libMLX90640_API.a: functions/MLX90640_API.o functions/MLX90640_$(I2C_MODE)_I2C_Driver.o
	ar rcs $@ $^
	ranlib $@

functions/MLX90640_API.o functions/MLX90640_RPI_I2C_Driver.o : CXXFLAGS+=-fPIC -I headers -shared $(I2C_LIBS)

acquisition.o thermalCam.o : CXXFLAGS+=-std=c++11

acquisition thermalCam: CXXFLAGS+=-I. -std=c++11

acquisition: acquisition.o libMLX90640_API.a
	$(CXX) -L/home/pi/mlx90640-library $^ -o $@ $(I2C_LIBS)

thermalCam: thermalCam.o libMLX90640_API.a
	$(CXX) -L/home/pi/mlx90640-library $^ -o $@ $(I2C_LIBS)

clean:
	rm -f acquisition
	rm -f thermalCam
	rm -f functions/*.o
	rm -f *.o
	rm -f *.so
	rm -f *.a