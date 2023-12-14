#include "device.h"

int REQ_ID = 1;

static ssize_t _write_to_dev(const char *data) {
	struct file *file;
	loff_t pos;
	ssize_t ret;

	// Open the device
	file = filp_open("/dev/fdtable", O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (IS_ERR(file)) {
		return PTR_ERR(file);
	}

	// Move to the end of the device
	pos = file->f_pos;

	// Write data to the file
	ret = kernel_write(file, data, strlen(data), &pos);

	// Close the file
	filp_close(file, NULL);

	return ret;
}

static int _get_dev_response(char* req_id, char *buf) {
	struct file *file;
	loff_t pos;
	ssize_t ret;
	char line_start[10];
	char expected_key[10];
	snprintf(expected_key, 10, "R%s", req_id);

	// Open the device
	file = filp_open("/dev/fdtable", O_RDONLY, 0);

	// Move to the end of the device
	pos = file->f_pos;

	while (true) {
		// Read data from the file
		ret = kernel_read(file, line_start, 10, &pos);

		// If end of file, wait for new device content
		if (ret <= 0) {
			continue;
		}

		printk("read new line start for %s, result: %s\n", req_id, line_start);

		// Move pointer past the request id
		pos = pos + 10;

		// If start of line does not match, go to the start of new line
		if (strcmp(line_start, expected_key) != 0) {
			char ch;
			while (ch != '\n') {
				ret = kernel_read(file, &ch, 1, &pos);
				pos++;
			}
			continue;
		}

		// Line matches request id, so write response to buffer
		// Write to the buffer until the next \n
		int buf_count = 0;
		char ch;
		while (ch != '\n') {
			ret = kernel_read(file, &ch, 1, &pos);
			pos++;
			buf[buf_count++] = ch;
		}

		break;
	}



	// Close the file
	filp_close(file, NULL);

	return 0;
}

void request_device(char *call, const char *filename, char *res_buf) {
	char req_buf[1024];
	char padded_id[10];
	snprintf(padded_id, 10, "%09d", REQ_ID);
	REQ_ID++;
	snprintf(req_buf, sizeof(req_buf), "%s %s %s\n", padded_id, call, filename);
	printk("write to device: %s", req_buf);
	_write_to_dev(req_buf);
	_get_dev_response(padded_id, res_buf);
}