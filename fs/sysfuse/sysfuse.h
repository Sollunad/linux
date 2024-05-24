#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include "device.h"

int sysfuse_fstat(unsigned int fd, struct kstat *stat);

int sysfuse_open(const char *filename, unsigned int fd);

void sysfuse_close(unsigned int fd);

ssize_t sysfuse_read(unsigned int fd, char __user *buf, size_t count);

bool sysfuse_is_responsible(unsigned int fd);
