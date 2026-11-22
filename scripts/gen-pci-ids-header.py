#!/usr/bin/env python3
"""Generate sorted constexpr C++20 headers from PCI.ids database file."""

from pathlib import Path
import re
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
        self.devices: dict[int, PciDeviceModel] = {}


def escape_c_string(text: str) -> str:
    """Escape backslashes and quotes for C++ string literals."""
    return text.replace('\\', '\\\\').replace('"', '\\"')


def get_substring_between(text: str, char_start: str, char_end: str) -> str:
    """Extract substring between the first char_start and the last char_end."""
    start = text.find(char_start)
    end = text.rfind(char_end)
    if start != -1 and end > start:
        return text[start + 1:end]
    return ""


def shorten_vendor_name(name: str) -> str:
    """Extract clean vendor name."""
    short_name = get_substring_between(name, '[', ']')
    if not short_name:
        redundant_substrings = [
            "Inc.",
            "Inc",
            "Corporation",
            "Corp.",
            "Co.,Ltd.",
            "Co.,Ltd",
            "Technology",
            "Systemberatung GmbH",
        ]
        short_name = name
        for substr in redundant_substrings:
            short_name = short_name.replace(substr, "")
        short_name = re.sub(r'\s+', ' ', short_name).strip(",. ")
    return short_name


def parse_pci_ids(filename: str) -> list[PciVendorModel]:
    """Parse the pci.ids file and return a list of vendors with their devices."""
    vendor_re = re.compile(r'^([0-9a-fA-F]{4})  (.+)$')
    device_re = re.compile(r'^\t([0-9a-fA-F]{4})  (.+)$')

    vendors: dict[int, PciVendorModel] = {}
    current_vendor: PciVendorModel | None = None

    with open(filename, 'r', encoding='utf-8', errors='replace') as f:
        for raw_line in f:
            line = raw_line.rstrip('\r\n')
            if not line or line.startswith('#'):
                continue
            if line.startswith('C '):
                break

            if m := vendor_re.match(line):
                vid = int(m.group(1), 16)
                vname = shorten_vendor_name(m.group(2))
                current_vendor = vendors.get(vid)
                if current_vendor is None:
                    current_vendor = PciVendorModel(vid, vname)
                    vendors[vid] = current_vendor
            elif current_vendor is not None and (m := device_re.match(line)):
                did = int(m.group(1), 16)
                dname = m.group(2)
                current_vendor.devices[did] = PciDeviceModel(did, dname)

    if not vendors:
        sys.exit(f'Error: no PCI vendors parsed from {filename}')

    return sorted(vendors.values(), key=lambda v: v.id)


