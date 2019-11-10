/*  modem-fhss.c - The simplest kernel module.
 */

#include <linux/fs.h>
#include <linux/err.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/kdev_t.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/moduleparam.h>

#include <asm/io.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/irq.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/of_irq.h>
#include <linux/uaccess.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/device.h>
#include <linux/of_device.h>
#include <linux/dma-mapping.h>

#include <linux/sched.h>
#include <linux/kernel.h> 		//printk()
#include <linux/slab.h>			//kmalloc()
#include <linux/errno.h>		//error codes
#include <linux/types.h>  		//size_t
#include <linux/interrupt.h> 	//mark_bh
#include <linux/kthread.h>
#include <linux/jiffies.h>
#include <linux/seq_file.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>

#include "modem-fhss.h"

#define DRIVER_NAME "modem-fhss"
#define PROCFS_NAME "modem-fhss-procfs"

//
#define REG_MLIP_LOOPBACK			0x0000		// заворотка на MLIP

//Регистры DMA
#define REG_DMA_CR					0x0004		// регистр управления DMA (0 бит - запуск DMA MM2P)
#define REG_DMA_SR					0x0008		// регистр статуса DMA (1 бит - прерывание intr_rx)
#define REG_DMA_IR					0x000C		// регистр включения прерываний DMA (1 бит - прерывание intr_rx)
#define REG_DMA_SA					0x0010		// адрес источника
#define REG_DMA_DA					0x0014		// адрес назначения
#define REG_DMA_SL					0x0018		// длина массива данных DMA для источника
#define REG_DMA_DL					0x001C		// длина массива данных DMA для получателя (read only)

//Регистры MLIP
#define REG_MLIP_SR					0x0020		// регистр статуса MLIP (0-ой бит - intr_tx)
#define	REG_MLIP_IR					0x0024		// регистр включения прерываний MLIP (0-ой бит - intr_tx)
#define	REG_MLIP_RST				0x0028		// регистр включения прерываний MLIP (0-ой бит - intr_tx)

//Регистры MLIP
#define REG_MLIP_CE					0x002C		// регистр ce MLIP (0-ой бит - Mlip_ce_o)
#define	REG_MLIP_CNTCRC_ON			0x0030		// Включить подсчет битых пакетов (0-ой бит - Mlip_cntcrc_on)
#define	REG_MLIP_CNTCRC				0x0034		// Счетчик битых пакетов MLIP (16 младших бит) (read only)

//Регистры модема
#define ADDR_REG_M_RST				0x0038		//
#define ADDR_REG_M_LOOP				0x003C		//
#define ADDR_REG_M_LOOK_DET			0x0040		//
#define ADDR_REG_M_EN_MOD			0x0044		//
#define ADDR_REG_M_EN_DEMOD			0x0048		//
#define ADDR_REG_M_EN_HOPPER		0x004C		//
#define ADDR_REG_M_SEL_FREQ			0x0050		//
#define ADDR_REG_M_SEED				0x0054		//
#define ADDR_REG_M_AV_SIZE			0x0058		//
#define ADDR_REG_M_DEC_PRD			0x005C		//
#define ADDR_REG_M_TH_FREQ			0x0060		//
#define ADDR_REG_M_SAW_GEN			0x0064		//
#define ADDR_REG_M_LOOKUP_SIZE		0x0074		//
#define ADDR_REG_M_AD_EN			0x0078		//
#define ADDR_REG_M_AD_TXNRX			0x007C		//
#define ADDR_REG_M_AD_INC			0x0080		//
#define ADDR_REG_M_AD_DEC			0x0084		//
#define ADDR_REG_M_AD_BUST_CONF		0x0088		//
#define ADDR_REG_M_AD_LIGHT			0x008C		//
#define ADDR_REG_M_CIC_EN			0x0090		//
#define ADDR_REG_M_DC_DEL_TX		0x0094		//
#define ADDR_REG_M_DC_DEL_RX		0x0098		//
#define ADDR_REG_M_AFC_EN			0x009C		//
#define ADDR_REG_M_AFC_KP			0x00A0		//
#define ADDR_REG_M_AFC_KI			0x00A4		//
#define ADDR_REG_M_GARD_MU_P		0x00A8		//
#define ADDR_REG_M_GARD_START		0x00AC		//
#define ADDR_REG_M_GARD_LEN_FRZ		0x00B0		//
#define ADDR_REG_M_HOPPER_THCNT		0x00B8
#define ADDR_REG_M_HOPPER_SLIP		0x00BA
#define ADDR_REG_M_MLIP_CNTCRC_ON	0x0030		// разрешение счетчика битых пакетов
#define ADDR_REG_M_MLIP_CNTCRC 		0x0034		// счетчик битых пакетов MLIP
#define ADDR_REG_M_MLIP_CNT_TX		0x00C8 		// счетчик прерываний TX 
#define ADDR_REG_M_MLIP_CNT_RX		0x00CA 		// счетчик прерываний RX
#define ADDR_REG_M_GARD_SLIP_FW		0x00D0		// ручное проскальзывание вперед
#define ADDR_REG_M_GARD_SLIP_BW		0x00D4		// ручное проскальзывание назад


