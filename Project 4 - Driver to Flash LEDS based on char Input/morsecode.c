/**************************************************************************
 * Assignment #4: Linux Drivers
 * Author: Sunny Pritpal Singh (301244872)
 * CMPT 433 
 * - A driver to flash leds based on input values consisting of characters
 * 	a-z or A-Z. Characters are translated into morse code values and displayed
 *      as flashes to the LED.
 *      This driver also outputs stats to /proc/morse-code
 *      example usage: echo hello world > /dev/morse-code
 *      Adapted from: Brian Fraser
 ***************************************************************************/
#include <linux/module.h>
#include <linux/miscdevice.h>		// for misc-driver calls.
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/ctype.h>
#include <linux/kfifo.h>
#include <linux/proc_fs.h>	
//#error Are we building this?

#define MY_DEVICE_FILE  "morse-code" //name of /dev/"file"
#define DEVICE_NAME  "morse-code" //name of /proc/"file"

/******************************************************
 * LED
 ******************************************************/
#include <linux/leds.h>

DEFINE_LED_TRIGGER(ledtrig_demo);

#define DOT_TIME_MULTIPLIER 3
#define LED_ON_TIME_ms 100
#define LED_ON_TIME_ms 100
#define LED_OFF_TIME_ms 900
#define ASCII_TO_MORSE_ARRAY_OFFSET 97


#define PRINT_BUFFER_SIZE 1024
static char print_buffer[PRINT_BUFFER_SIZE];

/**************************************************************
 * FIFO Support
 *************************************************************/
// Info on the interface:
//    https://www.kernel.org/doc/htmldocs/kernel-api/kfifo.html#idp10765104
// Good example:
//    http://lxr.free-electrons.com/source/samples/kfifo/bytestream-example.c

#define FIFO_SIZE 256	// Must be a power of 2.
static DECLARE_KFIFO(my_fifo, char, FIFO_SIZE);



int DOT_TIME = 200; //defines interval between flashes
int cmdline = 0;    //user value for changing time interval

int characters_flashed = 0; //stats that will be displayed in /proc directory
int dots_flashed = 0;
int dashes_flashed = 0;

module_param(cmdline, int, 0); //retrieves cmd line input

			//MORSE CODE values for letters a->z
const char morse_values[26][5] = {".-00", "-...0", "-.-.0", "-..0", ".000", 
			    "..-.0", "--.0", "....0", "..00", ".---0",
			    "-.-0", ".-.0", "--00", "-.00", "---0",
			    ".--.0", "--.-0", ".-.0", "...0", "-000",
			    "..-0", "...-0", ".--0", "-..-0", "-.--0",
			    "--..0"};


/* Read information from this /proc entry:
 * 	returns: Number of bytes written to page.
 */
static int my_read_proc_info(struct file *file, char *buf, size_t count, loff_t *offp)
{
	int read_offset = (int) *offp;
	int bytes_read = 0;
	int length = 0;

	// Generate the data:
	// Note that two string literals back-to-back are merged by the compiler.
	length = snprintf(print_buffer, PRINT_BUFFER_SIZE,
				"Sunny's Morse Code Driver:\n"
				"====================\n"
				"Dot Time:         %d\n"
				"# Characters:     %d\n"
				"# Dots:           %d\n"
				"# Dashes:         %d\n"
				"\n",

				DOT_TIME, characters_flashed, dots_flashed, dashes_flashed
			);

	// Copy generate text into the user's buffer
	// (size limited by how much data as we have, and size of the user's buffer)
	bytes_read = length - read_offset;
	if (bytes_read > count) {
		bytes_read = count;
	}
	if (copy_to_user(buf, print_buffer + read_offset, bytes_read)) {
		return -EFAULT;
	}

	// Return what we read
	*offp += bytes_read;
	return bytes_read;
}

//blink interval for dot			    
static void my_led_blink_dot(void)
{
	led_trigger_event(ledtrig_demo, LED_FULL);
	msleep(DOT_TIME);
	led_trigger_event(ledtrig_demo, LED_OFF);
	msleep(DOT_TIME);
}

//blink interval for dash
static void my_led_blink_dash(void)
{
	led_trigger_event(ledtrig_demo, LED_FULL);
	msleep(DOT_TIME_MULTIPLIER * DOT_TIME);
	led_trigger_event(ledtrig_demo, LED_OFF);
	msleep(DOT_TIME);
}

//basic blink to LED   
static void my_led_blink(void)
{
	led_trigger_event(ledtrig_demo, LED_FULL);
	msleep(LED_ON_TIME_ms);
	led_trigger_event(ledtrig_demo, LED_OFF);
	msleep(LED_OFF_TIME_ms);
}

static void led_register(void)
{
	// Setup the trigger's name -> appears in "cat trigger":
	led_trigger_register_simple("morse-code", &ledtrig_demo);
}

static void led_unregister(void)
{
	// Cleanup
	led_trigger_unregister_simple(ledtrig_demo);
}


/******************************************************
 * Callbacks
 ******************************************************/
