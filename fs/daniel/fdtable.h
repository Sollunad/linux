#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include "device.h"

int fdt_fstat(unsigned int fd, struct kstat *stat);

int fdt_open(const char *filename);

ssize_t fdt_read(unsigned int fd, char __user *buf, size_t count);

bool fdt_is_responsible(unsigned int fd);
