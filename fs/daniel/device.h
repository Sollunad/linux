#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/buffer_head.h>

void request_device(char *call, const char *filename, char *res_buf);