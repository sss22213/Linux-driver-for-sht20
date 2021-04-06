#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>

#define SHT20_DEV_NAME "sht20"
#define SHT20_TEMPERATURE_NO_HOLD_ADDRESS 0xF3
#define SHT20_HUMIDITY_NO_HOLD_ADDRESS 0xF5

static struct class *sht20_class = NULL;
static dev_t dev = 0;

/* sht20 character device struct*/
struct sht20_i2c_cdev {
    struct i2c_client *client;
    struct cdev cdev;
};

/* Read temperature from sht20 */
static int read_temperature(struct i2c_client *client, u8 *buf)
{
    /* i2c tranfer status */
    int status = 0;
    
    /* Store temperature */
    int temperature = 0;

    u8 tmp[3] = {0};

    /* If send fault, status less than zero. */
    status = i2c_smbus_write_byte(client, SHT20_TEMPERATURE_NO_HOLD_ADDRESS);
    if(status < 0) {
        return -1;
    }
    
    /**
     *  Follow spec.
     *  14 bits must be delay 85 ms.
     */
    msleep(85);

    /* If receive fault, status less than zero. */
    status = i2c_master_recv(client, tmp, 3);
    if(status < 0) {
        return -1;
    }

    temperature = (tmp[0] << 8) | (tmp[1] &0xFC);
    temperature = -4685 + ((17572 * temperature) >> 16);
    buf[0] = temperature >> 8;
    buf[1] = temperature & 0xFF;

    /* Success obtain temperature */
    return 0;
}

static int read_humidity(struct i2c_client *client, u8 *buf)
{
    int humidity = 0;

    /* i2c tranfer status */
    int status = 0; 

    u8 tmp[3] = {0};  

    /* If send fault, status less than zero. */
    status = i2c_smbus_write_byte(client, SHT20_HUMIDITY_NO_HOLD_ADDRESS);
    if(status < 0) {
        return -1;
    }

    /**
     *  Follow spec.
     *  14 bits must be delay 29 ms.
     */
    msleep(29);

    /* If receive fault, status less than zero. */
    status = i2c_master_recv(client, tmp, 3);
    if(status < 0) {
        return -1;
    }

    humidity = (tmp[0] << 8) | (tmp[1] &0xFC);
    humidity = -600 + ((12500 * humidity) >> 16);
    buf[0] = humidity >> 8;
    buf[1] = humidity & 0xFF;

    /* Success obtain humidity */
    return 0;
}

static ssize_t sht20_i2c_open(struct inode *inode, struct file *filp)
{
    struct sht20_i2c_cdev *sht20 = container_of(inode->i_cdev, struct sht20_i2c_cdev, cdev);

    pr_info("Open device.\n");

    filp->private_data = sht20->client;

    return 0;
}

static ssize_t sht20_i2c_read(struct file *filp, char __user *buf, size_t count, loff_t *offset)
{
    u8 tmp[2] = {0};

    int status = -1;

    struct i2c_client *client = filp->private_data;
    if (!client) {
    	pr_err("Failed to get struct i2c_client.\n");
	    return -EINVAL;
    }

    /* If obtain temperature success, status equal zero.*/
    status = read_temperature(client, tmp);
    if(status == 0) {
        buf[0] = tmp[0];
        buf[1] = tmp[1];
    }


    /* If obtain temperature success, status equal zero.*/
    status = read_humidity(client, tmp);
    if(status == 0) {
        buf[2] = tmp[0];
        buf[3] = tmp[1];
    }

    return 0;
}

struct file_operations sht20_i2c_fops = {
    .open = sht20_i2c_open,
    .read = sht20_i2c_read,
};


static int sht20_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct sht20_i2c_cdev *sht20 = NULL;

    struct device *device = NULL;

    pr_info("Probe device.\n");

    /* Alloc space for cdev */
    alloc_chrdev_region(&dev, 0, 1, SHT20_DEV_NAME);

    /* Create class for sht20 */
    sht20_class = class_create(THIS_MODULE, SHT20_DEV_NAME);

    sht20 = kzalloc(sizeof(struct sht20_i2c_cdev), GFP_KERNEL);

    sht20->client = client;

    /* Initialize character */
    cdev_init(&(sht20->cdev), &sht20_i2c_fops);

    sht20->cdev.owner = THIS_MODULE;

    /* Add character to system */
    cdev_add(&(sht20->cdev), dev, 1);

    /* Create device */
    device = device_create(sht20_class, NULL, dev, NULL, SHT20_DEV_NAME);

    i2c_set_clientdata(client, sht20);

    return 0;
  
}

static int sht20_remove(struct i2c_client *client)
{
    pr_info("sht20 device is removing.\n");

    return 0;
}

static struct of_device_id sht20_id_tables [] = {
    { .compatible="sht20", },
    { }
};

MODULE_DEVICE_TABLE(of, sht20_id_tables);

static struct i2c_driver sht20_drv = {
    .probe = sht20_probe,
    .remove = sht20_remove,
    .driver = {
    	.name = "sht20 device 0.1",
	.owner = THIS_MODULE,
	.of_match_table = sht20_id_tables,
    },
};

module_i2c_driver(sht20_drv);
MODULE_LICENSE("GPL");