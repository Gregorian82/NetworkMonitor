# NM Network Monitor

NM is the network monitor software to record the vehicle network data according to some events or conditions, which can be configured by users.

So far it only supports the CAN network, and extra CAN hardware USB device is needed.

## Software Installation
To make NM running correctly, some hardware driver and configuration software shall be installed.


### Tools
- Python 3.6
- pip and setuptools
- [canmatrix](https://github.com/ebroecker/canmatrix), dbc file translation
- [excel2xx](https://github.com/cupen/excel2xx), excel file translation

### CAN Device Driver

Currently [PEAK device](https://www.peak-system.com/PCAN-USB.199.0.html) has been tested. 

![PEAK USB Device](https://www.peak-system.com/typo3temp/pics/61ef9f60a0.jpg)

Click "Download PCAN Driver Package" in [PEAK webpage](https://www.peak-system.com/fileadmin/media/linux/index.htm) 

Requirements for your Linux OS: 
- The kernel headers (or Linux headers) package is available
- libpopt-dev package installed

```bash
sudo apt-get install libpopt-dev
```

Unzip the package
```bash
make clean
make
sudo make install
sudo modprobe pcan
```

To make sure the installed driver is correct, insert the PEAK USB hardware and
```bash
ls -l /dev/pcan*
```

## Configuration File Generation

I already put the configuration example in "examples" subfolder.

### DBC file

please using [canmatrix](https://github.com/ebroecker/canmatrix) to make the dbc file convert into the json format and save the file name as **can.json** and put it into the subfolder **cfg**

Examples:
```bash
canconvert xxx.dbc can.json
```

### Event Configuration file

In the examples folder has the "event.xls", modify the content from **4th** row.

The idea is that each event can be composed of different signal conditions, their relationship is used with "**AND**" and "**OR**". 

In the "**operator**" column, following symbols are available "==", ">=", "<=", ">", "<" and "!=".

If some signals are bool type, in the "**Threshold**" column, TRUE is equal to 1, FALSE is equal to 0.

please using [excel2xx](https://github.com/cupen/excel2xx) to make the xls file convert into the json format and save the file name as **event.json** and put it into the subfolder **cfg**

Example:
```bash
excel2xx json xxx.xls -o event.json
```

## Code Generation and Execution

Makefile is ready if use the default CAN driver

Code Generation:
```bash
make clean
make
```

To execute the code
```bash
./nm x
```
the parameter x (0~4) is the settings of baudrate: 
0 = 100kbps, 
1 = 125kbps, 
2 = 250kbps, 
3 = 500kbps, 
4 = 1Mkbps 

## Storage Data

Directory "**Report**" structure

    ├── raw                 # raw data w/o conifguration, full data
    └── event               # 
        ├── raw             # raw data w/ configuration
        └── summary         # event summary


## Hardware Embedded Platform

Any x86 and arm platform with LINUX can be used, please consider about the memory size for the storage.

## Support and Contact

zghauto@outlook.com