//#define IOC_MAGIC       1
//#define IOC_MAXNR       3
//#define IOC_SET         _IOW(IOC_MAGIC, 0, int)
//#define IOC_GET         _IOR(IOC_MAGIC, 1, int)

#define DMA_LENGTH 1024

static int major_number = 0;
static int minor_number = 0;

//static int param_ioctl = 0;

static struct class *device_class;

struct chrdrv_private {
	void __iomem *io_base;

	int irq_rx;
	int irq_tx;

	char *src_addr;
	char *dst_addr;
	dma_addr_t src_handle;
	dma_addr_t dst_handle;

	wait_queue_head_t wq_rx;
	wait_queue_head_t wq_tx;

	spinlock_t lock;

	int flag_wait_rx;
	int flag_wait_tx;

	int device_open;

	struct device *device;
	struct kobject *mykobj;
	struct resource resource;
	struct proc_dir_entry *our_proc_file;

	struct cdev cdev;
};

static __inline void chrdrv_write_reg(struct chrdrv_private *priv, const unsigned int reg_addr, const unsigned int reg_value) {
	iowrite32(reg_value, (void __iomem *)(priv->io_base + reg_addr));
}

static __inline unsigned int chrdrv_read_reg(struct chrdrv_private *priv, const unsigned int reg_addr) {
	return ioread32((void __iomem *)(priv->io_base + reg_addr));
}

