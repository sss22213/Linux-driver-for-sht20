#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

int main()
{
    unsigned char buffer[4] = {0x00};
    int fd = open("/dev/sht20", O_RDWR);
    if (fd < 0) {
    	printf("Error: cannot open file.\n");
	    return -1;
    }
    printf("Open success\n");
    read(fd, &buffer, 4);
    printf("0x%x 0x%x 0x%x 0x%x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
    close(fd);
    return 0;
}