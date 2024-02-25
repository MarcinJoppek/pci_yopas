/* SPDX-License-Identifier: <SPDX License Expression> */

#ifndef _PCI_YOPAS_H
#define _PCI_YOPAS_H

#include "qemu/osdep.h"
#include "qemu/units.h"
#include "hw/pci/pci_device.h"
#include "hw/pci/pcie.h"
#include "hw/pci/pcie_sriov.h"
#include "hw/pci/msi.h"
#include "qemu/module.h"
#include "sysemu/kvm.h"
#include "qom/object.h"

#include "qemu/log.h"

#define Y_UNUSED_PARAM(x)	((void)x)

/* PF */
#define TYPE_Y			"yopas"
#define Y_VENDOR_ID		0xdead
#define Y_DEVICE_ID		0xbeef

#define Y_MMIO_IDX		0
#define Y_MMIO_SIZE		(16 * KiB)

#define Y_CAP_OFFSET_INIT	0x50
#define Y_CAP_OFFSET_SRIOV	0x100

#define Y_VF_MAX		8
#define Y_VF_OFFSET		8
#define Y_VF_STRIDE		1

/* VF */
#define TYPE_YVF		"yopasvf"
#define YVF_DEVICE_ID		0xfeed

#define YVF_MMIO_IDX		0
#define YVF_MMIO_SIZE		(16 * KiB)

#define YVF_CAP_OFFSET_INIT	0x50

#endif /* _PCI_YOPAS_H */