static int get_config(struct modem_fhss_attr *a)
{
	if (!strcmp(a->attr.name, "modem_rst")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_RST);
		printk(KERN_INFO "read modem_rst: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_loop")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_LOOP);
		printk(KERN_INFO "read modem_loop: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_look_det")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_LOOK_DET);
		printk(KERN_INFO "read modem_look_det: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_en_mod")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_EN_MOD);
		printk(KERN_INFO "read modem_en_mod: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_en_demod")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_EN_DEMOD);
		printk(KERN_INFO "read modem_en_demod: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_en_hopper")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_EN_HOPPER);
		printk(KERN_INFO "read modem_en_hopper: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_sel_freq")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_SEL_FREQ);
		printk(KERN_INFO "read modem_sel_freq: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_seed")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_SEED);
		printk(KERN_INFO "read modem_seed: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_av_size")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_AV_SIZE);
		printk(KERN_INFO "read modem_av_size: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_dec_prd")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_DEC_PRD);
		printk(KERN_INFO "read modem_dec_prd: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_th_freq")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_TH_FREQ);
		printk(KERN_INFO "read modem_th_freq: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_saw_gen")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_SAW_GEN);
		printk(KERN_INFO "read modem_saw_gen: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_lookup_size")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_LOOKUP_SIZE);
		printk(KERN_INFO "read modem_lookup_size: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_ad_en")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_AD_EN);
		printk(KERN_INFO "read modem_ad_en: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_ad_txnrx")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_AD_TXNRX);
		printk(KERN_INFO "read modem_ad_txnrx: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_ad_inc")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_AD_INC);
		printk(KERN_INFO "read modem_ad_inc: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_ad_dec")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_AD_DEC);
		printk(KERN_INFO "read modem_ad_dec: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_ad_bust_conf")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_AD_BUST_CONF);
		printk(KERN_INFO "read modem_ad_bust_conf: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_ad_light")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_AD_LIGHT);
		printk(KERN_INFO "read modem_ad_light: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_cic_en")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_CIC_EN);
		printk(KERN_INFO "read modem_cic_en: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_dc_del_tx")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_DC_DEL_TX);
		printk(KERN_INFO "read modem_dc_del_tx: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_dc_del_rx")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_DC_DEL_RX);
		printk(KERN_INFO "read modem_dc_del_rx: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_afc_en")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_AFC_EN);
		printk(KERN_INFO "read modem_afc_en: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_afc_kp")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_AFC_KP);
		printk(KERN_INFO "read modem_afc_kp: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_afc_ki")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_AFC_KI);
		printk(KERN_INFO "read modem_afc_ki: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_gard_mu_p")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_GARD_MU_P);
		printk(KERN_INFO "read modem_gard_mu_p: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_gard_start")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_GARD_START);
		printk(KERN_INFO "read modem_gard_start: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_gard_len_frz")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_GARD_LEN_FRZ);
		printk(KERN_INFO "read modem_gard_len_frz: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_hopper_thcnt")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_HOPPER_THCNT);
		printk(KERN_INFO "read modem_hopper_thcnt: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_hopper_slip")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_HOPPER_SLIP);
		printk(KERN_INFO "read modem_hopper_slip: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_mlip_loop")) {
		a->value = chrdrv_read_reg(a->priv, REG_MLIP_LOOPBACK);
		printk(KERN_INFO "read modem_mlip_loop: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_mlip_cntcrc_on")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_MLIP_CNTCRC_ON);
		printk(KERN_INFO "read modem_mlip_cntcrc_on: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_mlip_cntcrc")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_MLIP_CNTCRC);
		printk(KERN_INFO "read modem_mlip_cntcrc: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_mlip_cnt_tx")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_MLIP_CNT_TX);
		printk(KERN_INFO "read modem_mlip_cnt_tx: %d at 0x%x\n", a->value, ADDR_REG_M_MLIP_CNT_TX);
	} else if (!strcmp(a->attr.name, "modem_mlip_cnt_rx")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_MLIP_CNT_RX);
		printk(KERN_INFO "read modem_mlip_cnt_rx: %d at 0x%x\n", a->value, ADDR_REG_M_MLIP_CNT_RX);
	} else if (!strcmp(a->attr.name, "modem_gard_slip_fw")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_GARD_SLIP_FW);
		printk(KERN_INFO "read modem_gard_slip_fw: %d at 0x%x\n", a->value, ADDR_REG_M_GARD_SLIP_FW);
	} else if (!strcmp(a->attr.name, "modem_gard_slip_bw")) {
		a->value = chrdrv_read_reg(a->priv, ADDR_REG_M_GARD_SLIP_BW);
		printk(KERN_INFO "read modem_gard_slip_bw: %d at 0x%x\n", a->value, ADDR_REG_M_GARD_SLIP_BW);
	} 

	return 0;
}

static ssize_t sysfs_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	unsigned long flags;
	struct modem_fhss_attr *a = container_of(attr, struct modem_fhss_attr, attr);

	spin_lock_irqsave(&a->priv->lock, flags);

	get_config(a);

	spin_unlock_irqrestore(&a->priv->lock, flags);

	return scnprintf(buf, PAGE_SIZE, "%d\n", a->value);
}