static ssize_t my_write(struct file* file, const char *buff,
		size_t count, loff_t* ppos)
{
	int i;
	int k;
	int index = 0;
	//int space_flag = 1;
	
	printk(KERN_INFO "morsecode: Flashing for string of length %d.\n", count);

	// Blink once per character (-1 to skip end null)
	for (i = 0; i < count-1; i++) {
		int space_flag = 1;
		char ch;
		my_led_blink();
		
		if (copy_from_user(&ch, &buff[i], sizeof(ch))) {
			return -EFAULT;
		}

		ch = tolower(ch); //covert any input from user to lower case

			//translate the input into morse code array index value 
		if(ch >='a' && ch <='z'){
			index = ch;
			index = index - ASCII_TO_MORSE_ARRAY_OFFSET;
		
			
		/* OUTPUT ONE CHARACTER OF A MESSAGE ON THE LED*/	
			for (k = 0; k <= 4; k++){//looks up the morse code value
						//and blinks the led accordingly

				if(morse_values[index][k] == '0')
				{
					characters_flashed++;
					msleep(2 * DOT_TIME);
					space_flag = 0;
					kfifo_put(&my_fifo, ' ');
					break;
					//my_led_blink_dot();	
				}
										
				if(morse_values[index][k] == '.')
				{
					dots_flashed++;
					my_led_blink_dot();
					kfifo_put(&my_fifo, '.');
					space_flag = 0;	
				}
				
				if(morse_values[index][k] == '-')
				{
					dashes_flashed++;
					my_led_blink_dash();
					kfifo_put(&my_fifo, '-');
					space_flag = 0;
				}
			
			}//end for loop

			printk(KERN_INFO "%c", ch);
		}
		
		if((ch == ' ') && (space_flag == 0)){
			//6 dot times because one dot time already ouput from and of char 
			
			//kfifo_put(&my_fifo, ' ');
			kfifo_put(&my_fifo, ' ');
			kfifo_put(&my_fifo, ' ');
		
			msleep(6 * DOT_TIME);
			space_flag = 0;
		}
		
		else if((ch == ' ') && (space_flag == 1)){

			//7 dot times because this is either first space or a sequence of spaces

			//kfifo_put(&my_fifo, ' ');
			kfifo_put(&my_fifo, ' ');
			kfifo_put(&my_fifo, ' ');
			
			msleep(7 * DOT_TIME);
			space_flag = 1;
		}
		
		if (i == (count -2)){
		
			kfifo_put(&my_fifo, '\n');
		}
	}

	// Return # bytes actually written.
	return count;
}

/******************************************************
 * Read finction to safely set user sapce values from FIFO
 ******************************************************/
static ssize_t my_read(struct file *file,
		char *buf, size_t count, loff_t *ppos)
{
	// Pull all available data from fifo into user buffer
	int num_bytes_read = 0;
	if (kfifo_to_user(&my_fifo, buf, count, &num_bytes_read)) {
		return -EFAULT;
	}

	// Can use kfifo_get() to pull out a single value to kernel:
	/*
	int val;
	if (!kfifo_get(&echo_fifo, &val)) {
		// fifo empty.... handle it some how
	}
	// now data is in val... do with it what you will.
	*/

	return num_bytes_read;  // # bytes actually read.
}

/******************************************************
 * Misc Driver functions for file operations
 ******************************************************/
// Callbacks:  (structure defined in <kernel>/include/linux/fs.h)
struct file_operations my_fops = {
	.owner    =  THIS_MODULE,
	.read	  =  my_read,
	.write    =  my_write,
};

// Character Device info for the Kernel:
static struct miscdevice my_miscdevice = {
		.minor    = MISC_DYNAMIC_MINOR,         // Let the system assign one.
		.name     = MY_DEVICE_FILE,             // /dev/.... file.
		.fops     = &my_fops                    // Callback functions.
};

// Setup callbacks to all the functions for proc.
// (an also use write, close, ....)
struct file_operations proc_fops = {
		.owner= THIS_MODULE,
		.read = my_read_proc_info
};

/******************************************************
 * Driver initialization and exit:
 ******************************************************/
static int __init my_init(void)
{
	int ret;
	printk(KERN_INFO "----> morse code led driver init(): file /dev/%s.\n", MY_DEVICE_FILE);
	if((cmdline > 2000 )|| (cmdline < 1)){
		//printk(KERN_INFO "morse code driver: invalid dot time.\n");
		DOT_TIME = 200;
	
	} else {
		DOT_TIME = cmdline;
	}
	
	// Initialize the FIFO.
	INIT_KFIFO(my_fifo);
		
	printk(KERN_INFO "DOT_TIME is : %d\n", DOT_TIME);
	// Register as a misc driver:
	ret = misc_register(&my_miscdevice);

	
	// Register proc:
	proc_create(
			DEVICE_NAME,           // name of /proc/... entry.
			0,                     // Mode (r/w)
			NULL,                  // Parent (none)
			&proc_fops             // file_operations struct
			);

	// LED:
	led_register();

	return ret;
}





static void __exit my_exit(void)
{
	printk(KERN_INFO "<---- morse code led driver exit().\n");

	// Unregister misc driver
	misc_deregister(&my_miscdevice);
	
	// remove proc entry	
	remove_proc_entry(DEVICE_NAME, NULL);
	// remove LED:
	led_unregister();
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sunny Singh");
MODULE_DESCRIPTION("A morse code led driver");
MODULE_LICENSE("GPL");

