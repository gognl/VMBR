# VMBR (Virtual-Machine Based Rootkit)
This project includes a malicious type 1 hypervisor (VMM) which runs a Windows guest. The VMM collects keylogs from the guest, and sends them over the network to an attacker.

## Running the project
1. Clone the repository.
	```
	git clone https://github.com/gognl/VMBR.git
	```
3. In order to run the project you must acquire a Windows ISO that is compatible with Legacy BIOS.  This is a little complicated. You can follow the steps described in [this article](https://raywoodcockslatest.wordpress.com/2021/08/17/linux-v2p/#ICa "this article") in order to get the ISO. I have also summerized the steps:
	1. Download [Virtualbox](https://www.virtualbox.org/ "Virtualbox"), [Rufus](https://rufus.ie/ "Rufus"), and [Clonezilla](https://clonezilla.org/downloads.php "Clonezilla"). Also, get a (UEFI) Windows ISO.
	2. In Virtualbox, create a new VM using your UEFI Windows ISO. 
	3. Virtualbox might have already automatically convereted the ISO to Legacy BIOS. If not, you can change the boot to Legacy BIOS through the [Windows settings](https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/boot-to-uefi-mode-or-legacy-bios-mode?view=windows-11 "Windows settings").
	4. Shut down the VM, and change its settings so that it boots from the Clonezilla ISO (a detailed explanation regarding how to do it is available in the article provided above).
	5. Use the clonezilla interface to load the disk to an external USB (keep in mind that its contents will be deleted).
	6. Use Rufus to convert the bootable USB to a VHD file.

	After getting the Legacy BIOS ISO, rename it to `win10.vhd` and place it in the [victim](https://github.com/gognl/VMBR/tree/main/victim "victim") folder.

4. Run [run.sh](https://github.com/gognl/VMBR/blob/main/run.sh "run.sh")

Starting the attacker will initiate scanning for victim machines. Those machines have to be up and running (and almost done booting) in order for the attacker to find them.

## Limitations
This project is supposed to support all (Legacy BIOS bootable) Windows versions using at least NDIS 6.0 (all versions since Windows Vista), but was only tested on Windows 10 22H2 build 19045.2965.

Regarding the network structure, the victim can be behind a NAT, but the attacker has to be accessible over the network (not behind a NAT). The attacker's IP can be changed in [hooking.h](https://github.com/gognl/VMBR/blob/main/victim/include/hooks/hooking.h "hooking.h").

This project must run over a 64bit Intel CPU that supports VT-x.

## Design
The project utilizes Windows functions in order to receive the keylogs and send them over the network. The functions used are:
- [KeyboardClassServiceCallBack](https://learn.microsoft.com/en-us/previous-versions/ff542324(v=vs.85) "KeyboardClassServiceCallBack") - for collecting keylogs
- [NdisMIndicateReceiveNetBufferLists](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ndis/nf-ndis-ndismindicatereceivenetbufferlists "NdisMIndicateReceiveNetBufferLists") - for capturing incoming packets
- ndisMSendNBLToMiniportInternal (undocumented) - for sending packets

They are all hooked using INT3 and memory shadowing (to protect the hooks). The process of locating them includes:
1. Hooking the LSTAR MSR, and using it to find the base of ntoskrnl (scanning backwards and looking for the correct PE header).
2. Finding PsLoadedModuleList by signature-scanning ntoskrnl for a function that references it (MiObtainSectionForDriver).
3. Iterating over PsLoadedModuleList to find ndis.sys and kbdclass.sys.
4. Locating the relevant functions by signature-scanning in the appropriate drivers.
