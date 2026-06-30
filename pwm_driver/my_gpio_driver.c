#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define DEVICE_NAME "my_gpio_dev"
#define GPIO_PIN 22 + 571

static int major;
static struct class *cls;
static struct device *dev;
static int last_duty = 1500; // 마지막으로 설정된 펄스 폭 저장

// 유저가 read()를 호출할 때: 현재 설정된 duty 값을 알려줌
static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    if (*off > 0) return 0; // EOF 처리
    if (copy_to_user(buf, &last_duty, sizeof(int))) return -EFAULT;
    *off += sizeof(int);
    return sizeof(int);
}

// 유저가 write()를 호출할 때: 모터 각도를 변경함
static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    int duty_us;
    int i;

    if (len < sizeof(int)) return -EINVAL;
    if (copy_from_user(&duty_us, buf, sizeof(int))) return -EFAULT;

    // 안전 범위 제한 (500us ~ 2500us)
    if (duty_us < 500) duty_us = 500;
    if (duty_us > 2500) duty_us = 2500;
    
    last_duty = duty_us;

    // 소프트웨어 PWM 생성 (20ms 주기)
    for (i = 0; i < 20; i++) {
        gpio_set_value(GPIO_PIN, 1);
        udelay(duty_us);
        gpio_set_value(GPIO_PIN, 0);
        udelay(20000 - duty_us);
    }
    return sizeof(int);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,   // read() 연결
    .write = my_write, // write() 연결
};

static int __init my_init(void) {
    gpio_free(GPIO_PIN);
    if (gpio_request(GPIO_PIN, "MY_GPIO")) return -EBUSY;

    major = register_chrdev(0, DEVICE_NAME, &fops);
    cls = class_create("my_gpio_class");
    dev = device_create(cls, NULL, MKDEV(major, 0), NULL, "my_gpio"); // /dev/my_gpio 자동 생성

    gpio_direction_output(GPIO_PIN, 0);
    printk(KERN_INFO "Read/Write 드라이버 로드 완료\n");
    return 0;
}

static void __exit my_exit(void) {
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    gpio_set_value(GPIO_PIN, 0);
    gpio_free(GPIO_PIN);
    unregister_chrdev(major, DEVICE_NAME);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
