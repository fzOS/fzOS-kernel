#include <lai/host.h>
#include <memory/memory.h>
#include <common/kstring.h>
#include <acpi/xsdt.h>
#include <common/printk.h>
#include <common/io.h>
#include <drivers/pci.h>
void *laihost_malloc(size_t size)
{
    return memalloc(size);
}
void *laihost_realloc(void * orig, size_t new)
{
    if(orig) {
        U64 orig_size = *(((U64*)orig)-1);
        void* newaddr = memalloc(new);
        //由于可能存在新地址小于源地址的情况，我们应该在这里选择较小的长度拷贝。
        memcpy(newaddr,orig,(orig_size<new?orig_size:new));
        return newaddr;
    }
    return memalloc(new);
}
void laihost_free(void  *orig)
{
    memfree(orig);
}
void *laihost_map(size_t address, size_t count)
{
    (void)count;
    return (void*)(address | KERNEL_ADDR_OFFSET);
}
void laihost_unmap(void *pointer, size_t count)
{
    (void)count;
    (void)pointer;
}
void *laihost_scan(char *sig, size_t index)
{
    //我们现在只支持一张表（Virtualbox只有一张233333）
    if(index) {
        return nullptr;
    }
    return acpi_get_table_by_name(sig);
}
void laihost_log(int level, const char *msg)
{
    if(level==LAI_WARN_LOG) {
        debug("lai:");
        debug((char*)msg);
        debug("\n");
    }
}
__attribute__((noreturn)) void laihost_panic(const char *msg)
{
    debug(" lai:");
    debug((char*)msg);
    debug("\n");
    while(1) {
        halt();
    }
}
void laihost_outb(uint16_t port, uint8_t val)
{
    outb(port,val);
}
void laihost_outw(uint16_t port, uint16_t val)
{
    outw(port,val);
}
void laihost_outd(uint16_t port, uint32_t val)
{
    outl(port,val);
}

/* Read a byte/word/dword from the given I/O port. */
uint8_t laihost_inb(uint16_t port)
{
    return inb(port);
}
uint16_t laihost_inw(uint16_t port)
{
    return inw(port);
}
uint32_t laihost_ind(uint16_t port)
{
    return inl(port);
}
void laihost_sleep(uint64_t ms)
{
    return;
}

/* Read a byte/word/dword from the given device's PCI configuration space
   at the given offset. */
uint8_t laihost_pci_readb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset)
{
    return pci_read_byte(bus,slot,fun,offset);
}
uint16_t laihost_pci_readw(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset)
{
    return pci_config_read_word(bus,slot,fun,offset);
}
uint32_t laihost_pci_readd(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset)
{
    return pci_read_dword(bus,slot,fun,offset);
}
void laihost_pci_writeb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint8_t val)
{
    pci_write_byte(bus,slot, fun, offset,val);
}
void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint16_t val)
{
    pci_write_word(bus,slot, fun, offset,val);
}
void laihost_pci_writed(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint32_t val)
{
    pci_write_dword(bus,slot, fun, offset,val);
}