def generate_c_code(vendors: list[PciVendorModel], keep_vendor_ids: set[int] | None = None) -> str:
    """Generate zero-allocation sorted constexpr C++20 lookup tables."""
    if keep_vendor_ids is not None:
        filtered_vendors = [v for v in vendors if v.id in keep_vendor_ids]
    else:
        filtered_vendors = list(vendors)

    filtered_vendors.sort(key=lambda v: v.id)

    lines = [
        "// SPDX-License-Identifier: BSD-3-Clause",
        "// https://opensource.org/license/BSD-3-Clause",
        "// Generated from https://pci-ids.ucw.cz/v2.2/pci.ids",
        "",
        "#ifndef CPPNEOFETCH_PCI_IDS_HPP",
        "#define CPPNEOFETCH_PCI_IDS_HPP",
        "",
        "#include <algorithm>",
        "#include <cstddef>",
        "#include <cstdint>",
        "#include <string_view>",
        "",
        "/**",
        " * @brief Represents a single PCI device entry in the generated table.",
        " */",
        "typedef struct {",
        "    uint16_t ui16_Id;",
        "    const char* kpc_Name;",
        "} pci_device_t;",
        "",
        "/**",
        " * @brief Represents a PCI vendor entry and its sorted device array.",
        " */",
        "typedef struct {",
        "    uint16_t ui16_Id;",
        "    const char* kpc_Name;",
        "    const pci_device_t* kp_Devices;",
        "    std::size_t ui64_DeviceCount;",
        "} pci_vendor_t;",
        "",
    ]

    for vendor in filtered_vendors:
        if vendor.devices:
            sorted_devs = sorted(vendor.devices.values(), key=lambda d: d.id)
            lines.append(f"// {vendor.name}")
            lines.append(f"inline constexpr pci_device_t k_PciDevs_{vendor.id:04X}[] = {{")
            for dev in sorted_devs:
                lines.append(f'    {{ 0x{dev.id:04X}, "{escape_c_string(dev.name)}" }},')
            lines.append("};")
            lines.append("")

    lines.append("inline constexpr pci_vendor_t k_PciVendors[] = {")
    for vendor in filtered_vendors:
        dev_ptr = f"k_PciDevs_{vendor.id:04X}" if vendor.devices else "nullptr"
        dev_cnt = len(vendor.devices)
        lines.append(
            f'    {{ 0x{vendor.id:04X}, "{escape_c_string(vendor.name)}", {dev_ptr}, {dev_cnt} }},'
        )
    lines.extend([
        "};",
        "",
        "/**",
        " * @brief Look up a PCI vendor and device name using binary search.",
        " *",
        " * @param ui16_VendorId - 16-bit PCI vendor ID",
        " * @param ui16_DeviceId - 16-bit PCI device ID",
        " * @param rsv_VendorName - Output vendor name view",
        " * @param rsv_DeviceName - Output device name view",
        " *",
        " * @return true if both vendor and device IDs were found, false otherwise",
        " */",
        "inline bool fb_LookupPciDevice(uint16_t ui16_VendorId,",
        "                               uint16_t ui16_DeviceId,",
        "                               std::string_view& rsv_VendorName,",
        "                               std::string_view& rsv_DeviceName) {",
        "    constexpr std::size_t kui64_VendorCount = sizeof(k_PciVendors) / sizeof(k_PciVendors[0]);",
        "    const pci_vendor_t* kp_VendorEnd = k_PciVendors + kui64_VendorCount;",
        "    const pci_vendor_t* kp_VendorIt = std::lower_bound(",
        "        k_PciVendors, kp_VendorEnd, ui16_VendorId,",
        "        [](const pci_vendor_t& kr_Entry, uint16_t ui16_Id) {",
        "            return kr_Entry.ui16_Id < ui16_Id;",
        "        });",
        "    if (kp_VendorEnd == kp_VendorIt || ui16_VendorId != kp_VendorIt->ui16_Id || nullptr == kp_VendorIt->kp_Devices) {",
        "        return false;",
        "    }",
        "    const pci_device_t* kp_DevEnd = kp_VendorIt->kp_Devices + kp_VendorIt->ui64_DeviceCount;",
        "    const pci_device_t* kp_DevIt = std::lower_bound(",
        "        kp_VendorIt->kp_Devices, kp_DevEnd, ui16_DeviceId,",
        "        [](const pci_device_t& kr_Entry, uint16_t ui16_Id) {",
        "            return kr_Entry.ui16_Id < ui16_Id;",
        "        });",
        "    if (kp_DevEnd == kp_DevIt || ui16_DeviceId != kp_DevIt->ui16_Id) {",
        "        return false;",
        "    }",
        "    rsv_VendorName = kp_VendorIt->kpc_Name;",
        "    rsv_DeviceName = kp_DevIt->kpc_Name;",
        "    return true;",
        "}",
        "",
        "#endif // CPPNEOFETCH_PCI_IDS_HPP",
        "",
    ])

    return "\n".join(lines)


def main(keep_vendor_ids: set[int] | None, pci_ids_path: str, output_path: str) -> None:
    """Main entry point for generating PCI database C++ headers."""
    vendors = parse_pci_ids(pci_ids_path)
    out = Path(output_path)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(generate_c_code(vendors, keep_vendor_ids), encoding='utf-8')


if __name__ == '__main__':
    if len(sys.argv) not in (2, 3):
        sys.exit('Usage: gen-pci-ids-header.py <path/to/pci.ids> [output_header_path]')

    KEEP_VENDOR_IDS = {
        0x03e7,   # Movidius / Intel
        0x0955,   # NVIDIA legacy
        0x1002,   # AMD/ATI
        0x1022,   # AMD
        0x102b,   # Matrox
        0x106b,   # Apple
        0x108e,   # Oracle
        0x10de,   # NVIDIA
        0x1234,   # QEMU / Bochs VGA
        0x126f,   # Silicon Motion
        0x12d2,   # SGS Thomson / NVIDIA
        0x1414,   # Microsoft Hyper-V
        0x14c3,   # MediaTek
        0x15ad,   # VMware
        0x1a03,   # ASPEED
        0x1ab8,   # Parallels
        0x1af4,   # Red Hat / VirtIO
        0x1d0f,   # Amazon
        0x1ed5,   # Moore Threads
        0x5143,   # Qualcomm
        0x8086,   # Intel
        0x8087,   # Intel GMA
        0x80ee,   # VirtualBox / InnoTek
    }

    out_path = sys.argv[2] if len(sys.argv) == 3 else "build/generated/PciIds.hpp"
    main(KEEP_VENDOR_IDS, sys.argv[1], out_path)
