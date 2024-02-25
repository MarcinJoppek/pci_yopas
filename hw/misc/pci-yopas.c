// SPDX-License-Identifier: <SPDX License Expression>

#include "pci-yopas.h"

#define TAG "y - "

struct y {
	/*< private >*/
	PCIDevice parent_obj;

	/*< public >*/
	MemoryRegion mmio;
};
OBJECT_DECLARE_SIMPLE_TYPE(y, Y)

static uint64_t y_mmio_read(void *data, hwaddr addr, unsigned int size)
{
	return 0;
}

static void y_mmio_write(void *data, hwaddr addr, uint64_t val, unsigned int size)
{
}

static const MemoryRegionOps y_mmio_ops = {
	.read = y_mmio_read,
	.write = y_mmio_write,
	.endianness = DEVICE_LITTLE_ENDIAN,
	.impl = {
		.min_access_size = 4,
		.max_access_size = 4,
	},
};

static void y_write_config(PCIDevice *pci_dev,
	uint32_t addr, uint32_t val, int len)
{
	pci_default_write_config(pci_dev, addr, val, len);
	pcie_cap_flr_write_config(pci_dev, addr, val, len);
}

static void y_realize(PCIDevice *pci_dev, Error **errp)
{
	Y_UNUSED_PARAM(errp);

	int err;
	struct y *dev = Y(pci_dev);

	qemu_log(TAG "start realizing...\n");

	pci_dev->config_write = y_write_config;

	memory_region_init_io(&dev->mmio, OBJECT(dev),
		&y_mmio_ops, dev, "y-mmio",
		Y_MMIO_SIZE);

	pci_register_bar(pci_dev, Y_MMIO_IDX,
		PCI_BASE_ADDRESS_SPACE_MEMORY,
		&dev->mmio);


	pci_config_set_interrupt_pin(pci_dev->config, 1);

	err = msi_init(pci_dev, 0x40, 1, true, false, errp);
	if (err < 0) {
		qemu_log(TAG "msi init failure: %d", err);
		goto _end;
	}

	err = pcie_endpoint_cap_init(pci_dev, Y_CAP_OFFSET_INIT);
	if (err < 0) {
		qemu_log(TAG "end point init failure: %d", err);
		goto _end;
	}

	pcie_cap_flr_init(pci_dev);

	pcie_sriov_pf_init(pci_dev, Y_CAP_OFFSET_SRIOV,
		TYPE_YVF, YVF_DEVICE_ID, Y_VF_MAX, Y_VF_MAX,
		Y_VF_OFFSET, Y_VF_STRIDE);

	pcie_sriov_pf_init_vf_bar(pci_dev, YVF_MMIO_IDX,
		PCI_BASE_ADDRESS_MEM_TYPE_64 | PCI_BASE_ADDRESS_MEM_PREFETCH,
		YVF_MMIO_SIZE);
_end:
	return;
}

static void y_exit(PCIDevice *pci_dev)
{
	Y_UNUSED_PARAM(pci_dev);

	qemu_log(TAG "exiting...\n");
}

static void y_class_init(ObjectClass *oc, void *data)
{
	DeviceClass	*dc	= DEVICE_CLASS(oc);
	PCIDeviceClass	*k	= PCI_DEVICE_CLASS(oc);

	k->realize		= y_realize;
	k->exit			= y_exit;
	k->vendor_id		= Y_VENDOR_ID;
	k->device_id		= Y_DEVICE_ID;
	k->revision		= 0x00;
	k->class_id		= PCI_CLASS_OTHERS;
	dc->desc		= "PCI yopas Device";

	set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

static const TypeInfo y_info = {
	.name		= TYPE_Y,
	.parent		= TYPE_PCI_DEVICE,
	.instance_size	= sizeof(struct y),
	.class_init	= y_class_init,
	.interfaces	= (InterfaceInfo[]) {
		{ INTERFACE_PCIE_DEVICE },
		{ },
	},
};

static void y_register(void)
{
	type_register_static(&y_info);
}

type_init(y_register)
