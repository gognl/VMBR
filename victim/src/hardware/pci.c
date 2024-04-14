#include <hardware/pci.h>
#include <lib/types.h>
#include <lib/instr.h>

void set_pci_device_register(pci_device_t dev, uint8_t register_id, pci_device_register_t value){
    pci_config_address_t config_address = {0};
    config_address.bus = dev.bus;
    config_address.device = dev.device;
    config_address.function = dev.function;
    config_address.register_id = register_id;
    config_address.enable = TRUE;

    __outl(PCI_CONFIG_ADDRESS, config_address.value);
    __outl(PCI_CONFIG_DATA, value.value);

}

pci_device_register_t get_pci_device_register(pci_device_t dev, uint8_t register_id){
    pci_config_address_t config_address = {0};
    config_address.bus = dev.bus;
    config_address.device = dev.device;
    config_address.function = dev.function;
    config_address.register_id = register_id;
    config_address.enable = TRUE;

    __outl(PCI_CONFIG_ADDRESS, config_address.value);

    pci_device_register_t reg;
    reg.value = __inl(PCI_CONFIG_DATA);
    return reg;
}

STATUS scan_for_device(pci_device_t *dev_ptr, uint16_t vendor_id, uint16_t device_id, uint8_t bus, uint8_t device, uint8_t function){

    dev_ptr->bus = bus;
    dev_ptr->device = device;
    dev_ptr->function = function;
    
    pci_device_register_t reg = get_pci_device_register(*dev_ptr, 0);
    if (reg.register_0.vendor_id == vendor_id && reg.register_0.device_id == device_id){
        return SUCCESS;
    }

    return FAIL;
}

STATUS get_pci_device(pci_device_t *dev_ptr, uint16_t vendor_id, uint16_t device_id){
    LOG_DEBUG("Searching for device (%x, %x)...\n", vendor_id, device_id);
    uint8_t bus, device, function;
    STATUS found = FAIL;
    for (bus = 0; bus < PCI_BUS_COUNT; bus++){
        for (device = 0; device < PCI_DEVICE_COUNT; device++){
            for (function = 0; function < PCI_FUNCTION_COUNT; function++){
                found = scan_for_device(dev_ptr, vendor_id, device_id, bus, device, function);
                if (found) return SUCCESS;
            }
        }
    }

    // Device not found
    return FAIL;
}
