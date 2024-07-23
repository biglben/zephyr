#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/retention/retention.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/kernel.h>

static const struct device *retention_dev = DEVICE_DT_GET(DT_NODELABEL(retention0));

static void print_raw(void)
{
	uint8_t *ptr = (uint8_t *)(DT_REG_ADDR(DT_NODELABEL(ret_mem)) +
				   DT_REG_ADDR(DT_NODELABEL(retention0)));
	printk("data at mem:\n");
	for (size_t i = 0; i < DT_REG_SIZE(DT_NODELABEL(retention0)); i++) {
		printk("%02x ", ptr[i]);
	}
	printk("\n");
}

static void fill_raw(void)
{
	uint8_t *ptr = (uint8_t *)(DT_REG_ADDR(DT_NODELABEL(ret_mem)) +
				   DT_REG_ADDR(DT_NODELABEL(retention0)));
	for (size_t i = 0; i < DT_REG_SIZE(DT_NODELABEL(retention0)); i++) {
		ptr[i] = 0xaa;
	}
}

int main(void)
{
	printk("\n\n--------------------------------------------------------------------\n");
	printk("Hello World!\n");
	uint32_t new_value = 0x01234567;

	// Try to read
	if (retention_is_valid(retention_dev) == 1) {
		printk("valid\n");
		uint32_t tmp = 0;
		if (retention_read(retention_dev, 0, (uint8_t *)&tmp, sizeof(tmp)) == 0) {
			printk("data ok, tmp is %x\n", tmp);
			new_value = tmp + 1;
			k_sleep(K_SECONDS(1));
		} else {
			printk("data valid but read fail\n");
			print_raw();
			k_sleep(K_SECONDS(10));
		}
	} else {
		printk("data invalid\n");
		print_raw();
		k_sleep(K_SECONDS(10));
	}

	retention_clear(retention_dev);

	// fill_raw();

	// write value
	printk("Try to write value %x\n\n", new_value);
	if (retention_write(retention_dev, 0, (uint8_t *)&new_value, sizeof(new_value)) != 0) {
		printk("write failed\n");
	}
	// call it twice seems to work:
	// retention_write(retention_dev, 0, (uint8_t *)&new_value, sizeof(new_value));
	printk("written\n");
	print_raw();
	// Bootbanner seems to overwrite last newline
	printk("\n");
	sys_reboot(SYS_REBOOT_COLD);
	return 0;
}