static int set_config(const struct modem_fhss_attr *a)
{
	if (!strcmp(a->attr.name, "modem_rst")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_RST, a->value);
		printk(KERN_INFO "write modem_rst: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_loop")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_LOOP, a->value);
		printk(KERN_INFO "write modem_loop: %d\n", a->value);
	//} else if (!strcmp(a->attr.name, "modem_look_det")) {
	//	chrdrv_write_reg(a->priv, ADDR_REG_M_LOOK_DET, a->value);
	//	printk(KERN_INFO "write modem_look_det: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_en_mod")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_EN_MOD, a->value);
		printk(KERN_INFO "write modem_en_mod: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_en_demod")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_EN_DEMOD, a->value);
		printk(KERN_INFO "write modem_en_demod: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_en_hopper")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_EN_HOPPER, a->value);
		printk(KERN_INFO "write modem_en_hopper: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_sel_freq")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_SEL_FREQ, a->value);
		printk(KERN_INFO "write modem_sel_freq: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_seed")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_SEED, a->value);
		printk(KERN_INFO "write modem_seed: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_av_size")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_AV_SIZE, a->value);
		printk(KERN_INFO "write modem_av_size: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_dec_prd")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_DEC_PRD, a->value);
		printk(KERN_INFO "write modem_dec_prd: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_th_freq")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_TH_FREQ, a->value);
		printk(KERN_INFO "write modem_th_freq: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_saw_gen")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_SAW_GEN, a->value);
		printk(KERN_INFO "write modem_saw_gen: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_lookup_size")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_LOOKUP_SIZE, a->value);
		printk(KERN_INFO "write modem_lookup_size: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_ad_en")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_AD_EN, a->value);
		printk(KERN_INFO "write modem_ad_en: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_ad_txnrx")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_AD_TXNRX, a->value);
		printk(KERN_INFO "write modem_ad_txnrx: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_ad_inc")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_AD_INC, a->value);
		printk(KERN_INFO "write modem_ad_inc: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_ad_dec")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_AD_DEC, a->value);
		printk(KERN_INFO "write modem_ad_dec: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_ad_bust_conf")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_AD_BUST_CONF, a->value);
		printk(KERN_INFO "write modem_ad_bust_conf: %d\n", a->value);
	//} else if (!strcmp(a->attr.name, "modem_ad_light")) {
	//	chrdrv_write_reg(a->priv, ADDR_REG_M_AD_LIGHT, a->value);
	//	printk(KERN_INFO "write modem_ad_light: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_cic_en")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_CIC_EN, a->value);
		printk(KERN_INFO "write modem_cic_en: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_dc_del_tx")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_DC_DEL_TX, a->value);
		printk(KERN_INFO "write modem_dc_del_tx: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_dc_del_rx")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_DC_DEL_RX, a->value);
		printk(KERN_INFO "write modem_dc_del_rx: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_afc_en")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_AFC_EN, a->value);
		printk(KERN_INFO "write modem_afc_en: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_afc_kp")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_AFC_KP, a->value);
		printk(KERN_INFO "write modem_afc_kp: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_afc_ki")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_AFC_KI, a->value);
		printk(KERN_INFO "write modem_afc_ki: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_gard_mu_p")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_GARD_MU_P, a->value);
		printk(KERN_INFO "write modem_gard_mu_p: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_gard_start")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_GARD_START, a->value);
		printk(KERN_INFO "write modem_gard_start: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_gard_len_frz")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_GARD_LEN_FRZ, a->value);
		printk(KERN_INFO "write modem_gard_len_frz: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_hopper_thcnt")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_HOPPER_THCNT, a->value);
		printk(KERN_INFO "write modem_hopper_thcnt: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_hopper_slip")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_HOPPER_SLIP, a->value);
		printk(KERN_INFO "write modem_hopper_slip: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_mlip_loop")) {
		chrdrv_write_reg(a->priv, REG_MLIP_LOOPBACK, a->value);
		printk(KERN_INFO "write modem_mlip_loop: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_mlip_cntcrc_on")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_MLIP_CNTCRC_ON, a->value);
		printk(KERN_INFO "write modem_mlip_cntcrc_on: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_mlip_cntcrc")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_MLIP_CNTCRC, a->value);
		printk(KERN_INFO "write modem_mlip_cntcrc: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_mlip_cnt_tx")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_MLIP_CNT_TX, a->value);
		printk(KERN_INFO "write modem_mlip_cnt_tx: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_mlip_cnt_rx")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_MLIP_CNT_RX, a->value);
		printk(KERN_INFO "write modem_mlip_cnt_rx: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_gard_slip_fw")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_GARD_SLIP_FW, a->value);
		printk(KERN_INFO "write modem_gard_slip_fw: %d\n", a->value);
	} else if (!strcmp(a->attr.name, "modem_gard_slip_bw")) {
		chrdrv_write_reg(a->priv, ADDR_REG_M_GARD_SLIP_BW, a->value);
		printk(KERN_INFO "write modem_gard_slip_bw: %d\n", a->value);
	}

	return 0;
}

