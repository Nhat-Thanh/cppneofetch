#ifndef _DEFINE_HPP_
#define _DEFINE_HPP_

#define CHAR_ENDSTR '\0'
#define CHAR_QUOTE  '\"'

#define UINT32_INIT 0xFFFFFFFF
#define UINT64_INIT 0xFFFFFFFFFFFFFFFF

#define STR(x) #x

#define SYSCALL_SUCCESS 0
#define SYSCALL_FAILED -1

#define PATH_PROC_STAT_FORMAT "/proc/%d/stat"
#define PATH_PROC_COMM_FORMAT "/proc/%d/comm"
#define PATH_SYS_PCI_DEV_BUS  "/sys/bus/pci/devices"

#define PCI_CLASS_ID_DISPLAY_CTRL 0x00000003
#endif
