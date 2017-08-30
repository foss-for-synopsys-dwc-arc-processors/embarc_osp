# OpenThread NCP Example

This application is designed to show how to use the OpenThread Network Co-Processor (NCP) example in embARC. It can serve as a low-power wireless Network Co-Processor (NCP) to communicate with Userspace WPAN Network Daemon (wpantund) on Unix-like operating systems. It can be used along with OpenThread wpantund to provide Internet connectivity to the Thread network.

## Hardware and Software Setup
### Required Hardware
- 2 x [DesignWare ARC EM Starter Kit(EMSK)][1], recommended version 2.3
- 2 x [Digilent Pmod RF2][2]
- 2 x SD card
- 1 x [Raspberry Pi 3][3] or PC for running [OpenThread wpantund][4]

### Required Software
- Metaware or ARC GNU Toolset
- Serial port terminal, such as putty, tera-term or minicom
- [OpenThread wpantund][4]

### Hardware Connection
- Connect PmodRF2 to J6.

![EMSK Connections][30]

- Configure your hardware with proper core configuration. ARCEM7D on EMSK 2.3 is used as an example in the following document.

## User Manual
## Run OpenThread in self-boot mode
Two EMSKs will be used as two Thread nodes, one is CLI Node and the other is NCP Node. They are set automatically by OpenThread stack. The following example is to run the OpenThread CLI and NCP in the EMSK self-boot mode with using SD card.

- The Digilent PmodRF2 and the self-made MRF24J40 module are both supported.
- Use AC adapter to ensure a steady power supply.
- Open two Tera Term emulators to connect EMSKs with different COM ports.
- The self-boot mode preparation is included in the following steps.
- **make run** is not supported because both EMSK board is v2.3 and it will lead to conflict.

## Build and Run OpenThread example

1. Program the secondary bootloader application into onboard SPI Flash.
    - Go to `\embARC\example\baremetal\bootloader` in command line.
    - Enter **make TOOLCHAIN=gnu BD_VER=23 CUR_CORE=arcem7d bin**
        - Select configuration 1 on the EMSK for ARCEM7D by setting all DIP switches to the OFF position.
    - Insert the SD card to your PC, and copy the binary file `obj_emsk_23/gnu_arcem7d/emsk_bootloader_gnu_arcem7d.bin` to the SD Card root directory.  Rename it to `em7d_2bt.bin` as the secondary bootloader.
        - The bootloader has been modified in embARC. Please perform the following steps to update the bootloader in the EMSK SPI flash.
        - Update the bootloader in both EMSK boards. The other EMSK board should be updated by performing the same steps.
    - Connect one EMSK board to the host with USB. Insert the SD Card into the EMSK board SD Card slot.
        - Go to `\embARC\example\freertos\net\ntshell` in command line. Enter **make TOOLCHAIN=gnu BD_VER=23 CUR_CORE=arcem7d**. Once the application builds successfully, enter **make TOOLCHAIN=gnu BD_VER=23 CUR_CORE=arcem7d run** in command line to start NT-Shell.
    - Use NT-Shell command **spirw** in the terminal window to write the `em7d_2bt.bin` to SPI flash.
        - run **spirw -h** to show help.
        - run **spirw -w em7d_2bt.bin 0x17f00000 0x17f00004** to program SPI flash.
        - Check the output message to see if it is programmed successfully. The process may take some time. If `WF INIT FAILED` and `MRF24G Init Failed` is shown in the Tera Term, there is no error since the WiFi module has not been used here.
        
        ![spirw_em7d_2bt][31]

2. Generate `boot.bin` of the OpenThread CLI and NCP example.
    - Go to `\embARC\example\baremetal\openthread\cli` and `\embARC\example\baremetal\openthread\ncp` in command line.
    - Enter **make TOOLCHAIN=gnu BD_VER=23 CUR_CORE=arcem7d bin**.
    - Insert SD Card to PC. Copy the generated binary file `obj_emsk_23/gnu_arcem7d/openthread_gnu_arcem7d.bin` to SD cards root. And rename it to `boot.bin`. Note that the secondary bootloader can only identify `boot.bin` in the SD card root.

3. Run OpenThread CLI and NCP example. Before resetting the EMSK boards, make sure Bit 4 of the onboard DIP switch is ON to enable secondary bootloader to run.
    - Insert SD Card back to one EMSK. It will run the **CLI** example. Press the reset button to reboot it. Wait for loading boot.bin from SD card. The response in the terminal window is shown as below.
    
        ![enter_no_cli_emsk][32]
    
    - Insert the other SD card to the other EMSK. It will run the **NCP** example. Press the reset button to reboot it. Wait for loading boot.bin from SD card.

        ![enter_no_ncp_emsk][33]
    
    - Enter **1** and press Enter button in CLI's Tera Term. Enter **2** and press Enter button in the NCP's. Enter the number here to generate pseudo random number for OpenThread. Recommend to enter numbers in order, such as **1**, **2** and **3**. Using same number in different nodes may lead error. The number will not be shown directly in the Tera Term until pressing Enter button from the keyboard.

    - After entering **2** in the NCP's Tera Term, the NCP will start up automatically. It will show you some messy code in the last line, just ignore it. And after that this Tera Term won't be used any more.
    
        ![show_no_emsk][34]
    
    - Enter the following commands in the CLI's Tera Term windows.

            > panid 0x1234
            > ifconfig up
            > thread start

    - Wait 20 seconds for completing Thread configuration. Enter “state” to see the state of the node, it will be the **leader**.
    
        ![start_cli_emsk][35]

