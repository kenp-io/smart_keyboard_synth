## Readme!

This folder contains the code and instructions to create the firmware of a key.

## Compiling the user program

The firmware was created with the MBED Studio IDE. You can pull the latest version of MBED OS and get an example program running (eg the blinky program). Once you got it working you can replace the main.cpp and mbed_app.json file in the root directory of the project.

Then to compile you need to install MBEC CLI 1 on your computer and run the following command in the folder of the project.
```sh
mbed-cli compile -m NUCLEO_F303K8 -t GCC_ARM
```
The reason we don't use MBED Studio to compile is because it overrides the memory setting changed we made with its build profiles.
Then we need to convert the generated ELF file to a SREC file so that BootCommander can use it.
```sh
/Applications/ARM/arm-none-eabi/bin/objcopy -O srec '/path/to/Mbed Programs/example_app/BUILD/NUCLEO_F303K8/GCC_ARM/example_app.elf' '/path/to/Mbed Programs/example_app/BUILD/NUCLEO_F303K8/GCC_ARM/example_app.srec'
```
We now have the SREC file that can be flashed to all the keys with BootCommander.

## Getting the user program to work with the bootloader

The first time you compile your program, you need to modify a few files in order to get it to work with OpenBLT.
Basic instructions were found here : https://www.feaser.com/openblt/doku.php?id=manual:ports:armcm4_stm32f3

Please read them and you can apply them to the following files in MBED OS:

Vector table location: in cmsis_nvic.h edit the following lines:

```sh
#define MBED_ROM_START  0x08002000
#define MBED_ROM_SIZE 0xE000  // 64 KB - 8 KB
```
in system_clock.c comment the following lines:
```sh
#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET;
#else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
#endif
```

## Building the Bootloader

Follow the instructions on the OpenBLT website to build the Bootloader: https://www.feaser.com/openblt/doku.php?id=manual:demos:nucleo_f303k8_gcc

You need to build a different bootloader for every key and change the following lines in each blt_conf.h file to give different CAN identifiers.
```sh
/** \brief Configure CAN message ID target->host. */
#define BOOT_COM_CAN_TX_MSG_ID          (0x011 /*| 0x80000000*/)
/** \brief Configure CAN message ID host->target. */
#define BOOT_COM_CAN_RX_MSG_ID          (0x021 /*| 0x80000000*/)
```
## Flashing the Bootloader

We use the custom Bootloader OpenBLT and we need to flash it on every key. This needs to be done only once.
Run the following commands:
```sh
/Applications/STMicroelectronics/STM32CubeProgrammer.app/Contents/MacOs/bin/STM32_Programmer_CLI -c port=SWD -e all
```
To erase the whole memory on the key.
```sh
/Applications/STMicroelectronics/STM32CubeProgrammer.app/Contents/MacOs/bin/STM32_Programmer_CLI -c port=SWD -w /path/to/openblt_stm32f303_CAN1.srec
```
To flash the bootloader on the key.

## Flashing the program

This is described in the Raspberry Pi folder.
