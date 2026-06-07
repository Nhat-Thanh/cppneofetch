#!/usr/bin/env python3
"""Generate C headers from PCI.ids database file."""

import sys


class PciDeviceModel:
    """Represents a PCI device entry."""

    def __init__(self, id_: int, name: str):
        self.id = id_
        self.name = name


class PciVendorModel:
    """Represents a PCI vendor with its devices."""

    def __init__(self, id_: int, name: str):
        self.id = id_
        self.name = name
        self.devices: list[PciDeviceModel] = []

def get_substring_between(text: str, char_start: str, char_end: str) -> str:
    """Eextract subtring the first char_start and the last char_end"""
    start = text.find(char_start) + 1
    end = text.rfind(char_end)
    if start > 0:
        return text[start:end]
    return ""

def shorten_vendor_name(name: str) -> str:
    """Extract clean vendor name"""
    shorten_name = get_substring_between(name, '[', ']')
    if shorten_name == "":
        redundant_substrings = [
            "Inc",
            "Corporation",
            "Corp.",
            "Co.,Ltd",
            "Technology"
        ]
        shorten_name = name
        for substr in redundant_substrings:
            shorten_name = shorten_name.replace(substr, "")
        shorten_name = shorten_name.strip(",. ")
    return shorten_name
        

def parse_pci_ids(filename: str) -> list[PciVendorModel]:
    """Parse the pci.ids file and return a list of vendors with their devices."""
    with open(filename, 'r') as f:
        full_text = f.read()

    if not full_text.strip():
        sys.exit(f'Error: {filename} is empty')

    # Remove trailing sections (known classes) by finding the last double-newline
    dev_list_text = full_text[:full_text.rfind('\n\n\n')]

    vendors: list[PciVendorModel] = []

    for line in dev_list_text.split('\n'):
        # Skip empty lines and comments
        if not line or line[0] == '#':
            continue

        # Main vendor entries start at column 1 (tab)
        if len(line) > 1 and line[0] != '\t':
            id_str, name = line.split('  ', maxsplit=1)
            vendor = PciVendorModel(int(id_str, 16), shorten_vendor_name(name))
            vendors.append(vendor)

        # Device entries are indented with two tabs (or one tab then space)
        elif len(line) > 2 and line[0] == '\t' and line[1] != '\t':
            id_str, name = line[1:].split('  ', maxsplit=1)
            vendors[-1].devices.append(PciDeviceModel(int(id_str, 16), name))
                

    return vendors


def generate_c_code(vendors: list[PciVendorModel], keep_vendor_ids: set[int] | None = None) -> str:
    """Generate C header code for the PCI vendor/device database."""

    # Filter vendors if needed (e.g., only specific hardware manufacturers)
    if keep_vendor_ids is not None:
        filtered_vendors = [vendor for vendor in vendors if vendor.id in keep_vendor_ids]
    else:
        filtered_vendors = vendors[:]

    header = '''
// SPDX-License-Identifier: BSD-3-Clause
// https://opensource.org/license/BSD-3-Clause
// Generated from https://pci-ids.ucw.cz/v2.2/pci.ids

#ifndef _PCIIDS_HPP_
#define _PCIIDS_HPP_

#include <stdint.h>
#include <stddef.h>
#include <map>

typedef struct {
    const char* kpc_Name;
    const std::map<uint16_t, const char*>* kpMap_PciDevs;
} pci_vendor_t;
'''

    # Generate device arrays for each vendor that has at least one device
    for vendor in filtered_vendors:
        if vendor.devices:
            # Build the comma-separated list of device 
            piece = ',\n    '.join(
                '{{ 0x{:04X}, "{}" }}'.format(
                    device.id,
                    device.name.replace('"', '\\"')
                ) for device in vendor.devices
            )
            header += f'''
// {vendor.name}
static const std::map<uint16_t, const char*> skMap_PciDevs_{vendor.id:04X} = {{
    {piece},
}};
'''

    # Generate the final vendors array
    piece = ',\n    '.join(
        '{{ 0x{:04X}, {{ "{}", {} }} }}'.format(
            vendor.id,
            vendor.name.replace('"', '\\"'),
            f"&skMap_PciDevs_{vendor.id:04X}" if vendor.devices else "nullptr"
        ) for vendor in filtered_vendors
    )

    header += f'''
const std::map<uint16_t, pci_vendor_t> kMap_PciVendors = {{
    {piece},
}};
'''
    header += "\n#endif\n"

    return header


def main(keep_vendor_ids: set[int] | None, pci_ids_path: str) -> None:
    """Main entry point for generating PCI database C headers."""
    
    vendors = parse_pci_ids(pci_ids_path)
    header_path = "Utils/PciIds.hpp"
    fd = open(header_path, 'w')
    fd.write(generate_c_code(vendors, keep_vendor_ids))
    fd.close()

if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit('Usage: gen-pciids.py </path/to/pci.ids>')

    # Vendor IDs to include (common hardware manufacturers):
    KEEP_VENDOR_IDS = {
        0x106b,   # Apple
        0x1002,   # AMD
        0x1022,   # AMD legacy
        0x8086,   # Intel
        0x8087,   # Intel GMA
        0x03e7,   # ATI/AMD
        0x0955,   # Nvidia legacy
        0x10de,   # Nvidia
        0x12d2,   # Broadcom/Nvidia
        0x1ed5,   # MThreads
        0x5143,   # Qualcomm
        0x14c3,   # MediaTek
        0x15ad,   # VMware
        0x1af4,   # RedHat
        0x1ab8,   # Parallel
        0x1414,   # Microsoft
        0x108e,   # Oracle VM
    }

    main(KEEP_VENDOR_IDS, sys.argv[1])

