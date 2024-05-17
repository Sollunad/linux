#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/buffer_head.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

int request_device(char *call, const char *filename, char *param, char *res_buf);