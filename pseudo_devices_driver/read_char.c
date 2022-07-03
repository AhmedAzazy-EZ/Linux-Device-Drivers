#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

char buffer[20];

int main(void)
{


	int fd;
	printf("Yooo Openning\n");
	fd = open("/dev/pcd_1", O_RDONLY);

	if (fd < 0)
	{
		printf("Can't Open the file-____-\n");

	}
	printf("Read done\n");
	read(fd , buffer , 4 );
	printf("%s" , buffer);

	return 0;
}
