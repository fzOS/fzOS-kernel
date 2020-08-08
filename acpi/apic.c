#include <apic.h>
#include <printk.h>
int number_of_processors;
int parse_apic(U8* in) {
    if(validate_table(in)) {
        printk(" Error:broken apic.\n");
        return -1;
    }
    APICHeader* header = (APICHeader*) in;
    debug(" Local APIC address:%x\n",header->LocalAPICAddress);
    if(header->Flags) {
        debug(" APIC:Multi 8259 controller enabled.\n");
    }
    //循环处理APIC。
    U8* position = in+sizeof(APICHeader);
    while(position-in<header->Common.Length) {
        switch(*position) {
            case 0: { //Processor Local APIC
                ProcessorLocalApic* table = (ProcessorLocalApic*)position;
                if(table->Flags) {
                    debug(" Processor#%d :%b\n",number_of_processors++,table->APICID);
                }
                position += sizeof(ProcessorLocalApic);
                break;
            }
            case 1: { //I/O APIC
                IOApic* table = (IOApic*)position;
                printk(" Found I/O APIC at %x.\n",table->IOAPICAddr);
                position += sizeof(IOApic);
                break;
            }
            case 2: { //Interrupt Source Override
                position += 10;
                break;
            }
            case 3: { //Non-maskable Interrupt Source
                
                break;
            }
            case 4: { //Local APIC NMI
                
                break;
            }
            case 5: { //Local APIC Address Override
                
                break;
            }
            case 6: { //I/O SAPIC
                
                break;
            }
            case 7: { //Local SAPIC
                
                break;
            }
            case 8: { //Platform Interrupt Sources
                
                break;
            }
            case 9: { //Processor Local x2PIC
                
                break;
            }
            case 0xA: { //Local x2APIC NMI
                
                break;
            }
            case 0xB: { //GIC CPU Interface
                
                break;
            }
            case 0xC: { //GIC Distributor
                
                break;
            }
            case 0xD: { //GIC MSI Frame
                
                break;
            }
            case 0xE: { //GIC Redistributor
                
                break;
            }
            case 0xF: { //GIC Interrupt Translation Service
                
                break;
            }
        }
    }
    return 0;
}