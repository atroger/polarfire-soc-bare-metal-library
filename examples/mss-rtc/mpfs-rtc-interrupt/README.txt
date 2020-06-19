================================================================================
                 PolarFire SoC MSS RTC interrupt example
================================================================================
This example project demonstrates the use of the PolarFire SoC MSS RTC driver to
configure the MSS RTC block to demonstrate RTC interrupt in binary mode. The 
messages are displayed over the UART terminal at the occurance of each interrupt.

--------------------------------------------------------------------------------
                            How to use this example
--------------------------------------------------------------------------------
To use this project you will need a UART terminal configured as below.
    - 115200 baud
    - 8 data bits
    - 1 stop bit
    - no parity
    - no flow control

This is a self contained example project. A greeting message is displayed over 
the UART terminal. The example project takes care of configuring and intiating 
the MSS RTC block. User can observe the UART messages displayed over the terminal 
to identify the occurance of periodic interrupt. 
 
--------------------------------------------------------------------------------
                                Target hardware
--------------------------------------------------------------------------------
This example project can be used on PolarFire SoC FPGA family hardware platforms
as well as the PolarFire SoC model on Renode emulation platform. 

There are configurations that needs to be set for this example project. The
configurations are categorized into hardware and software configurations and 
are respectively located in the following folders
    platform/config/hardware/hw_config
    platform/config/software/mpfs_hal/mss_sw_config

The "hw_config" defines the hardware configurations such as clocks. You must 
make sure that the configurations in this example project match the actual 
configurations of your target design you are using to test this example project.

The "sw_config" folder defines the software configurations such as HART 
configurations. These configurations have no dependency on the hardware
configurations that you use. If you change these configurations, you may have 
to change the software code to work according to the new configurations that 
you use.

## Executing project in the Renode emulation

To launch the PolarFire Soc Icicle board emulation model on Renode from 
SoftConsole , launch the preconfigured external tool from
   Run -> External Tools -> "PolarFire-SoC-Renode-emulation-platform". 

This will also launch an UART terminal which works with Renode.

Build the project and launch the debug configuration named 
mpfs-rtc-interrupt renode all-harts start-platform-and-debug.launch which is 
configured for Renode. 

Please refer to Working_with_Renode.md file for more detailed description about
renode configuration.


## Executing project on PolarFire SoC hardware

The same application can be used on PolarFire SoC hardware platform. The MMUART0
must be connected to host PC. The host PC must connect to the serial port using 
a terminal emulator such as Tera Term or PuTTY.

Build the project and launch the debug configuration named 
mpfs-rtc-interrupt hw-emulation all-harts debug.launch which is configured 
for PolarFire SoC hardware emulation platform.
