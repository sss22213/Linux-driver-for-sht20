#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

int main()
{
    float temperature = 0;
    
    float humidity = 0;

    unsigned char buffer[4] = {0x00};

    /* Read info from sht20 */
    int fd = open("/dev/sht20", O_RDWR);
    if (fd < 0) {
    	printf("Error: cannot open file.\n");
	    return -1;
    }
    printf("Open success\n");
    read(fd, &buffer, 4);
    close(fd);

    printf("Origin: 0x%x 0x%x 0x%x 0x%x\n", buffer[0], buffer[1], buffer[2], buffer[3]);

    temperature = (buffer[0] << 8 | buffer[1]) / 100.0;

    humidity = (buffer[2] << 8 | buffer[3]) / 100.0;

    printf("Temperature: %.2f, Humidity: %.2f\n", temperature, humidity);

    return 0;
}