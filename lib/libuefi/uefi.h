#ifndef __UEFI_H
#define __UEFI_H

#ifndef NULL
#define NULL ((void *)0)
#endif

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define UEFI_FILE_PROTOCOL_REVISION	   0x00010000 // Revision 1.0
#define UEFI_FILE_PROTOCOL_REVISION2	   0x00020000 // Revision 2.0
#define UEFI_FILE_PROTOCOL_LATEST_REVISION UEFI_FILE_PROTOCOL_REVISION2

#define UEFIERR(a)	(0x8000000000000000 | (uint32_t)(a))
#define UEFI_ERROR_MASK 0x8000000000000000
#define UEFIERR_OEM(a)	(0xc000000000000000 | (uint32_t)(a))

#define BAD_POINTER 0xFBFBFBFBFBFBFBFB
#define MAX_ADDRESS 0xFFFFFFFFFFFFFFFF

#define UEFI_SIGNATURE_16(A, B) ((A) | (B << 8))

#define UEFI_SIGNATURE_32(A, B, C, D) \
	(UEFI_SIGNATURE_16(A, B) | (UEFI_SIGNATURE_16(C, D) << 16))

#define UEFI_SIGNATURE_64(A, B, C, D, E, F, G, H) \
	(UEFI_SIGNATURE_32(A, B, C, D) |          \
	 ((uint64_t)(UEFI_SIGNATURE_32(E, F, G, H)) << 32))

#define va_start(v, l) __builtin_va_start(v, l)
#define va_end(v)      __builtin_va_end(v)
#define va_arg(v, l)   __builtin_va_arg(v, l)
#define va_copy(d, s)  __builtin_va_copy(d, s)

#define UEFIWARN(a)		  (a)
#define UEFI_ERROR(a)		  (((uint64_t)a) < 0)
#define UEFI_SUCCESS		  0
#define UEFI_LOAD_ERROR		  UEFIERR(1)
#define UEFI_INVALID_PARAMETER	  UEFIERR(2)
#define UEFI_UNSUPPORTED	  UEFIERR(3)
#define UEFI_BAD_BUFFER_SIZE	  UEFIERR(4)
#define UEFI_BUFFER_TOO_SMALL	  UEFIERR(5)
#define UEFI_NOT_READY		  UEFIERR(6)
#define UEFI_DEVICE_ERROR	  UEFIERR(7)
#define UEFI_WRITE_PROTECTED	  UEFIERR(8)
#define UEFI_OUT_OF_RESOURCES	  UEFIERR(9)
#define UEFI_VOLUME_CORRUPTED	  UEFIERR(10)
#define UEFI_VOLUME_FULL	  UEFIERR(11)
#define UEFI_NO_MEDIA		  UEFIERR(12)
#define UEFI_MEDIA_CHANGED	  UEFIERR(13)
#define UEFI_NOT_FOUND		  UEFIERR(14)
#define UEFI_ACCESS_DENIED	  UEFIERR(15)
#define UEFI_NO_RESPONSE	  UEFIERR(16)
#define UEFI_NO_MAPPING		  UEFIERR(17)
#define UEFI_TIMEOUT		  UEFIERR(18)
#define UEFI_NOT_STARTED	  UEFIERR(19)
#define UEFI_ALREADY_STARTED	  UEFIERR(20)
#define UEFI_ABORTED		  UEFIERR(21)
#define UEFI_ICMP_ERROR		  UEFIERR(22)
#define UEFI_TFTP_ERROR		  UEFIERR(23)
#define UEFI_PROTOCOL_ERROR	  UEFIERR(24)
#define UEFI_INCOMPATIBLE_VERSION UEFIERR(25)
#define UEFI_SECURITY_VIOLATION	  UEFIERR(26)
#define UEFI_CRC_ERROR		  UEFIERR(27)
#define UEFI_END_OF_MEDIA	  UEFIERR(28)
#define UEFI_END_OF_FILE	  UEFIERR(31)
#define UEFI_INVALID_LANGUAGE	  UEFIERR(32)
#define UEFI_COMPROMISED_DATA	  UEFIERR(33)

#define UEFI_WARN_UNKOWN_GLYPH	   UEFIWARN(1)
#define UEFI_WARN_UNKNOWN_GLYPH	   UEFIWARN(1)
#define UEFI_WARN_DELETE_FAILURE   UEFIWARN(2)
#define UEFI_WARN_WRITE_FAILURE	   UEFIWARN(3)
#define UEFI_WARN_BUFFER_TOO_SMALL UEFIWARN(4)

