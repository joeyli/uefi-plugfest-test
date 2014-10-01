/*
 * A simple program to test enable/disable synaptics LED by 97 command.
 */
#include <stdio.h>
#include <sys/io.h>
#include <linux/types.h>

int main(void)
{
        __u8 rdata; 

        ioperm (0x60, 1, 1);

	/* set caps on */
	rdata = rdata | 4;

        /* use ED command to enable LED */
        outb(0xED, 0x60);

        outb(rdata, 0x60);

        /* wait 1 seconds, just for test  */
        sleep(1);

        /* use ED command to disable LED */
	rdata = 0;
        
        outb(0xED, 0x60); 

        outb(rdata, 0x60);

        return 0;
}