static ssize_t sysfs_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t len)
{
	unsigned long flags;
	struct modem_fhss_attr *a = container_of(attr, struct modem_fhss_attr, attr);

	spin_lock_irqsave(&a->priv->lock, flags);

	sscanf(buf, "%d", &a->value);

	set_config(a);

	spin_unlock_irqrestore(&a->priv->lock, flags);

	return len;
}

static irqreturn_t chrdrv_irq_rx_handler(int irq, void *dev_id, struct pt_regs *regs) {
	struct chrdrv_private *priv = (struct chrdrv_private *)dev_id;

	chrdrv_write_reg(priv, REG_DMA_SR, 2);

	priv->flag_wait_rx = 1;

	wake_up_interruptible(&priv->wq_rx);

	return IRQ_HANDLED;
}

static irqreturn_t chrdrv_irq_tx_handler(int irq, void *dev_id, struct pt_regs *regs) {
	struct chrdrv_private *priv = (struct chrdrv_private *)dev_id;

	chrdrv_write_reg(priv, REG_MLIP_SR, 1);

	priv->flag_wait_tx = 1;

	wake_up_interruptible(&priv->wq_tx);

	return IRQ_HANDLED;
}

static int chrdrv_open(struct inode *pinode, struct file *filp) {
	struct chrdrv_private *priv;

	priv = container_of(pinode->i_cdev, struct chrdrv_private, cdev);
	filp->private_data = priv;

	if (priv->device_open) {
		return -EBUSY;
	}
	priv->device_open++;

	if (request_irq(priv->irq_rx, chrdrv_irq_rx_handler, IRQF_SHARED, DRIVER_NAME, priv)) {
		return -ENOMEM;
	}
	if (request_irq(priv->irq_tx, chrdrv_irq_tx_handler, IRQF_SHARED, DRIVER_NAME, priv)) {
		return -ENOMEM;
	}

	return 0;
}

static ssize_t chrdrv_read(struct file *filp, const char __user * buf, size_t len, loff_t * off) {
	struct chrdrv_private *priv = (struct chrdrv_private *)filp->private_data;

	int ret;
	int	datalen;

	ret = 0;

	ret = wait_event_interruptible_timeout(priv->wq_rx, priv->flag_wait_rx != 0, msecs_to_jiffies(1000));
	if (ret == 0) {
		return 0;
	}

	datalen = chrdrv_read_reg(priv, REG_DMA_DL);

	copy_to_user(buf, priv->dst_addr, datalen);

	priv->flag_wait_rx = 0;

	return datalen;
}

static ssize_t chrdrv_write(struct file *filp, const char __user * buf, size_t len, loff_t * off) {
	struct chrdrv_private *priv = (struct chrdrv_private *)filp->private_data;

	int ret;

	if (len > (DMA_LENGTH)) len = DMA_LENGTH;

	copy_from_user(priv->src_addr, buf, len);

	chrdrv_write_reg(priv, REG_DMA_SL, len);
	chrdrv_write_reg(priv, REG_DMA_CR, 1);

	ret = 0;

	ret = wait_event_interruptible_timeout(priv->wq_tx, priv->flag_wait_tx != 0, msecs_to_jiffies(1000));
	if (ret == 0) {
		return 0;
	}

	priv->flag_wait_tx = 0;

	return len;
}

