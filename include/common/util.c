
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char *
allocate_strmem (int len)
{

        if (len <= 0) {
                fprintf (stderr,
                        "ERR: Cannot allocate memory because len = %i in allocate_strmem().\n",
                        len);
               return NULL;
        }

        void *tmp;
        tmp = (char *) malloc (len * sizeof (char));
        if(tmp == NULL) {
                fprintf (stderr,
                        "ERR: Cannot allocate memory for array allocate_strmem().\n");
                return NULL;
        }

        memset (tmp, 0, len * sizeof (char));
        return (tmp);

}

uint8_t *
allocate_ustrmem (int len)
{


        if (len <= 0) {
                fprintf (stderr,
                        "ERR: Cannot allocate memory because len = %i in allocate_ustrmem().\n",
                        len);
                return NULL;
        }

        void *tmp;
        tmp = (uint8_t *) malloc (len * sizeof (uint8_t));

        if (tmp == NULL) {
                fprintf (stderr,
                        "ERR: Cannot allocate memory for array allocate_ustrmem().\n");
                return NULL;
        }
        memset (tmp, 0, len * sizeof (uint8_t));

        return tmp;
}


int *
allocate_intmem (int len)
{

        if (len <= 0) {
                fprintf (stderr,
                         "ERR: Cannot allocate memory because len = %i in allocate_intmem().\n",
                        len);
                return NULL;
        }
        void *tmp;
        tmp = (int *) malloc (len * sizeof (int));

        if (tmp == NULL) {
                fprintf (stderr,
                         "ERR: Cannot allocate memory for array allocate_intmem().\n");
                return NULL;
        }

        memset (tmp, 0, len * sizeof (int));
        return tmp;
}

uint16_t
checksum (uint16_t *addr, int len)
{
        int count = len;
        register uint32_t sum = 0;
        uint16_t answer = 0;

        // Sum up 2-byte values until none or only one byte left.
        while (count > 1) {
                sum += *(addr++);
                count -= 2;
        }

        // Add left-over byte, if any.
        if (count > 0) {
              sum += *(uint8_t *) addr;
        }

        // Fold 32-bit sum into 16 bits; we lose information by doing this,
        // increasing the chances of a collision.
        // sum = (lower 16 bits) + (upper 16 bits shifted right 16 bits)
        while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
        }

        // Checksum is one's compliment of sum.
        answer = ~sum;

        return (answer);
}

void
DumpHex(const void* data, size_t size)
{
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}
