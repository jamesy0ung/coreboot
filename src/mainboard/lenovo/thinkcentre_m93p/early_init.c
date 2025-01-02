#include <bootblock_common.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>
#include <device/pnp_ops.h>

#define SUPERIO_BASE 0x2e
#define SIO_GPIO     PNP_DEV(SUPERIO_BASE, IT8728F_GPIO)
#define SERIAL_DEV   PNP_DEV(SUPERIO_BASE, IT8728F_SP1)

void bootblock_mainboard_early_init(void)
{
	/* Initialize SuperIO */
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE); // Use configured base address

	/* Kill watchdog first to prevent reboot */
	ite_kill_watchdog(SIO_GPIO);

	/* Disable SIO reboot */
	ite_reg_write(SIO_GPIO, 0xEF, 0x7E);

	/* Configure GPIO base address */
	ite_reg_write(SIO_GPIO, 0x62, 0x0a); // Set GPIO base to 0xA00

	/* Clear watchdog timeout */
	ite_reg_write(SIO_GPIO, 0x72, 0x20);
	ite_reg_write(SIO_GPIO, 0x73, 0x00);
}
