/*dispatch and deliver the HIO messages */
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include "hio_transport.h"
#include "hio_def.h"
#include <string.h>

static int gfd;
//static int gfd_read;

int init_hio_transport(char *device) 
{
	if (device && 0 != *device) 
	{
		gfd = open(device, O_RDWR | O_NOCTTY);
		if (gfd < 0) 
		{
			printf("opening device failed");
			return -1;
		}
		
		struct termios SerialPortSettings; /* Create the structure                          */

		tcgetattr(gfd, &SerialPortSettings); /* Get the current attributes of the Serial port */

		/* Setting the Baud rate */
		cfsetispeed(&SerialPortSettings, B921600); /* Set Read  Speed as 921600                       */
		cfsetospeed(&SerialPortSettings, B921600); /* Set Write Speed as 921600                       */
		/* 8N1 Mode */
		SerialPortSettings.c_cflag &= ~PARENB; /* Disables the Parity Enable bit(PARENB),So No Parity   */
		SerialPortSettings.c_cflag &= ~CSTOPB; /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
		SerialPortSettings.c_cflag &= ~CSIZE; /* Clears the mask for setting the data size             */
		SerialPortSettings.c_cflag |= CS8; /* Set the data bits = 8                                 */

		SerialPortSettings.c_cflag &= ~CRTSCTS; /* No Hardware flow Control                         */
		SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */

		SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable XON/XOFF flow control both i/p and o/p */
		SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Non Cannonical mode                            */

		SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/
		
		/* Setting Time outs */
		SerialPortSettings.c_cc[VMIN] = 0; 
		SerialPortSettings.c_cc[VTIME] = 2; /* Wair for up to 0.2 seconds  */
		
		if ((tcsetattr(gfd, TCSANOW, &SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
			printf("\n  ERROR ! in Setting attributes");
		
		tcflush(gfd, TCIFLUSH);
		
		return 0;
		
	}
	
	return -1;
}


int send_command(int group_id, int msg_id, void *data, int size, int transid)
{
	int len;
	char actual_pay_load[512] = {0,};
	struct hio_header_info header = { 0, };

	header.groupid = group_id;
	header.msgid = msg_id;

	header.trandid = transid;

	len = sizeof(header) + size;
	header.len = len - 2;

	if (data)
		memcpy(actual_pay_load + sizeof(header), data, size);
	memcpy(actual_pay_load, &header, sizeof(header));

	return write_hio_message(actual_pay_load, len);
}

int write_hio_message(char *message, int msg_len) {
	int bytes_written = 0;
	while (bytes_written < msg_len) {
		bytes_written += write(gfd, message + bytes_written, 1);

	}
	return bytes_written;
}

int read_hio_message(char *message, int len) {

	return read(gfd, message, len); /* Read the data*/
}

void deinit_hio_transport() {
	close(gfd);
	gfd = -1;
}
