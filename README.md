# Linux-driver-for-sht20


## Test Environment:
 - OS: ubuntu 20.04 Server 64bits LTS
 - Board: Raspberry pi 4b

## Install guide:
  1. Compile device tree
  ```bash=
    dtc -@ -I dts -O dtb -o sht20-i2c.dtbo sht20-i2c.dti
  ```
  2. Move device tree binary code to /boot/firmware/overlays/
  3. Add line on /boot/firmware/config.txt
  ```text=
  + dtoverlay=sht20-i2c  
  ```
  4. reboot
  5. Compile driver
  ```bash=
  make
  ```
  6. Load module
  ```bash=
  sudo insmod sht20_i2c_drv.ko
  ```
  7. Compile test program
  ```bash=
  make test
  ```
  8. Give permission
  ```bash=
  sudo chmod 666 /dev/sht20
  ```
  9. Excute test
  ```bash=
  ./test
  ```

## Data format
```text=
0xa 0xb 0xc 0xb
```

Temperature(°C) = 0xa0b / 100

Humidity(RH) =  0xc0b / 100 %
