================================================================================
                    PolarFire SoC MSS GPIO example
================================================================================

This example project demonstrates the use of the PolarFire SoC MSS GPIO block. 
Both the input and output port configurations are implemented along with the 
interrupt handling for the input ports.

--------------------------------------------------------------------------------
                            How to use this example
--------------------------------------------------------------------------------
To use this project you will need a UART terminal configured as below:
    - 115200 baud
    - 8 data bits
    - 1 stop bit
    - no parity
    - no flow control

The example project will display instructions over the serial port. A greeting
message and menu instructions are displayed over the UART. Follow the instruction 
to use different menu options provided by the example project.

--------------------------------------------------------------------------------
                                Target hardware
--------------------------------------------------------------------------------
This example project can be used on the PolarFire SoC model on Renode emulation 
platform as well as PolarFire SoC FPGA family hardware platforms. 

There are configurations that needs to be set for this example project. The
configurations are categorized into hardware and software configurations and 
are respectively located in the following folders
    ./mpfs_config/hw_config
    ./mpfs_config/sw_config

The "hw_config" defines the hardware configurations such as clocks. You must 
make sure that the configurations in this example project match the actual 
configurations of your target design you are using to test this example project.

The "sw_config" folder defines the software configurations such as HART 
configurations. These configurations have no dependency on the hardware
configurations that you use. If you change these configurations, you may have 
to change the software code to work according to the new configurations that 
you use.

## Executing project in the Renode emulation

To launch the PolarFire SoC Icicle board emulation model on Renode from 
SoftConsole , launch the preconfigured external tool from
   Run -> External Tools -> "PolarFire-SoC-Icicle-Renode-emulation-platform". 

This will also launch an UART terminal which works with Renode.

Build the project and launch the debug configuration named 
"mpfs-gpio-example renode start-platform-and-debug.launch" which is configured 
to start Renode and launch debug session connecting to hart0. 

For more information, refer Working_with_Renode.md copied in the root folder of
this project.

## Executing project on PolarFire SoC hardware

The same application can be used on PolarFire SoC hardware platform. The MMUART0
must be connected to host PC. The host PC must connect to the serial port using 
a terminal emulator such as Tera Term or PuTTY.

Build the project and launch the debug configuration named 
mpfs-gpio-example hw-emulation all-harts debug.launch which is configured for 
PolarFire SoC hardware platform.