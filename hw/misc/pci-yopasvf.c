// SPDX-License-Identifier: <SPDX License Expression>

#include "pci-yopas.h"

#define TAG "yvf - "

struct yvf {
	/*< private >*/
	PCIDevice parent_obj;

	/*< public >*/
	MemoryRegion mmio;
};
OBJECT_DECLARE_SIMPLE_TYPE(yvf, YVF)

static uint64_t yvf_mmio_read(void *data, hwaddr addr, unsigned int size)
{
	return 0;
}

static void yvf_mmio_write(void *data, hwaddr addr, uint64_t val, unsigned int size)
{
}

static const MemoryRegionOps yvf_mmio_ops = {
	.read = yvf_mmio_read,
	.write = yvf_mmio_write,
	.endianness = DEVICE_LITTLE_ENDIAN,
	.impl = {
		.min_access_size = 4,
		.max_access_size = 4,
	},
};

static void yvf_write_config(PCIDevice *pci_dev,
	uint32_t addr, uint32_t val, int len)
{
	pci_default_write_config(pci_dev, addr, val, len);
	pcie_cap_flr_write_config(pci_dev, addr, val, len);
}

static void yvf_realize(PCIDevice *pci_dev, Error **errp)
{
	Y_UNUSED_PARAM(errp);

	int err;
	struct yvf *dev = YVF(pci_dev);

	qemu_log(TAG "start realizing...\n");

	pci_dev->config_write = yvf_write_config;

	memory_region_init_io(&dev->mmio, OBJECT(dev),
		&yvf_mmio_ops, dev, "yvf-mmio",
		YVF_MMIO_SIZE);

	pcie_sriov_vf_register_bar(pci_dev, YVF_MMIO_IDX, &dev->mmio);

	err = msi_init(pci_dev, 0x40, 1, true, false, errp);
	if (err < 0) {
		qemu_log(TAG "msi init failure: %d", err);
		goto _end;
	}

	err = pcie_endpoint_cap_init(pci_dev, YVF_CAP_OFFSET_INIT);
	if (err < 0)
		qemu_log(TAG "end point init failure: %d", err);

	pcie_cap_flr_init(pci_dev);

_end:
	return;
}

static void yvf_exit(PCIDevice *pci_dev)
{
	Y_UNUSED_PARAM(pci_dev);

	qemu_log(TAG "exiting...\n");
}

static void yvf_class_init(ObjectClass *oc, void *data)
{
	DeviceClass	*dc	= DEVICE_CLASS(oc);
	PCIDeviceClass	*k	= PCI_DEVICE_CLASS(oc);

	k->realize		= yvf_realize;
	k->exit			= yvf_exit;
	k->vendor_id		= Y_VENDOR_ID;
	k->device_id		= YVF_DEVICE_ID;
	k->revision		= 0x00;
	k->class_id		= PCI_CLASS_OTHERS;
	dc->desc		= "PCI yopasvf Device";

	set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

static const TypeInfo yvf_info = {
	.name		= TYPE_YVF,
	.parent		= TYPE_PCI_DEVICE,
	.instance_size	= sizeof(struct yvf),
	.class_init	= yvf_class_init,
	.interfaces	= (InterfaceInfo[]) {
		{ INTERFACE_PCIE_DEVICE },
		{ },
	},
};

static void yvf_register(void)
{
	type_register_static(&yvf_info);
}

type_init(yvf_register)