#define UEFI_DEVICE_PATH_PROTOCOL_GUID \
	{ 0x9576e91,                   \
	  0x6d3f,                      \
	  0x11d2,                      \
	  { 0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }
#define UEFI_DP_TYPE_MASK		 0x7F
#define UEFI_DP_TYPE_UNPACKED		 0x80
#define END_DEVICE_PATH_TYPE		 0x7f
#define END_ENTIRE_DEVICE_PATH_SUBTYPE	 0xff
#define END_INSTANCE_DEVICE_PATH_SUBTYPE 0x01
#define END_DEVICE_PATH_LENGTH		 (sizeof(struct efi_device_path))
#define DP_IS_END_TYPE(a)
#define DP_IS_END_SUBTYPE(a)        ( ((a)->subtype == END_ENTIRE_DEVICE_PATH_SUBTYPE )
#define DEVICE_PATH_TYPE(a)	   (((a)->type) & UEFI_DP_TYPE_MASK)
#define DEVICE_PATH_SUBTYPE(a)	   ((a)->subtype)
#define DEVICE_PATH_NODE_LENGTH(a) (((a)->length[0]) | ((a)->length[1] << 8))
#define NEXT_DEVICE_PATH_NODE(a)                       \
	((struct efi_device_path *)(((uint8_t *)(a)) + \
				    DEVICE_PATH_NODE_LENGTH(a)))
#define IS_DEVICE_PATH_END_TYPE(a) (DEVICE_PATH_TYPE(a) == END_DEVICE_PATH_TYPE)
#define IS_DEVICE_PATH_END_SUBTYPE(a) \
	((a)->subtype == END_ENTIRE_DEVICE_PATH_SUBTYPE)
#define IS_DEVICE_PATH_END(a) \
	(IS_DEVICE_PATH_END_TYPE(a) && IS_DEVICE_PATH_END_SUBTYPE(a))
#define IS_DEVICE_PATH_UNPACKED(a) ((a)->type & UEFI_DP_TYPE_UNPACKED)

#define SET_DEVICE_PATH_NODE_LENGTH(a, l)             \
	{                                             \
		(a)->length[0] = (uint8_t)(l);        \
		(a)->length[1] = (uint8_t)((l) >> 8); \
	}
#define SET_DEVICE_PATH_END_NODE(a)                              \
	{                                                        \
		(a)->type = END_DEVICE_PATH_TYPE;                \
		(a)->subtype = END_ENTIRE_DEVICE_PATH_SUBTYPE;   \
		(a)->length[0] = sizeof(struct efi_device_path); \
		(a)->length[1] = 0;                              \
	}

#define UEFI_SPECIFICATION_MAJOR_REVISION 1
#define UEFI_SPECIFICATION_MINOR_REVISION 02

#define TPL_APPLICATION	     4
#define TPL_CALLBACK	     8
#define TPL_NOTIFY	     16
#define TPL_HIGH_LEVEL	     31
#define UEFI_TPL_APPLICATION TPL_APPLICATION
#define UEFI_TPL_CALLBACK    TPL_CALLBACK
#define UEFI_TPL_NOTIFY	     TPL_NOTIFY
#define UEFI_TPL_HIGH_LEVEL  TPL_HIGH_LEVEL

#define NEXT_MEMORY_DESCRIPTOR(ptr, size) \
	((struct efi_memory_descriptor *)(((uint8_t *)ptr) + size))

#define UEFI_PAGE_SIZE	4096
#define UEFI_PAGE_MASK	0xFFF
#define UEFI_PAGE_SHIFT 12

#define UEFI_SIZE_TO_PAGES(a) \
	(((a) >> UEFI_PAGE_SHIFT) + ((a) & UEFI_PAGE_MASK ? 1 : 0))

#define UEFI_MEMORY_UC		       0x0000000000000001
#define UEFI_MEMORY_WC		       0x0000000000000002
#define UEFI_MEMORY_WT		       0x0000000000000004
#define UEFI_MEMORY_WB		       0x0000000000000008
#define UEFI_MEMORY_UCE		       0x0000000000000010
#define UEFI_MEMORY_WP		       0x0000000000001000
#define UEFI_MEMORY_RP		       0x0000000000002000
#define UEFI_MEMORY_XP		       0x0000000000004000
#define UEFI_MEMORY_RUNTIME	       0x8000000000000000
#define UEFI_MEMORY_DESCRIPTOR_VERSION 1

#define EVT_TIMER	    0x80000000
#define EVT_RUNTIME	    0x40000000
#define EVT_RUNTIME_CONTEXT 0x20000000

#define EVT_NOTIFY_WAIT	  0x00000100
#define EVT_NOTIFY_SIGNAL 0x00000200

#define EVT_SIGNAL_EXIT_BOOT_SERVICES	  0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 0x60000202

#define EVT_EFI_SIGNAL_MASK 0x000000FF
#define EVT_EFI_SIGNAL_MAX  4

#define UEFI_EVENT_TIMER		     EVT_TIMER
#define UEFI_EVENT_RUNTIME		     EVT_RUNTIME
#define UEFI_EVENT_RUNTIME_CONTEXT	     EVT_RUNTIME_CONTEXT
#define UEFI_EVENT_NOTIFY_WAIT		     EVT_NOTIFY_WAIT
#define UEFI_EVENT_NOTIFY_SIGNAL	     EVT_NOTIFY_SIGNAL
#define UEFI_EVENT_SIGNAL_EXIT_BOOT_SERVICES EVT_SIGNAL_EXIT_BOOT_SERVICES
#define UEFI_EVENT_SIGNAL_VIRTUAL_ADDRESS_CHANGE \
	EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE
#define UEFI_EVENT_EFI_SIGNAL_MASK EVT_EFI_SIGNAL_MASK
#define UEFI_EVENT_EFI_SIGNAL_MAX  EVT_EFI_SIGNAL_MAX

#define UEFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL  0x00000001
#define UEFI_OPEN_PROTOCOL_GET_PROTOCOL	       0x00000002
#define UEFI_OPEN_PROTOCOL_TEST_PROTOCOL       0x00000004
#define UEFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define UEFI_OPEN_PROTOCOL_BY_DRIVER	       0x00000010
#define UEFI_OPEN_PROTOCOL_EXCLUSIVE	       0x00000020

#define UEFI_OPTIONAL_PTR 0x00000001
#define UEFI_INTERNAL_FNC 0x00000002
#define UEFI_INTERNAL_PTR 0x00000004

#define UEFI_GLOBAL_VARIABLE \
	{ 0x8BE4DF61,        \
	  0x93CA,            \
	  0x11d2,            \
	  { 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C } }
#define UEFI_VARIABLE_NON_VOLATILE			    0x00000001
#define UEFI_VARIABLE_BOOTSERVICE_ACCESS		    0x00000002
#define UEFI_VARIABLE_RUNTIME_ACCESS			    0x00000004
#define UEFI_VARIABLE_HARDWARE_ERROR_RECORD		    0x00000008
#define UEFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS	    0x00000010
#define UEFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x00000020
#define UEFI_VARIABLE_APPEND_WRITE			    0x00000040
#define UEFI_MAXIMUM_VARIABLE_SIZE			    1024

#define CAPSULE_FLAGS_PERSIST_ACROSS_RESET  0x00010000
#define CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE 0x00020000
#define CAPSULE_FLAGS_INITIATE_RESET	    0x00040000

#define MPS_TABLE_GUID \
	{ 0xeb9d2d2f,  \
	  0x2d88,      \
	  0x11d3,      \
	  { 0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d } }
#define ACPI_TABLE_GUID \
	{ 0xeb9d2d30,   \
	  0x2d88,       \
	  0x11d3,       \
	  { 0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d } }
#define ACPI_20_TABLE_GUID \
	{ 0x8868e871,      \
	  0xe4f1,          \
	  0x11d3,          \
	  { 0xbc, 0x22, 0x0, 0x80, 0xc7, 0x3c, 0x88, 0x81 } }
#define SMBIOS_TABLE_GUID \
	{ 0xeb9d2d31,     \
	  0x2d88,         \
	  0x11d3,         \
	  { 0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d } }
#define SMBIOS3_TABLE_GUID \
	{ 0xf2fd1544,      \
	  0x9794,          \
	  0x4a2c,          \
	  { 0x99, 0x2e, 0xe5, 0xbb, 0xcf, 0x20, 0xe3, 0x94 } }
#define SAL_SYSTEM_TABLE_GUID \
	{ 0xeb9d2d32,         \
	  0x2d88,             \
	  0x11d3,             \
	  { 0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d } }

#define UEFI_RUNTIME_SERVICES_SIGNATURE 0x56524553544e5552
#define UEFI_RUNTIME_SERVICES_REVISION              \
	(UEFI_SPECIFICATION_MAJOR_REVISION << 16) | \
		(UEFI_SPECIFICATION_MINOR_REVISION)

#define UEFI_BOOT_SERVICES_SIGNATURE 0x56524553544f4f42
#define UEFI_BOOT_SERVICES_REVISION                 \
	(UEFI_SPECIFICATION_MAJOR_REVISION << 16) | \
		(UEFI_SPECIFICATION_MINOR_REVISION)

#define UEFI_SYSTEM_TABLE_SIGNATURE 0x5453595320494249
#define UEFI_SYSTEM_TABLE_REVISION                  \
	(UEFI_SPECIFICATION_MAJOR_REVISION << 16) | \
		(UEFI_SPECIFICATION_MINOR_REVISION)

#define UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID \
	{ 0x387477c2,                         \
	  0x69c7,                             \
	  0x11d2,                             \
	  { 0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }
#define UEFI_BLACK		0x00
#define UEFI_BLUE		0x01
#define UEFI_GREEN		0x02
#define UEFI_CYAN		(UEFI_BLUE | UEFI_GREEN)
#define UEFI_RED		0x04
#define UEFI_MAGENTA		(UEFI_BLUE | UEFI_RED)
#define UEFI_BROWN		(UEFI_GREEN | UEFI_RED)
#define UEFI_LIGHTGRAY		(UEFI_BLUE | UEFI_GREEN | UEFI_RED)
#define UEFI_BRIGHT		0x08
#define UEFI_DARKGRAY		(UEFI_BRIGHT)
#define UEFI_LIGHTBLUE		(UEFI_BLUE | UEFI_BRIGHT)
#define UEFI_LIGHTGREEN		(UEFI_GREEN | UEFI_BRIGHT)
#define UEFI_LIGHTCYAN		(UEFI_CYAN | UEFI_BRIGHT)
#define UEFI_LIGHTRED		(UEFI_RED | UEFI_BRIGHT)
#define UEFI_LIGHTMAGENTA	(UEFI_MAGENTA | UEFI_BRIGHT)
#define UEFI_YELLOW		(UEFI_BROWN | UEFI_BRIGHT)
#define UEFI_WHITE		(UEFI_BLUE | UEFI_GREEN | UEFI_RED | UEFI_BRIGHT)
#define UEFI_TEXT_ATTR(f, b)	((f) | ((b) << 4))
#define UEFI_BACKGROUND_BLACK	0x00
#define UEFI_BACKGROUND_BLUE	0x10
#define UEFI_BACKGROUND_GREEN	0x20
#define UEFI_BACKGROUND_CYAN	(UEFI_BACKGROUND_BLUE | UEFI_BACKGROUND_GREEN)
#define UEFI_BACKGROUND_RED	0x40
#define UEFI_BACKGROUND_MAGENTA (UEFI_BACKGROUND_BLUE | UEFI_BACKGROUND_RED)
#define UEFI_BACKGROUND_BROWN	(UEFI_BACKGROUND_GREEN | UEFI_BACKGROUND_RED)
#define UEFI_BACKGROUND_LIGHTGRAY \
	(UEFI_BACKGROUND_BLUE | UEFI_BACKGROUND_GREEN | UEFI_BACKGROUND_RED)
#define BOXDRAW_HORIZONTAL		   0x2500
#define BOXDRAW_VERTICAL		   0x2502
#define BOXDRAW_DOWN_RIGHT		   0x250c
#define BOXDRAW_DOWN_LEFT		   0x2510
#define BOXDRAW_UP_RIGHT		   0x2514
#define BOXDRAW_UP_LEFT			   0x2518
#define BOXDRAW_VERTICAL_RIGHT		   0x251c
#define BOXDRAW_VERTICAL_LEFT		   0x2524
#define BOXDRAW_DOWN_HORIZONTAL		   0x252c
#define BOXDRAW_UP_HORIZONTAL		   0x2534
#define BOXDRAW_VERTICAL_HORIZONTAL	   0x253c
#define BOXDRAW_DOUBLE_HORIZONTAL	   0x2550
#define BOXDRAW_DOUBLE_VERTICAL		   0x2551
#define BOXDRAW_DOWN_RIGHT_DOUBLE	   0x2552
#define BOXDRAW_DOWN_DOUBLE_RIGHT	   0x2553
#define BOXDRAW_DOUBLE_DOWN_RIGHT	   0x2554
#define BOXDRAW_DOWN_LEFT_DOUBLE	   0x2555
#define BOXDRAW_DOWN_DOUBLE_LEFT	   0x2556
#define BOXDRAW_DOUBLE_DOWN_LEFT	   0x2557
#define BOXDRAW_UP_RIGHT_DOUBLE		   0x2558
#define BOXDRAW_UP_DOUBLE_RIGHT		   0x2559
#define BOXDRAW_DOUBLE_UP_RIGHT		   0x255a
#define BOXDRAW_UP_LEFT_DOUBLE		   0x255b
#define BOXDRAW_UP_DOUBLE_LEFT		   0x255c
#define BOXDRAW_DOUBLE_UP_LEFT		   0x255d
#define BOXDRAW_VERTICAL_RIGHT_DOUBLE	   0x255e
#define BOXDRAW_VERTICAL_DOUBLE_RIGHT	   0x255f
#define BOXDRAW_DOUBLE_VERTICAL_RIGHT	   0x2560
#define BOXDRAW_VERTICAL_LEFT_DOUBLE	   0x2561
#define BOXDRAW_VERTICAL_DOUBLE_LEFT	   0x2562
#define BOXDRAW_DOUBLE_VERTICAL_LEFT	   0x2563
#define BOXDRAW_DOWN_HORIZONTAL_DOUBLE	   0x2564
#define BOXDRAW_DOWN_DOUBLE_HORIZONTAL	   0x2565
#define BOXDRAW_DOUBLE_DOWN_HORIZONTAL	   0x2566
#define BOXDRAW_UP_HORIZONTAL_DOUBLE	   0x2567
#define BOXDRAW_UP_DOUBLE_HORIZONTAL	   0x2568
#define BOXDRAW_DOUBLE_UP_HORIZONTAL	   0x2569
#define BOXDRAW_VERTICAL_HORIZONTAL_DOUBLE 0x256a
#define BOXDRAW_VERTICAL_DOUBLE_HORIZONTAL 0x256b
#define BOXDRAW_DOUBLE_VERTICAL_HORIZONTAL 0x256c
#define BLOCKELEMENT_FULL_BLOCK		   0x2588
#define BLOCKELEMENT_LIGHT_SHADE	   0x2591
#define GEOMETRICSHAPE_UP_TRIANGLE	   0x25b2
#define GEOMETRICSHAPE_RIGHT_TRIANGLE	   0x25ba
#define GEOMETRICSHAPE_DOWN_TRIANGLE	   0x25bc
#define GEOMETRICSHAPE_LEFT_TRIANGLE	   0x25c4
#define ARROW_UP			   0x2191
#define ARROW_DOWN			   0x2193

#define UEFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID \
	{ 0x387477c1,                        \
	  0x69c7,                            \
	  0x11d2,                            \
	  { 0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }
#define CHAR_NULL	     0x0000
#define CHAR_BACKSPACE	     0x0008
#define CHAR_TAB	     0x0009
#define CHAR_LINEFEED	     0x000A
#define CHAR_CARRIAGE_RETURN 0x000D
#define SCAN_NULL	     0x0000
#define SCAN_UP		     0x0001
#define SCAN_DOWN	     0x0002
#define SCAN_RIGHT	     0x0003
#define SCAN_LEFT	     0x0004
#define SCAN_HOME	     0x0005
#define SCAN_END	     0x0006
#define SCAN_INSERT	     0x0007
#define SCAN_DELETE	     0x0008
#define SCAN_PAGE_UP	     0x0009
#define SCAN_PAGE_DOWN	     0x000A
#define SCAN_F1		     0x000B
#define SCAN_F2		     0x000C
#define SCAN_F3		     0x000D
#define SCAN_F4		     0x000E
#define SCAN_F5		     0x000F
#define SCAN_F6		     0x0010
#define SCAN_F7		     0x0011
#define SCAN_F8		     0x0012
#define SCAN_F9		     0x0013
#define SCAN_F10	     0x0014
#define SCAN_F11	     0x0015
#define SCAN_F12	     0x0016
#define SCAN_ESC	     0x0017

#define PRIMARY_PART_HEADER_LBA	       1
#define UEFI_PTAB_HEADER_ID	       "EFI PART"
#define UEFI_PART_USED_BY_EFI	       0x0000000000000001
#define UEFI_PART_REQUIRED_TO_FUNCTION 0x0000000000000002
#define UEFI_PART_USED_BY_OS	       0x0000000000000004
#define UEFI_PART_REQUIRED_BY_OS       0x0000000000000008
#define UEFI_PART_BACKUP_REQUIRED      0x0000000000000010
#define UEFI_PART_USER_DATA	       0x0000000000000020
#define UEFI_PART_CRITICAL_USER_DATA   0x0000000000000040
#define UEFI_PART_REDUNDANT_PARTITION  0x0000000000000080
#define UEFI_PART_TYPE_UNUSED_GUID \
	{ 0x00000000,              \
	  0x0000,                  \
	  0x0000,                  \
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } }
#define UEFI_PART_TYPE_EFI_SYSTEM_PART_GUID \
	{ 0xc12a7328,                       \
	  0xf81f,                           \
	  0x11d2,                           \
	  { 0xba, 0x4b, 0x00, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b } }
#define UEFI_PART_TYPE_LEGACY_MBR_GUID \
	{ 0x024dee41,                  \
	  0x33e7,                      \
	  0x11d3,                      \
	  { 0x9d, 0x69, 0x00, 0x08, 0xc7, 0x81, 0xf3, 0x9f } }

#define INTERNAL_SHELL_GUID \
	{ 0xd65a6b8c,       \
	  0x71e5,           \
	  0x4df0,           \
	  { 0xa9, 0x09, 0xf0, 0xd2, 0x99, 0x2b, 0x5a, 0xa9 } }

#define UEFI_LOADED_IMAGE_PROTOCOL_GUID \
	{ 0x5B1B31A1,                   \
	  0x9562,                       \
	  0x11d2,                       \
	  { 0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } }

#define LOADED_IMAGE_PROTOCOL		    UEFI_LOADED_IMAGE_PROTOCOL_GUID
#define UEFI_LOADED_IMAGE_PROTOCOL_REVISION 0x1000
#define UEFI_IMAGE_INFORMATION_REVISION	    UEFI_LOADED_IMAGE_PROTOCOL_REVISION

#define UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
	{ 0x964e5b22,                         \
	  0x6459,                             \
	  0x11d2,                             \
	  { 0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }

#define UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL_REVISION 0x00010000
#define UEFI_FILE_IO_INTERFACE_REVISION \
	UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL_REVISION

#define UEFI_FILE_MODE_READ   0x0000000000000001
#define UEFI_FILE_MODE_WRITE  0x0000000000000002
#define UEFI_FILE_MODE_CREATE 0x8000000000000000

#define UEFI_FILE_READ_ONLY  0x0000000000000001
#define UEFI_FILE_HIDDEN     0x0000000000000002
#define UEFI_FILE_SYSTEM     0x0000000000000004
#define UEFI_FILE_RESERVED   0x0000000000000008
#define UEFI_FILE_DIRECTORY  0x0000000000000010
#define UEFI_FILE_ARCHIVE    0x0000000000000020
#define UEFI_FILE_VALID_ATTR 0x0000000000000037

#define UEFI_FILE_PROTOCOL_REVISION 0x00010000
#define UEFI_FILE_HANDLE_REVISION   UEFI_FILE_PROTOCOL_REVISION
#define UEFI_FILE_INFO_GUID \
	{ 0x9576e92,        \
	  0x6d3f,           \
	  0x11d2,           \
	  { 0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }

#define FILENAME_MAX 262
#define UEFI_SHELL_PARAMETERS_PROTOCOL_GUID \
	{ 0x752f3136,                       \
	  0x4e16,                           \
	  0x4fdc,                           \
	  { 0xa2, 0x2a, 0xe5, 0xf4, 0x68, 0x12, 0xf4, 0xca } }

#define SHELL_INTERFACE_PROTOCOL_GUID \
	{ 0x47c7b223,                 \
	  0xc42a,                     \
	  0x11d2,                     \
	  { 0x8e, 0x57, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }

#define UEFI_RNG_PROTOCOL_GUID \
	{ 0x3152bca5,          \
	  0xeade,              \
	  0x433d,              \
	  { 0x86, 0x2e, 0xc0, 0x1c, 0xdc, 0x29, 0x1f, 0x44 } }

#define UEFI_SERIAL_IO_PROTOCOL_GUID \
	{ 0xBB25CF6F,                \
	  0xF1D4,                    \
	  0x11D2,                    \
	  { 0x9A, 0x0C, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0xFD } }

#define SERIAL_IO_INTERFACE_REVISION		 0x00010000
#define UEFI_SERIAL_CLEAR_TO_SEND		 0x0010
#define UEFI_SERIAL_DATA_SET_READY		 0x0020
#define UEFI_SERIAL_RING_INDICATE		 0x0040
#define UEFI_SERIAL_CARRIER_DETECT		 0x0080
#define UEFI_SERIAL_REQUEST_TO_SEND		 0x0002
#define UEFI_SERIAL_DATA_TERMINAL_READY		 0x0001
#define UEFI_SERIAL_INPUT_BUFFER_EMPTY		 0x0100
#define UEFI_SERIAL_OUTPUT_BUFFER_EMPTY		 0x0200
#define UEFI_SERIAL_HARDWARE_LOOPBACK_ENABLE	 0x1000
#define UEFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE	 0x2000
#define UEFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE 0x4000

#define UEFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
	{ 0x9042a9de,                      \
	  0x23dc,                          \
	  0x4a38,                          \
	  { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } }

#define UEFI_EDID_ACTIVE_GUID \
	{ 0xbd8c1056,         \
	  0x9f36,             \
	  0x44ec,             \
	  { 0x92, 0xa8, 0xa6, 0x33, 0x7f, 0x81, 0x79, 0x86 } }
#define UEFI_EDID_DISCOVERED_GUID \
	{ 0x1c0c34f6,             \
	  0xd380,                 \
	  0x41fa,                 \
	  { 0xa0, 0x49, 0x8a, 0xd0, 0x6c, 0x1a, 0x66, 0xaa } }

#define UEFI_SIMPLE_POINTER_PROTOCOL_GUID \
	{ 0x31878c87,                     \
	  0xb75,                          \
	  0x11d5,                         \
	  { 0x9a, 0x4f, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d } }

#define UEFI_PCI_OPTION_ROM_TABLE_GUID \
	{ 0x7462660f,                  \
	  0x1cbd,                      \
	  0x48da,                      \
	  { 0xad, 0x11, 0x91, 0x71, 0x79, 0x13, 0x83, 0x1c } }

#define ENOMEM 12

typedef __builtin_va_list va_list;
typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT64_TYPE__ int64_t;
typedef __SIZE_TYPE__ size_t;
typedef uint8_t bool;
typedef uint16_t wchar_t;
typedef uint64_t off_t;
typedef uint64_t ssize_t;
typedef __UINTPTR_TYPE__ uintptr_t;

struct uefi_table_header {
	uint64_t signature;
	uint32_t revision;
	uint32_t header_size;
	uint32_t crc32;
	uint32_t reserved;
};

struct uefi_input_key {
	uint16_t scan_code;
	uint16_t unicode_char;
};

struct simple_input_interface {
	uint64_t (*reset)(void *this, bool extended_verification);
	uint64_t (*read_key_stroke)(void *this, struct uefi_input_key *key);
	void *wait_for_key;
};

struct simple_text_output_mode {
	uint32_t max_mode;
	uint32_t mode;
	uint32_t attribute;
	uint32_t cursor_column;
	uint32_t cursor_row;
	bool cursor_visible;
};

struct simple_text_output_interface {
	uint64_t (*reset)(void *this, bool extended_verification);
	uint64_t (*output_string)(void *this, wchar_t *string);
	uint64_t (*test_string)(void *this, wchar_t *string);
	uint64_t (*query_mode)(void *this, uint64_t mode_number,
			       uint64_t *columns, uint64_t *rows);
	uint64_t (*set_mode)(void *this, uint64_t mode_number);
	uint64_t (*set_attribute)(void *this, uint64_t attribute);
	uint64_t (*clear_screen)(void *this);
	uint64_t (*set_cursor_position)(void *this, uint64_t column,
					uint64_t row);
	uint64_t (*enable_cursor)(void *this, bool visible);
	struct simple_text_output_mode *mode;
};

struct uefi_time {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t pad1;
	uint32_t nanosecond;
	int16_t time_zone;
	uint8_t daylight;
	uint8_t pad2;
};

struct uefi_time_capabilities {
	uint32_t resolution;
	uint32_t accuracy;
	bool sets_to_zero;
};

struct uefi_memory_descriptor {
	uint32_t type;
	uint32_t pad;
	uint64_t physical_start;
	uint64_t virtual_start;
	uint64_t number_of_pages;
	uint64_t attribute;
};

struct uefi_guid {
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint8_t data4[8];
};

enum uefi_reset_type { uefi_reset_cold, uefi_reset_warm, uefi_reset_shutdown };

struct uefi_capsule_header {
	struct uefi_guid capsule_guid;
	uint32_t header_size;
	uint32_t flags;
	uint32_t capsule_image_size;
};

struct uefi_runtime_services {
	struct uefi_table_header hdr;

	uint64_t (*get_time)(struct uefi_time *time,
			     struct uefi_time_capabilities *capabilities);
	uint64_t (*set_time)(struct uefi_time *time);

	uint64_t (*get_wakeup_time)(bool *enabled, bool *pending,
				    struct uefi_time *time);
	uint64_t (*set_wakeup_time)(bool enabled, struct uefi_time *time);

	uint64_t (*set_virtual_address_map)(
		uint64_t memory_map_size, uint64_t descriptor_size,
		uint32_t descriptor_version,
		struct uefi_memory_descriptor *virtual_map);
	uint64_t (*convert_pointer)(uint64_t debug_disposition, void **address);

	uint64_t (*get_variable)(wchar_t *variable_name,
				 struct uefi_guid *vendor_guid,
				 uint32_t *attributes, uint64_t *data_size,
				 void *data);
	uint64_t (*get_next_variable_name)(uint64_t *variable_name_size,
					   wchar_t *variable_name,
					   struct uefi_guid *vendor_guid);
	uint64_t (*set_variable)(wchar_t *variable_name,
				 struct uefi_guid *vendor_guid,
				 uint32_t attributes, uint64_t data_size,
				 void *data);

	uint64_t (*get_next_high_monotonic_count)(uint32_t *high_count);
	uint64_t (*reset_system)(enum uefi_reset_type reset_type,
				 uint64_t reset_status, uint64_t data_size,
				 wchar_t *reset_data);

	uint64_t (*update_capsule)(
		struct uefi_capsule_header **capsule_header_array,
		uint64_t capsule_count, uint64_t scatter_gather_list);
	uint64_t (*query_capsule_capabilities)(
		struct uefi_capsule_header **capsule_header_array,
		uint64_t capsule_count, uint64_t *maximum_capsule_size,
		enum uefi_reset_type *reset_type);
	uint64_t (*query_variable_info)(
		uint32_t attributes, uint64_t *maximum_variable_storage_size,
		uint64_t *remaining_variable_storage_size,
		uint64_t *maximum_variable_size);
};

enum uefi_allocate_type {
	UEFI_ALLOCATE_ANY_PAGES,
	UEFI_ALLOCATE_MAX_ADDRESS,
	UEFI_ALLOCATE_ADDRESS,
	UEFI_MAX_ALLOCATE_TYPE
};

enum uefi_memory_type {
	UEFI_RESERVED_MEMORY_TYPE,
	UEFI_LOADER_CODE,
	UEFI_LOADER_DATA,
	UEFI_BOOT_SERVICES_CODE,
	UEFI_BOOT_SERVICES_DATA,
	UEFI_RUNTIME_SERVICES_CODE,
	UEFI_RUNTIME_SERVICES_DATA,
	UEFI_CONVENTIONAL_MEMORY,
	UEFI_UNUSABLE_MEMORY,
	UEFI_ACPI_RECLAIM_MEMORY,
	UEFI_ACPI_MEMORY_NVS,
	UEFI_MEMORY_MAPPED_IO,
	UEFI_MEMORY_MAPPED_IO_PORT_SPACE,
	UEFI_PAL_CODE,
	UEFI_PERSISTENT_MEMORY,
	UEFI_PERSISTENT_MEMORY_TYPE,
	UEFI_MAX_MEMORY_TYPE
};

enum uefi_timer_delay {
	UEFI_TIMER_CANCEL,
	UEFI_TIMER_PERIODIC,
	UEFI_TIMER_RELATIVE,
	UEFI_TIMER_TYPE_MAX
};

enum uefi_locate_search_type {
	UEFI_ALL_HANDLES,
	UEFI_BY_REGISTER_NOTIFY,
	UEFI_BY_PROTOCOL
};

struct uefi_device_path {
	uint8_t type;
	uint8_t subtype;
	uint8_t length[2];
};

struct uefi_open_protocol_information_entry {
	void *agent_handle;
	void *contoller_handle;
	uint32_t attributes;
	uint32_t open_count;
};

struct uefi_boot_services {
	struct uefi_table_header hdr;

	/* Task Priority Services */
	uint64_t (*raise_tpl)(uint64_t new_tpl);
	uint64_t (*restore_tpl)(uint64_t old_tpl);

	/* Memory Services */
	uint64_t (*allocate_pages)(enum uefi_allocate_type type,
				   enum uefi_memory_type memory_type,
				   uint64_t pages, uint64_t *memory);
	uint64_t (*free_pages)(uint64_t memory, uint64_t pages);
	uint64_t (*get_memory_map)(uint64_t *memory_map_size,
				   struct uefi_memory_descriptor *memory_map,
				   uint64_t *map_key, uint64_t *descriptor_size,
				   uint32_t *descriptor_version);
	uint64_t (*allocate_pool)(enum uefi_memory_type pool_type,
				  uint64_t size, void **buffer);
	uint64_t (*free_pool)(void *buffer);

	/* Event & Timer Services */
	uint64_t (*create_event)(uint32_t type, uint64_t notify_tpl,
				 void (*notify_function)(void *event,
							 void *context),
				 void *notify_context, void **event);

	uint64_t (*set_timer)(void *event, uint32_t type,
			      uint64_t trigger_time);
	uint64_t (*wait_for_event)(uint64_t number_of_events, void **event,
				   uint64_t *index);
	uint64_t (*signal_event)(void *event);
	uint64_t (*close_event)(void *event);
	uint64_t (*check_event)(void *event);

	/* Protocol Services */
	void *install_protocol_interface;
	void *reinstall_protocol_interface;
	void *uninstall_protocol_interface;

	uint64_t (*handle_protocol)(void *handle, struct uefi_guid *protocol,
				    void **interface);
	uint64_t (*pc_handle_protocol)(void *handle, struct uefi_guid *protocol,
				       void **interface);
	uint64_t (*register_protocol_notify)(struct uefi_guid *protocol,
					     void *event, void **registration);
	uint64_t (*locate_handle)(enum uefi_locate_search_type search_type,
				  struct uefi_guid *protocol, void *search_key,
				  uint64_t *buffer_size, void **buffer);

	uint64_t (*locate_device_path)(struct uefi_guid *protocol,
				       struct uefi_device_path **device_path,
				       void **device_handle);
	uint64_t (*install_configuration_table)(struct uefi_guid *guid,
						void *table);

	/* Image Services */
	uint64_t (*load_image)(bool boot_policy, void *parent_image_handle,
			       struct uefi_device_path *device_path,
			       void *source_buffer, uint64_t source_size,
			       void **image_handle);
	uint64_t (*start_image)(void *image_handle, uint64_t *exit_data_size,
				wchar_t **exit_data);
	uint64_t (*exit)(void *image_handle, uint64_t exit_status,
			 uint64_t exit_data_size, wchar_t *exit_data);

	uint64_t (*unload_image)(void *image_handle);
	uint64_t (*exit_boot_services)(void *image_handle, uint64_t map_key);

	/* Miscellaneous Services */
	uint64_t (*get_next_monotonic_count)(uint64_t *count);
	uint64_t (*stall)(uint64_t microseconds);
	uint64_t (*set_watchdog_timer)(uint64_t timeout, uint64_t watchdog_code,
				       uint64_t data_size,
				       wchar_t *watchdog_data);

	uint64_t (*connect_controller)(
		void *controller_handle, void **driver_image_handle,
		struct uefi_device_path *remaining_device_path, bool recursive);
	uint64_t (*disconnect_controller)(void *controller_handle,
					  void *driver_image_handle,
					  void *child_handle);
	uint64_t (*open_protocol)(void *handle, struct uefi_guid *protocol,
				  void **interface, void *agent_handle,
				  void *controller_handle, uint32_t attributes);
	uint64_t (*close_protocol)(void *handle, struct uefi_guid *protocol,
				   void *agent_handle, void *controller_handle);
	uint64_t (*open_protocol_information)(
		void *handle, struct uefi_guid *protocol,
		struct uefi_open_protocol_information_entry **entry_buffer,
		uint64_t *entry_count);
	uint64_t (*protocols_per_handle)(void *handle,
					 struct uefi_guid ***protocol_buffer,
					 uint64_t *protocol_buffer_count);
	uint64_t (*locate_handle_buffer)(
		enum uefi_locate_search_type search_type,
		struct uefi_guid *protocol, void *search_key,
		uint64_t *number_handle, void ***buffer);
	uint64_t (*locate_protocol)(struct uefi_guid *protocol,
				    void *registration, void **interface);
	uint64_t (*install_multiple_protocol_interfaces)(void **handle, ...);
	uint64_t (*uninstall_multiple_protocol_interfaces)(void *handle, ...);
	uint64_t (*calculate_crc32)(void *data, uint64_t data_size,
				    uint32_t *crc32);
};

struct uefi_configuration_table {
	struct uefi_guid vendor_guid;
	void *vendor_table;
};

struct uefi_system_table {
	struct uefi_table_header hdr;
	wchar_t *firmware_vendor;
	uint32_t firmware_revision;

	void *console_in_handle;
	struct simple_input_interface *con_in;

	void *console_out_handle;
	struct simple_text_output_interface *con_out;

	void *console_error_handle;
	struct simple_text_output_interface *std_err;

	struct uefi_runtime_services *runtime_services;
	struct uefi_boot_services *boot_services;

	uint64_t number_of_table_entries;
	struct uefi_configuration_table *configuration_table;
};

struct uefi_capsule_block_descriptor {
	uint64_t length;
	uint64_t continuation_pointer;
};

enum uefi_parity_type {
	UEFI_DEFAULT_PARITY,
	UEFI_NO_PARITY,
	UEFI_EVEN_PARITY,
	UEFI_ODD_PARITY,
	UEFI_MARK_PARITY,
	UEFI_SPACE_PARITY
};

enum uefi_stop_bits_type {
	UEFI_DEFAULT_STOP_BITS,
	UEFI_ONE_STOP_BIT,
	UEFI_ONE_FIVE_STOP_BITS,
	USEFI_TWO_STOP_BITS
};

struct uefi_serial_io_mode {
	uint32_t control_mask;
	uint32_t timeout;
	uint64_t baud_rate;
	uint32_t receive_fifo_depth;
	uint32_t data_bits;
	uint32_t parity;
	uint32_t stop_bits;
};

struct uefi_serial_io_protocol {
	uint32_t revision;
	uint64_t (*reset)(struct uefi_serial_io_protocol *this);
	uint64_t (*set_attributes)(struct uefi_serial_io_protocol *this,
				   uint64_t baud_rate,
				   uint32_t receive_fifo_depth,
				   uint32_t timeout,
				   enum uefi_parity_type parity,
				   uint8_t stop_bits,
				   enum uefi_stop_bits_type data_bits);
	uint64_t (*set_control_bits)(struct uefi_serial_io_protocol *this,
				     uint32_t control);
	uint64_t (*get_control_bits)(struct uefi_serial_io_protocol *this,
				     uint32_t *control);
	uint64_t (*write)(struct uefi_serial_io_protocol *this,
			  uint64_t *buffer_size, void *buffer);
	uint64_t (*read)(struct uefi_serial_io_protocol *this,
			 uint64_t *buffer_size, void *buffer);
	struct uefi_serial_io_mode *mode;
};

struct uefi_block_io_media {
	uint32_t media_id;
	bool removable_media;
	bool media_present;
	bool logical_partition;
	bool read_only;
	bool write_caching;
	uint32_t block_size;
	uint32_t io_align;
	uint64_t last_block;
};

struct uefi_block_io {
	uint32_t revision;
	struct uefi_block_io_media *media;
	uint64_t (*reset)(struct uefi_block_io *this,
			  bool extended_verification);
	uint64_t (*read_blocks)(struct uefi_block_io *this, uint32_t media_id,
				uint64_t lba, uint64_t buffer_size,
				void *buffer);
	uint64_t (*write_blocks)(struct uefi_block_io *this, uint32_t media_id,
				 uint64_t lba, uint64_t buffer_size,
				 void *buffer);
	uint64_t (*flush_blocks)(struct uefi_block_io *this);
};

struct block_file {
	off_t offset;
	struct uefi_block_io *bio;
};

enum uefi_gop_pixel_format {
	UEFI_GOP_PIXEL_RED_GREEN_BLUE_RESERVED_8BIT_PER_COLOR,
	UEFI_GOP_PIXEL_BLUE_GREEN_RED_RESERVED_8BIT_PER_COLOR,
	UEFI_GOP_PIXEL_BIT_MASK,
	UEFI_GOP_PIXEL_BLT_ONLY,
	UEFI_GOP_PIXEL_FORMAT_MAX
};

enum uefi_gop_blt_operation {
	UEFI_BLT_VIDEO_FILL,
	UEFI_BLT_VIDEO_TO_BLT_BUFFER,
	UEFI_BLT_BUFFER_TO_VIDEO,
	UEFI_BLT_VIDEO_TO_VIDEO,
	UEFI_GRAPHICS_OUTPUT_BLT_OPERATION_MAX
};

struct uefi_gop_pixel_bitmask {
	uint32_t red_mask;
	uint32_t green_mask;
	uint32_t blue_mask;
	uint32_t reserved_mask;
};

struct uefi_gop_mode_info {
	uint32_t version;
	uint32_t horizontal_resolution;
	uint32_t vertical_resolution;
	enum uefi_gop_pixel_format pixel_format;
	struct uefi_gop_pixel_bitmask pixel_bitmask;
	uint32_t pixels_per_scan_line;
};

struct uefi_gop_mode {
	uint32_t max_mode;
	uint32_t mode;
	struct uefi_gop_mode_info *info;
	uint64_t size_of_info;
	uint64_t frame_buffer_base;
	uint64_t frame_buffer_size;
};

struct uefi_gop {
	uint64_t (*query_mode)(struct uefi_gop *this, uint32_t mode_number,
			       uint64_t *size_of_info,
			       struct uefi_gop_mode_info **info);
	uint64_t (*set_mode)(struct uefi_gop *this, uint32_t mode_number);
	uint64_t (*blt)(struct uefi_gop *this, uint32_t *blt_buffer,
			enum uefi_gop_blt_operation blt_operation,
			uint64_t source_x, uint64_t source_y,
			uint64_t destination_x, uint64_t destination_y,
			uint64_t width, uint64_t height, uint64_t delta);
	struct uefi_gop_mode *mode;
};

struct uefi_edid {
	uint32_t size_of_edid;
	uint8_t *edid;
};

struct uefi_simple_pointer_state {
	int32_t relative_movement_x;
	int32_t relative_movement_y;
	int32_t relative_movement_z;
	bool left_button;
	bool right_button;
};

struct uefi_simple_pointer_mode {
	uint64_t resolution_x;
	uint64_t resolution_y;
	uint64_t resolution_z;
	bool left_button;
	bool right_button;
};

struct uefi_simple_pointer_protocol {
	uint64_t (*reset)(struct uefi_simple_pointer_protocol *this,
			  bool extended_verification);
	uint64_t (*get_state)(struct uefi_simple_pointer_protocol *this,
			      struct uefi_simple_pointer_state *state);
	void *wait_for_input;
	struct uefi_simple_pointer_mode *mode;
};

struct uefi_pci_option_rom_descriptor {
	uint64_t rom_address;
	enum uefi_memory_type memory_type;
	uint32_t rom_length;
	uint32_t seg;
	uint8_t bus;
	uint8_t dev;
	uint8_t func;
	bool executed_legacy_bios_image;
	bool dont_load_efi_rom;
};

struct uefi_pci_option_rom_table {
	uint64_t pci_option_rom_count;
	struct uefi_pci_option_rom_descriptor *pci_option_rom_descriptors;
};

struct uefi_partition_table_header {
	struct uefi_table_header hdr;
	uint64_t my_lba;
	uint64_t alternate_lba;
	uint64_t first_usable_lba;
	uint64_t last_usable_lba;
	struct uefi_guid disk_guid;
	uint64_t partition_entry_lba;
	uint32_t number_of_partition_entries;
	uint32_t size_of_partition_entry;
	uint32_t partition_entry_array_crc32;
};

struct uefi_partition_entry {
	struct uefi_guid partition_type_guid;
	struct uefi_guid unique_partition_guid;
	uint64_t starting_lba;
	uint64_t ending_lba;
	uint64_t attributes;
	wchar_t partition_name[36];
};

struct uefi_loaded_image_protocol {
	uint32_t revision;
	void *parent_handle;
	struct uefi_system_table *system_table;
	void *device_handle;
	struct uefi_device_path *file_path;
	void *reserved;
	uint32_t load_options_size;
	void *load_options;
	void *image_base;
	uint64_t image_size;
	enum uefi_memory_type image_code_type;
	enum uefi_memory_type image_data_type;
};

struct uefi_file_protocol {
	uint64_t revision; // The version of the EFI_FILE_PROTOCOL interface (e.g., EFI_FILE_PROTOCOL_LATEST_REVISION)
	uint64_t (*open)(struct uefi_file_protocol *this,
			 struct uefi_file_protocol **new_handle,
			 wchar_t *file_name, uint64_t open_mode,
			 uint64_t attributes);
	uint64_t (*close)(struct uefi_file_protocol *this);
	uint64_t (*delete)(struct uefi_file_protocol *this);
	uint64_t (*read)(struct uefi_file_protocol *this, uint64_t *buffer_size,
			 void *buffer);
	uint64_t (*write)(struct uefi_file_protocol *this,
			  uint64_t *buffer_size, void *buffer);
	uint64_t (*get_position)(struct uefi_file_protocol *this,
				 uint64_t *position);
	uint64_t (*set_position)(struct uefi_file_protocol *this,
				 uint64_t position);
	uint64_t (*get_info)(struct uefi_file_protocol *this,
			     struct uefi_guid *information_type,
			     uint64_t *buffer_size, void *buffer);
	uint64_t (*set_info)(struct uefi_file_protocol *this,
			     struct uefi_guid *information_type,
			     uint64_t buffer_size, void *buffer);
	uint64_t (*flush)(struct uefi_file_protocol *this);
	uint64_t (*open_ex)(
		struct uefi_file_protocol *this,
		struct uefi_file_protocol **new_handle, wchar_t *file_name,
		uint64_t open_mode, uint64_t attributes,
		void *token); // Extended version with async support (revision 2)
	uint64_t (*read_ex)(
		struct uefi_file_protocol *this, uint64_t *buffer_size,
		void *buffer,
		void *token); // Extended version with async support (revision 2)
	uint64_t (*write_ex)(
		struct uefi_file_protocol *this, uint64_t *buffer_size,
		void *buffer,
		void *token); // Extended version with async support (revision 2)
	uint64_t (*flush_ex)(
		struct uefi_file_protocol *this,
		void *token); // Extended version with async support (revision 2)
};

struct uefi_filesystem_protocol {
	uint32_t revision;
	uint64_t (*open_volume)(struct uefi_filesystem_protocol *this,
				struct uefi_file_protocol **root);
};

struct __tools {
	void *(*load)(const char *path);
	uint64_t (*unload)(void *image);
	void (*options)(void *image, const char *options);
} extern *image;

extern int errno;

int wctomb(char *, wchar_t);
int mbtowc(wchar_t *restrict, const char *restrict, size_t);
ssize_t write(int, const void *, size_t);
ssize_t read(int, void *, size_t);
void *malloc(size_t);
void free(void *);
void *memcpy(void *, const void *, size_t);
void *memset(void *, int, size_t);
int memcmp(const void *, const void *, size_t);
size_t strlen(const char *);

#endif