static int chrdrv_close(struct inode *pinode, struct file *filp) {
	struct chrdrv_private *priv = (struct chrdrv_private *)filp->private_data;

	free_irq(priv->irq_rx, priv);
	free_irq(priv->irq_tx, priv);

	priv->device_open--;

	return 0;
}

/*static long chrdrv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
	struct chrdrv_private *priv = (struct chrdrv_private *)filp->private_data;

	if (_IOC_TYPE(cmd) != IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > IOC_MAXNR) return -ENOTTY;

	switch(cmd) {

		case IOC_SET:
			copy_from_user(&param_ioctl, (void*)arg, sizeof(param_ioctl));
			break;

		case IOC_GET:
			copy_to_user((void*)arg, &param_ioctl, sizeof(param_ioctl));
			break;

		default :
			return -ENOTTY;
	}

	return 0;
}*/

static int chrdrv_proc_show(struct seq_file *m, void *v) {
	char buffer[80];
	sprintf(buffer, "%s - %d\n", "major number", major_number);
	seq_printf(m, buffer);
	return 0;
}

static int chrdrv_proc_open(struct inode *inode, struct  file *file) {
	return single_open(file, chrdrv_proc_show, NULL);
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = chrdrv_open,
	.write = chrdrv_write,
	.read = chrdrv_read,
	.release = chrdrv_close,
	//.unlocked_ioctl = chrdrv_ioctl,
};

static struct sysfs_ops modem_fhss_ops = {
	.show = sysfs_show,
	.store = sysfs_store,
};

static struct kobj_type modem_fhss_type = {
	.sysfs_ops = &modem_fhss_ops,
	.default_attrs = modem_fhss_attr,
};

static const struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.open = chrdrv_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static char *mydevnode(struct device *dev, umode_t *mode) {
	if(mode) {
		*mode = 0644;
	}
	return 0;
}

static int probe(struct platform_device *pdev) {
	int res = 0;
	dev_t devno = 0;

	struct chrdrv_private *priv = (struct chrdrv_private*)kmalloc(sizeof(struct chrdrv_private), GFP_KERNEL);
	if (!priv) {
		return -1;
	}
	memset(priv, 0, sizeof(struct chrdrv_private));

	priv->our_proc_file = proc_create(PROCFS_NAME, 0664, NULL, &proc_fops);
	if (priv->our_proc_file == NULL) {
		remove_proc_entry(PROCFS_NAME, NULL);
		return -ENOMEM;
	}

	res = alloc_chrdev_region(&devno, minor_number, 1, DRIVER_NAME);
	if (res) {
		goto out_alloc;
	}
	major_number = MAJOR(devno);

	device_class = class_create(THIS_MODULE, DRIVER_NAME);
	if (IS_ERR(device_class)) {
		goto out_chrdev;
	}
	device_class->devnode=mydevnode;

	cdev_init(&priv->cdev, &fops);
	priv->cdev.owner = THIS_MODULE;
	priv->cdev.ops = &fops;
	res = cdev_add(&priv->cdev, devno, 1);
	if (res < 0) {
		goto out_chrdev;
	}

	priv->mykobj = kzalloc(sizeof(*priv->mykobj), GFP_KERNEL);
	if (priv->mykobj) {
		kobject_init(priv->mykobj, &modem_fhss_type);
		if (kobject_add(priv->mykobj, NULL, "%s", "modem-fhss")) {
			printk(KERN_ALERT "Sysfs creation failed\n");
			kobject_put(priv->mykobj);
			priv->mykobj = NULL;
			goto out_chrdev;
		}
	}

	chrdrv_set_attr_priv(priv);

	res = of_address_to_resource(pdev->dev.of_node, 0, &priv->resource);
	if (res) {
		goto out_chrdev;
	}

	res = request_mem_region(priv->resource.start, resource_size(&priv->resource), DRIVER_NAME);
	if (!res) {
		goto out_chrdev;
	}

	priv->io_base = of_iomap(pdev->dev.of_node, 0);
	if (!priv->io_base) {
		goto out_iomap;
	}

	priv->irq_rx = irq_of_parse_and_map(pdev->dev.of_node, 0);
	priv->irq_tx = irq_of_parse_and_map(pdev->dev.of_node, 1);

	priv->device = device_create(device_class, NULL, MKDEV(major_number, minor_number), NULL, DRIVER_NAME);
	if (IS_ERR(priv->device)) {
		goto out_class;
	}

	priv->device_open = 0;
	priv->flag_wait_rx = 0;
	priv->flag_wait_tx = 0;

	init_waitqueue_head(&priv->wq_rx);
	init_waitqueue_head(&priv->wq_tx);

	spin_lock_init(&priv->lock);

	priv->src_addr = dma_zalloc_coherent(NULL, DMA_LENGTH, &priv->src_handle, GFP_KERNEL);
	if (!priv->src_addr) {
		return -ENOMEM;
	}

	priv->dst_addr = dma_zalloc_coherent(NULL, DMA_LENGTH, &priv->dst_handle, GFP_KERNEL);
	if (!priv->dst_addr) {
		return -ENOMEM;
	}

	chrdrv_write_reg(priv, REG_DMA_SA, priv->src_handle);
	chrdrv_write_reg(priv, REG_DMA_DA, priv->dst_handle);

	chrdrv_write_reg(priv, REG_MLIP_RST, 1);
	chrdrv_write_reg(priv, REG_MLIP_RST, 0);

	chrdrv_write_reg(priv, REG_MLIP_IR, 1);
	chrdrv_write_reg(priv, REG_DMA_IR, 2);

	chrdrv_write_reg(priv, REG_MLIP_CE, 1);

	dev_set_drvdata(&pdev->dev, priv);

	printk(KERN_INFO "%s: interface registered\n", DRIVER_NAME);
	return 0;

out_class:
	cdev_del(&priv->cdev);
	class_destroy(device_class);

out_iomap:
	release_mem_region(priv->resource.start, resource_size(&priv->resource));

out_chrdev:
	unregister_chrdev_region(devno, 1);

out_alloc:
	kfree(priv);

	return res;
}