## Install and Run OpenThread wpantund

1. Download [OpenThread wpantund][4]. Install wpantund on the Linux host. Here we take the [Ubuntu Desktop 16.04.3 LTS][5] for example.

    - Install python-software-properties

            $ sudo apt-get install -y python-software-properties
            $ sudo add-apt-repository -y ppa:terry.guo/gcc-arm-embedded
            $ sudo apt-get update -qq

    - Install packages needed for wpantund build and runtime

            $ sudo apt-get install -y build-essential git make autoconf autoconf-archive automake dbus libtool gcc g++ gperf flex bison texinfo ncurses-dev libexpat-dev python sed python-pip gawk libreadline6-dev libreadline6 libdbus-1-dev libboost-dev
            $ sudo apt-get install -y --force-yes gcc-arm-none-eabi
            $ sudo pip install pexpect

    - Install *wpantund*

            $ cd wpantund
            $ ./configure --sysconfdir=/etc
            $ make
            $ sudo make install

    See [Simulation Codelab][6] for more detailed information.

2. Connect NCP to the Linux host via USB cable. Type **ls /dev/** to find out which descriptor stands for NCP's UART interface. Here it is `ttyUSB1`. The communication between NCP and wpantund is based on spinel protocol.

3. Run OpenThread wpantund and join the network created by CLI Node.

    - Run a terminal and type the following commands to start wpantund:

            sudo /usr/local/sbin/wpantund -o Config:NCP:SocketPath "/dev/ttyUSB1" -o Daemon:SyslogMask " -info" -o Config:TUN:InterfaceName utun6

        ![start_wpantund_pc][36]

    - Run another terminal and type the following commands to enter wpanctl:

            sudo /usr/local/bin/wpanctl -I utun6

        ![start_wpanctl_pc][37]

    - In the wpanctl, type the following commands to scan and join the Thread network:

            wpanctl:utun6> scan
            wpanctl:utun6> set Network:Key --data masterkey
            wpanctl:utun6> join 1

        ![join_wpanctl_pc][38]

    - Run a terminal on PC and type **ifconfig**, it will show you some information about a virtual IPv6 network interface (utun6):

        ![show_ni_pc][39]

    Note the IPv6 address, here it is `fdde:ad00:beef:0:4f6e:7e53:67c8:f5b0`.

## Ping to each other, the CLI and Linux Host

- Go to the CLI's Tera Term. Type **ipaddr** and note it's IPv6 address, here it is `fdde:ad00:beef:0:63a8:7376:c6ad:828c`. Type **ping fdde:ad00:beef:0:4f6e:7e53:67c8:f5b0**, it will ping the Linux host from CLI:

    ![ping_pc_from_cli][40]

- Go to the Linux host, run a terminal and type **ping6 fdde:ad00:beef:0:63a8:7376:c6ad:828c**, it will ping the CLI from Linux host:

    ![ping_cli_from_pc][41]

[1]: https://www.synopsys.com/dw/ipdir.php?ds=arc_em_starter_kit "DesignWare ARC EM Starter Kit(EMSK)"
[2]: http://store.digilentinc.com/pmod-rf2-ieee-802-15-rf-transceiver/ "Digilent Pmod RF2"
[3]: https://www.raspberrypi.org/products/raspberry-pi-3-model-b/ "Raspberry Pi 3"
[4]: https://github.com/openthread/wpantund/tree/full/latest-release "OpenThread wpantund"
[5]: https://www.ubuntu.com/download/desktop "Ubuntu Desktop 16.04.3 LTS"
[6]: https://codelabs.developers.google.com/codelabs/openthread-simulation/index.html "Simulation Codelab"

[30]: doc/screenshots/emsk_connections.jpg "EMSK Connections"
[31]: doc/screenshots/spirw_em7d_2bt.jpg "spirw em7d_2bt"
[32]: doc/screenshots/enter_no_cli_emsk.PNG "enter_no_cli_emsk"
[33]: doc/screenshots/enter_no_ncp_emsk.PNG "enter_no_ncp_emsk"
[34]: doc/screenshots/show_no_emsk.PNG "show_no_emsk"
[35]: doc/screenshots/start_cli_emsk.PNG "start_cli_emsk"
[36]: doc/screenshots/start_wpantund_pc.png "start_wpantund_pc"
[37]: doc/screenshots/start_wpanctl_pc.png "start_wpanctl_pc"
[38]: doc/screenshots/join_wpanctl_pc.png "join_wpanctl_pc"
[39]: doc/screenshots/show_ni_pc.png "show_ni_pc"
[40]: doc/screenshots/ping_pc_from_cli.PNG "ping_pc_from_cli"
[41]: doc/screenshots/ping_cli_from_pc.png "ping_cli_from_pc"