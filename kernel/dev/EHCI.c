#include "../kernel.h"

extern void mouseirq();

void irq_ehci(){
	printf("EHCI:           --- INTERRUPT --- \n");
	
	outportb(0x20,0x20);
	outportb(0xA0,0x20);
}

void ehci_init(int bus,int slot,int function){
	unsigned long baseaddr 		= getBARaddress(bus,slot,function,0x10);
	unsigned long HCIVERSION 	= baseaddr+2;
	unsigned long HCSPARAMS 	= baseaddr+4;
	unsigned long virtregaddr 	= baseaddr;
	virtregaddr 			+= ((unsigned char*)baseaddr)[0x00];
	unsigned long USBCMD 		= virtregaddr+0x00;
	unsigned long USBSTS 		= virtregaddr+0x04;
	unsigned long USBINTR 		= virtregaddr+0x08;
	unsigned long FRINDEX 		= virtregaddr+0x0C;
	unsigned long CTRLDSSEGMENT 	= virtregaddr+0x10;
	unsigned long PERIODICLISTBASE 	= virtregaddr+0x14;
	unsigned long ASYNCLISTADDR 	= virtregaddr+0x18;
	unsigned long CONFIGFLAG 	= virtregaddr+0x40;
	printf("EHCI: detected at addr %x !\n",baseaddr);
	resetTicks();
	((unsigned long*)USBCMD)[0] = 0x00080002;
	while(((unsigned long*)USBCMD)[0] == 0x00080002){
		if(getTicks()==5){
			break;
		}
	}
	((unsigned long*)USBINTR)[0] = 0b0111111;
	if(pciConfigReadWord(bus,slot,function,0x10)&0b110){
		printf("EHCI: warning 64bit possible\n");
	}
	int intnummer = pciConfigReadWord ( bus, slot, function, 0x3c ) & 0xFF;
	setNormalInt(intnummer,(unsigned long)mouseirq);
	printf("EHCI: assigned int number %x \n",intnummer);
	printf("EHCI: serial release number %x \n",pciConfigReadWord(bus,slot,function,0x60)&0xFF);
	printf("EHCI: framelength %x \n",pciConfigReadWord(bus,slot,function,0x61)&0xFF);
	printf("EHCI: portwake %x \n",pciConfigReadWord(bus,slot,function,0x62)&0xFFFF);
	unsigned long hccparams = ((unsigned long*)baseaddr)[0x08];
	unsigned long EECP = (hccparams & 0b1111111100000000)>>8;
	printf("EHCI: hcc params %x\n",hccparams);
	if(EECP){
		printf("EHCI: EHCI Extended Capabilities Pointer Enabled!\n");
		printf("EHCI: USB Legacy Support Extended Capability %x \n",getBARaddress(bus,slot,function,EECP));
		printf("EHCI: USB Legacy Support Control/Status %x \n",getBARaddress(bus,slot,function,EECP+4));
	}
	printf("EHCI: Capability Registers Length %x \n",((unsigned char*)baseaddr)[0x00]);
	printf("EHCI: Host Controller Interface Version Number %x \n",((unsigned short*)HCIVERSION)[0x00]);
	printf("EHCI: Structural Parameters %x \n",((unsigned long*)HCSPARAMS)[0x00]);
	printf("EHCI: Number of avail ports: %x \n",((unsigned long*)HCSPARAMS)[0x00]&0b01111);
	printf("EHCI: USBCMD %x \n",((unsigned long*)USBCMD)[0]);
	printf("EHCI: USBSTS %x \n",((unsigned long*)USBSTS)[0]);
	printf("EHCI: USBINTR %x \n",((unsigned long*)USBINTR)[0]);
	((unsigned long*)USBCMD)[0] |= 1;printf("EHCI:debugA\n");
	((unsigned long*)CONFIGFLAG)[0] = 1;printf("EHCI:debugB\n");
	printf("EHCI: USBCMD %x \n",((unsigned long*)USBCMD)[0]);
	printf("EHCI: USBSTS %x \n",((unsigned long*)USBSTS)[0]);
	printf("EHCI: USBINTR %x \n",((unsigned long*)USBINTR)[0]);
	printf("END OF OPERATIONS");
	for(;;);
}