static int remove(struct platform_device *pdev) {
	dev_t devno = 0;
	struct chrdrv_private *priv = dev_get_drvdata(&pdev->dev);

	devno = MKDEV(major_number, minor_number);

	if (priv->mykobj) {
		kobject_put(priv->mykobj);
		kfree(priv->mykobj);
	}

	dma_free_coherent(NULL, DMA_LENGTH, priv->src_addr, priv->src_handle);
	dma_free_coherent(NULL, DMA_LENGTH, priv->dst_addr, priv->dst_handle);

	unregister_chrdev_region(devno, 1);
	iounmap(priv->io_base);
	release_mem_region(priv->resource.start, resource_size(&priv->resource));
	cdev_del(&priv->cdev);
	device_destroy(device_class, devno);
	class_destroy(device_class);
	remove_proc_entry(PROCFS_NAME, NULL);

	kfree(priv);
	dev_set_drvdata(&pdev->dev, NULL);

	printk(KERN_INFO "%s: interface unregistered\n", DRIVER_NAME);

	return 0;
}

static struct of_device_id chrdrv_of_match[] = {
	{ .compatible = "xlnx,axi-modem-fhss-1.0", },
	{ .compatible = "axi-modem-fhss-1.0", },
	{ },
};
MODULE_DEVICE_TABLE(of, chrdrv_of_match);

static struct platform_driver chrdrv_driver = {
	.probe = probe,
	.remove = remove,
	.driver = {
		.name = DRIVER_NAME,
		.of_match_table = of_match_ptr(chrdrv_of_match),
	},
};

static int __init chrdrv_init(void) {
	return platform_driver_register(&chrdrv_driver);
}

static void __exit chrdrv_exit(void) {
	return platform_driver_unregister(&chrdrv_driver);
}

module_init(chrdrv_init);
module_exit(chrdrv_exit);

MODULE_DESCRIPTION("Modem-fhss module");
MODULE_AUTHOR("Aleksei Pashinov");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
