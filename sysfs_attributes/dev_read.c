/*
 * Author : Kiran Nayak
 * Edited by : Ahmed Azazy
 */
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
char buffer[2048];
char dev_directory[1000];
char device_name[100];
#define TRY_READ 10

int main(int argc, char *argv[])
{
	int fd;
	
	/*this variable holds remaining data bytes to be read */
	int remaining = TRY_READ;
	
	/*Holds count of total data bytes read so far */
	int total_read=0;
	
	int n =0,ret=0;
	printf("Number of args = %d\n" , argc);
	if(argc != 3 ){
		printf("Wrong usage\n");
		printf("Correct usage: <file> <readcount>\n");
		return 0;
	}

	printf("argv[0]=%s ... argv[1]= %s....agrv[2]=%d\n" , argv[0] , argv[0] , atoi(argv[2]));
	if(strlen((char *)argv[1]) > 100)
	{
		printf("Sorry device file name can't be more than 100 bytes\n");
		return 0;
	}

	strcpy(device_name , argv[1]);
	/*convert command line supplied data to integer */
	remaining = atoi(argv[2]);


	printf("read requested = %d\n",remaining);
	printf("device name is %s\n" , device_name);
	sprintf(dev_directory , "/dev/%s" , device_name );
	printf("directory is: %s\n" , dev_directory);
	fd = open(dev_directory,O_RDONLY);

	if(fd < 0){
		/*perror decodes user space errno variable and prints cause of failure*/
		perror("open");
		return fd;
	}

	printf("open was successful\n");

#if  0 
	/*activate this for lseek testing */
	ret = lseek(fd,-10,SEEK_SET);
	if(ret < 0){
		perror("lseek");
		close(fd);
		return ret;
	}
#endif
	/*Lets attempt reading twice */
	
	while(n != 2 && remaining)
	{
		/*read data from 'fd' */
		ret = read(fd,&buffer[total_read],remaining);

		if(!ret){
			/*There is nothing to read */
			printf("end of file \n");
			break;
		}else if(ret <= remaining){
			printf("read %d bytes of data \n",ret );
			/*'ret' contains count of data bytes successfully read , so add it to 'total_read' */
		        total_read += ret;
			/*We read some data, so decrement 'remaining'*/
			remaining -= ret;
		}else if(ret < 0){
			printf("something went wrong\n");
			break;
		}else
			break;

		n++;
	}

	printf("total_read = %d\n",total_read);

	//dump buffer
	for(int i=0 ; i < total_read ; i++)
		printf("%c",buffer[i]);

	printf("\n");
	close(fd);
	
	return 0;
}
