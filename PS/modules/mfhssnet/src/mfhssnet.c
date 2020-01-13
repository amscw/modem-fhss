#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/in.h>
#include <linux/netdevice.h>	// struct device and other headers
#include <linux/etherdevice.h>	// eth_type_trans
#include <linux/ip.h>			// struct iphdr
#include <linux/tcp.h>			// struct tcphdr
#include <linux/skbuff.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/dma-mapping.h>

#include "mfhssnet.h"
#include "mfhssfs.h"
#include "pool.h"
#include "mfhssioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("amscw");			// https://github.com/amscw


//-------------------------------------------------------------------------------------------------
// MACRO
//-------------------------------------------------------------------------------------------------
#define MFHSSNET_DMA_SIZE	2048
#define PRINT_DSTR_STG(from) PDEBUG("stage%i:%s...\n", from, destroy_stage_strings[from])
#define PRINT_CLOSE_STG(from) PDEBUG("stage%i:%s...\n", from, close_stage_strings[from])
// WARNING: требуется контекст с priv
#define REG_WR(dir, reg, value) iowrite32(value, (void __iomem*)(priv->io_base + REG_##dir##_##reg##_ADDRESS))
#define REG_RD(dir, reg) ioread32((void __iomem*)(priv->io_base + REG_##dir##_##reg##_ADDRESS))

//-------------------------------------------------------------------------------------------------
// Types
//-------------------------------------------------------------------------------------------------
enum destroy_stage_ {
	STAGE_DESTROY_ALL,
	STAGE_UNREGISTER_NETDEV,
	STAGE_UNMAP_MEM_REGION,
	STAGE_DESTROY_MEM_REGION,
	STAGE_CLEAN_DYNAMIC_REGS_DIR,
	STAGE_DESTROY_DYNAMIC_REGS_DIR,
	STAGE_CLEAN_STATIC_REGS_DIR,
	STAGE_DESTROY_STATIC_REGS_DIR,
	STAGE_DESTROY_POOL,
	STAGE_DESTROY_STATS,
	STAGE_DESTROY_NETDEV,
};

enum close_stage_
{
	STAGE_FREE_ALL,
	STAGE_FREE_DMA_SRC,
	STAGE_FREE_DMA_DST,
	STAGE_UNREG_IRQ_RX,
	STAGE_UNREG_IRQ_TX,
};

//-------------------------------------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------------------------------------
static int mfhss_open(struct net_device *dev); 
static int mfhss_close(struct net_device *dev);
static int mfhss_config(struct net_device *dev, struct ifmap *map);
static int mfhss_tx_pkt(struct sk_buff *skb, struct net_device *dev);
static int mfhss_ioctl(struct net_device *dev, struct ifreq *req, int cmd);
static int mfhss_change_mtu(struct net_device *dev, int new_mtu);
static void mfhss_tx_timeout(struct net_device *dev);
static struct net_device_stats *mfhss_stats(struct net_device *dev);
static int mfhss_header(struct sk_buff *skb, struct net_device *dev, unsigned short type, const void *daddr, const void *saddr, unsigned len);
static int mfhssnet_probe(struct platform_device *pl_dev);
static int mfhssnet_remove(struct platform_device *pl_dev);


//-------------------------------------------------------------------------------------------------
// Varibles
//-------------------------------------------------------------------------------------------------
// struct net_device *mfhss_dev;	// FIXME no need to hold this var, use platform_data instead!
static const struct net_device_ops mfhss_net_device_ops = {
 	.ndo_open            = mfhss_open,
 	.ndo_stop            = mfhss_close,
 	.ndo_start_xmit      = mfhss_tx_pkt,
 	.ndo_do_ioctl        = mfhss_ioctl,
 	.ndo_set_config      = mfhss_config,
 	.ndo_get_stats       = mfhss_stats,
 	.ndo_change_mtu      = mfhss_change_mtu,
 	.ndo_tx_timeout      = mfhss_tx_timeout
};
static const struct header_ops mfhss_header_ops = {
    .create  = mfhss_header,
    .cache = NULL,
};
static int timeout = DUMMY_NETDEV_TIMEOUT;
static unsigned long trans_start;
/* static */const struct of_device_id mfhssnet_of_match[] = {
	{ .compatible = "xlnx,axi-modem-fhss-1.0", },
	{}
};
MODULE_DEVICE_TABLE(of, mfhssnet_of_match);
static struct platform_driver mfhssnet_driver = {
		.driver = {
			.name	= DRIVER_NAME,
			.owner	= THIS_MODULE,
			.of_match_table = of_match_ptr(mfhssnet_of_match),
		},
		.probe 		= mfhssnet_probe,
		.remove		= mfhssnet_remove,
};
static const char * const destroy_stage_strings[] = {
	"destroying all",
	"unregister netdev",
	"unmapping memory region",
	"free io memory region",
	"clean dynamic regs directory",
	"destroying dynamic regs directory",
	"clean static regs directory",
	"destroying static regs directory",
	"destroying pool",
	"destroying interface statistics",
	"free netdev",
};

static const char* const close_stage_strings[] = {
	"free all",
	"free DMA source memory",
	"free DMA destination memory",
	"unregister RX interrupt handler",
	"unregister TX interrupt handler",
};

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/*
 * Net device support
 */
static char *ipaddr_to_str(u32 ipaddr)
{
	static char iptable[10][16];
	static int index = 0;
	union
	{
		u32 word;
		u8 bytes[sizeof (u32)];
	} tmp = { ntohl(ipaddr) };
	char *str = &iptable[index][0];

	sprintf(str, "%d.%d.%d.%d", tmp.bytes[3], tmp.bytes[2], tmp.bytes[1], tmp.bytes[0]);

	if (++index == sizeof iptable / sizeof *iptable)
		index = 0;

	return str;
}

static void rx_int_en(struct net_device *dev, int enable)
{
	struct mfhss_priv_ *priv = netdev_priv(dev);
	priv->rx_int_en = enable;
}

// static void rxPkt(struct net_device *pDev, struct dumPacket_ *pPkt)
// {
// 	struct sk_buff *pSkB;
// 	struct dumPriv_ *pPriv = netdev_priv(pDev);

// 	// The packet has been retrived from transmission medium.
// 	// Build an skb around it, so upper layers can handle it
// 	pSkB = dev_alloc_skb(pPkt->datalen + 2);
// 	if (pSkB == NULL)
// 	{
// 		if (printk_ratelimit())
// 			PRINT_STATUS_MSG("cannot allocate socket buffer, packet dropped", -ENOMEM);
// 		pPriv->stats.rx_dropped++;
// 		return;			
// 	}
// 	skb_reserve(pSkB, 2); // align IP on 16-bit boundary
// 	memcpy(skb_put(pSkB, pPkt->datalen), pPkt->data, pPkt->datalen);

// 	// Write metadata, and then pass to the receive level
// 	pSkB->dev = pDev;
// 	pSkB->protocol = eth_type_trans(pSkB, pDev);
// 	pSkB->ip_summed = CHECKSUM_UNNECESSARY;
// 	pPriv->stats.rx_packets++;
// 	pPriv->stats.rx_bytes += pPkt->datalen;
// 	netif_rx(pSkB);
// }

static irqreturn_t irq_rx_handler(int irq, void *devid)
{
	return IRQ_HANDLED;
}

static irqreturn_t irq_tx_handler(int irq, void *devid)
{
	int status_word;
	struct mfhss_priv_ *priv;
	struct mfhss_pkt_ *pkt = NULL;
	
	// As usual, check the "device" pointer to be sure it is really interrupting.
	// Then assign "struct device *dev"
	struct net_device *dev = (struct net_device *)devid;
	// and check with hw if it's really ours 

	// paranoid
	if (dev == NULL)
		return IRQ_NONE;

	// Lock the device
	priv = netdev_priv(dev);
	spin_lock(&priv->lock);
	// retrieve statusword: real netdevices use I/O instructions
	status_word = priv->status;
	// pPriv->status = 0;
	// if (status_word & DUMMY_NETDEV_RX_INTR) {
	// 	PDEBUG("rx interrupt occur at %s", pDev->name);
	// 	// send it to rxPkt for handling 
	// 	pPkt = dequeuePkt(pDev);
	// 	if (pPkt) {
	// 		PDEBUG("received new packet at %s, len %i", pDev->name, pPkt->datalen);
	// 		rxPkt(pDev, pPkt);
	// 	}
	// }
	if (status_word & DUMMY_NETDEV_TX_INTR) {
		PDEBUG("tx interrupt occur at %s", dev->name);
		// a transmission is over: free the skb
		priv->stats->tx_packets++;
		priv->stats->tx_bytes += priv->tx_pkt_len;
		dev_kfree_skb(priv->skb);
		priv->skb = NULL;
		status_word &= ~DUMMY_NETDEV_TX_INTR;
		priv->status = status_word;
	}

	// Unlock the device and we are done
	spin_unlock(&priv->lock);
	if (pkt) 
		pool_put(pkt); // Do this outside the lock!
	return IRQ_HANDLED;
}

static void tx_pkt_by_hw(char *buf, int len, struct net_device *dev)
{
	// This function deals with hw details. This interface loops back the packet to the other dummy interface (if any).
	// In other words, this function implements the dummy-device behaviour, while all other procedures are rather device-independent
	struct iphdr *ip;
	struct mfhss_priv_ *priv;
	u32 *saddr, *daddr;
	struct mfhss_pkt_ *tx_buffer;
    
	// I am paranoid. Ain't I?
	if (len < sizeof(struct ethhdr) + sizeof(struct iphdr))
	{
		PERR("packet too short (%i octets)", len);
		return;
	}
	
	// Ethhdr is 14 bytes, but the kernel arranges for iphdr
	ip = (struct iphdr *)(buf + sizeof(struct ethhdr));
	saddr = &ip->saddr;
	daddr = &ip->daddr;

	((u8*)saddr)[2] ^= 1; // change the third octet (class C)
	((u8*)daddr)[2] ^= 1;

	ip->check = 0;         // and rebuild the checksum (ip needs it)
	ip->check = ip_fast_csum((unsigned char *)ip, ip->ihl);

	PDEBUG("%s:%05i --> %s:%05i",
		ipaddr_to_str(ip->saddr),ntohs(((struct tcphdr *)(ip+1))->source),
		ipaddr_to_str(ip->daddr),ntohs(((struct tcphdr *)(ip+1))->dest));
			
	// Ok, now the packet is ready for transmission: first simulate a receive interrupt 
	// on the twin device, then a transmission-done on the transmitting device
	// pDest = dummyDevs[pDev == dummyDevs[0] ? 1 : 0];
	// pPriv = netdev_priv(pDest);
	tx_buffer = pool_get();
	if (tx_buffer != NULL)
	{
		tx_buffer->datalen = len;
	 	// fake transmit packet
		memcpy(tx_buffer->data, buf, len);
		PDEBUG("hw tx packet by %s, len is %i", dev->name, len);
		
	  	// fake receive packet
		// enqueuePkt(pDest, pTxBuffer);	
		// if (pPriv->bIsRxIntEnabled) 
		// {
		// 	pPriv->status |= DUMMY_NETDEV_RX_INTR;
		//  	dummyNetdevInterrupt(0, pDest, NULL);
		// }

		// terminate transmission
		priv = netdev_priv(dev);
		priv->tx_pkt_len = len;
		priv->tx_pkt_data = buf;
		priv->status |= DUMMY_NETDEV_TX_INTR;
		// regular_int_handler(0, dev, NULL);
		pool_put(tx_buffer);
	}
}

// this callback called after allocation net_device structure
static void mfhss_setup(struct net_device *dev)
{
	int err = 0;
	// The init function (sometimes called probe).
	// It is invoked by register_netdev()
	struct mfhss_priv_ *priv = netdev_priv(dev);

	// Then, initialize the priv field. This encloses the statistics and a few private fields.

	// PDEBUG("try to setup device 0x%p, pPriv=0x%p", pDev, pPriv);
	
	memset(priv, 0, sizeof(struct mfhss_priv_));
	spin_lock_init(&priv->lock);
	priv->dev = dev;

#if 0
    // Make the usual checks: check_region(), probe irq, ...  -ENODEV
	// should be returned if no device found.  No resource should be
	// grabbed: this is done on open(). 
#endif 

	// Then, assign other fields in dev, using ether_setup() and some hand assignments
	ether_setup(dev);	// assign some of the fields
	dev->watchdog_timeo = timeout;
	
	// keep the default flags, just add NOARP
	dev->flags |= IFF_NOARP;
	dev->features |= NETIF_F_HW_CSUM;
	
	dev->netdev_ops = &mfhss_net_device_ops;
	dev->header_ops = &mfhss_header_ops;
	
	rx_int_en(dev, 1);	// enable receive interrupts
	PRINT_ERR(err);	
}

static void mfhss_cleanup(enum destroy_stage_ from, struct net_device *dev)
{
	// dev != NULL because net device allocation the first thing of initialization
	// we can use it safely
	struct mfhss_priv_ *priv = netdev_priv(dev);

	switch (from)
	{
		case STAGE_DESTROY_ALL:
			PRINT_DSTR_STG(STAGE_DESTROY_ALL);

		case STAGE_UNREGISTER_NETDEV:
			PRINT_DSTR_STG(STAGE_UNREGISTER_NETDEV);
			unregister_netdev(dev);

		case STAGE_UNMAP_MEM_REGION:
			iounmap(priv->io_base);

		case STAGE_DESTROY_MEM_REGION:
			PRINT_DSTR_STG(STAGE_DESTROY_MEM_REGION);
			release_mem_region(priv->resource.start, resource_size(&priv->resource));

		case STAGE_CLEAN_DYNAMIC_REGS_DIR:
			PRINT_DSTR_STG(STAGE_CLEAN_DYNAMIC_REGS_DIR);
			clean_dir(priv->dynamic_regs);

		case STAGE_DESTROY_DYNAMIC_REGS_DIR:
			PRINT_DSTR_STG(STAGE_DESTROY_DYNAMIC_REGS_DIR);
			kset_unregister(priv->dynamic_regs);
			priv->dynamic_regs = NULL;

		case STAGE_CLEAN_STATIC_REGS_DIR:
			PRINT_DSTR_STG(STAGE_CLEAN_STATIC_REGS_DIR);
			clean_dir(priv->static_regs);

		case STAGE_DESTROY_STATIC_REGS_DIR:
			PRINT_DSTR_STG(STAGE_DESTROY_STATIC_REGS_DIR);
			kset_unregister(priv->static_regs);
			priv->static_regs = NULL;

		case STAGE_DESTROY_POOL:
			PRINT_DSTR_STG(STAGE_DESTROY_POOL);
			pool_destroy();

		case STAGE_DESTROY_STATS:
			PRINT_DSTR_STG(STAGE_DESTROY_STATS);
			if (priv->stats != NULL)
			{
				kfree(priv->stats);
				priv->stats = NULL;
			}

		case STAGE_DESTROY_NETDEV:
			PRINT_DSTR_STG(STAGE_DESTROY_NETDEV);
			if (dev != NULL) 
			{
				free_netdev(dev);
				dev = NULL;
			}
			break;
		default:
			mfhss_cleanup(STAGE_DESTROY_ALL, dev);
	}
}

static void mfhss_close_from(enum close_stage_ from, struct net_device *dev)
{
	struct mfhss_priv_ *priv;

	if (dev == NULL) 
	{
		PRINT_ERR(-EFAULT);
		return;
	}
	priv = netdev_priv(dev);

	switch(from)
	{
		case STAGE_FREE_ALL:
			PRINT_CLOSE_STG(STAGE_FREE_ALL);

		case STAGE_UNREG_IRQ_TX:
			PRINT_CLOSE_STG(STAGE_UNREG_IRQ_TX);
			free_irq(priv->irq_tx, dev);

		case STAGE_UNREG_IRQ_RX:
			PRINT_CLOSE_STG(STAGE_UNREG_IRQ_RX);
			free_irq(priv->irq_rx, dev);

		case STAGE_FREE_DMA_DST:
			PRINT_CLOSE_STG(STAGE_FREE_DMA_DST);
			dma_free_coherent(NULL, MFHSSNET_DMA_SIZE, priv->dst_addr, priv->dst_handle);

		case STAGE_FREE_DMA_SRC:
			PRINT_CLOSE_STG(STAGE_FREE_DMA_SRC);
			dma_free_coherent(NULL, MFHSSNET_DMA_SIZE, priv->src_addr, priv->src_handle);

		default:
			mfhss_close_from(STAGE_FREE_ALL, dev);
	}
}
	
/*
 * Net device operations
 */
static int mfhss_open(struct net_device *dev) 
{
	int err = 0;
	unsigned long flags = 0;
	struct mfhss_priv_ *priv = netdev_priv(dev);

	// request DMA memories
	priv->src_addr = dma_alloc_coherent(NULL, MFHSSNET_DMA_SIZE, &priv->src_handle, GFP_KERNEL);
	if (priv->src_addr == NULL)
	{
		PRINT_ERR(err = -ENOMEM);
		return err;
	}

	priv->dst_addr = dma_alloc_coherent(NULL, MFHSSNET_DMA_SIZE, &priv->dst_handle, GFP_KERNEL);
	if (priv->dst_addr == NULL)
	{
		PRINT_ERR(err = -ENOMEM);
		mfhss_close_from(STAGE_FREE_DMA_SRC, dev);
		return err;
	}

	// register interrupt handlers
	err = request_irq(priv->irq_rx, irq_rx_handler, IRQF_SHARED, DRIVER_NAME, dev);
	if (err != 0)
	{
		PRINT_ERR (err);
		mfhss_close_from(STAGE_FREE_DMA_DST, dev);
		return err;
	}

	err = request_irq(priv->irq_tx, irq_tx_handler, IRQF_SHARED, DRIVER_NAME, dev);
	if (err != 0)
	{
		PRINT_ERR (err);
		mfhss_close_from(STAGE_UNREG_IRQ_RX, dev);
		return err;
	}

	// start the device
	spin_lock_irqsave(&priv->lock, flags);
	REG_WR(DMA, SA, priv->src_handle);
	REG_WR(DMA, DA, priv->dst_handle);
	REG_WR(MLIP, RST, 1);
	REG_WR(MLIP, RST, 0);
	REG_WR(MLIP, IR, 1);
	REG_WR(DMA, IR, 1);
	REG_WR(MLIP, CE, 1);
	spin_unlock_irqrestore(&priv->lock, flags);

	// Assign the hardware address
	memcpy(dev->dev_addr, "\0FHSS0", ETH_ALEN);
	PDEBUG("MAC address %02x:%02x:%02x:%02x:%02x:%02x assigned to %s", 
		dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2], dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5],
		dev->name);

	// start xmit
	netif_start_queue(dev);
	PRINT_ERR(err);
	return err;
}

static int mfhss_close(struct net_device *dev)
{
	int err = 0;
	unsigned long flags = 0;
	struct mfhss_priv_ *priv = netdev_priv(dev);

	// запретить передачу
	netif_stop_queue(dev);

	// выключить устройство
	spin_lock_irqsave(&priv->lock, flags);
	REG_WR(MLIP, CE, 0);
	REG_WR(DMA, IR, 0);
	REG_WR(MLIP, IR, 0);
	spin_unlock_irqrestore(&priv->lock, flags);

	// освободить память
	mfhss_close_from(STAGE_FREE_ALL, dev);
	
	PRINT_ERR(err);
	return err;	
}

static int mfhss_config(struct net_device *dev, struct ifmap *map)
{
	int err = 0;

	// PDEBUG("configure the device %s", pDev->name);
	if (dev->flags & IFF_UP)
	{
		// can't act on a running interface
		err = EBUSY;
		PRINT_ERR(err);
		return -err;
	} 

	if (map->base_addr != dev->base_addr) {
		// can't change I/O address
		err = EOPNOTSUPP;
		PRINT_ERR(err);
		return -err;
	} 

	if (map->irq != dev->irq) {
		// Allow changing the IRQ
		dev->irq = map->irq;
	}

	// ignore other fields
	PRINT_ERR(err);
	return err;
}

static int mfhss_tx_pkt(struct sk_buff *skb, struct net_device *dev)
{
	int len;
	char *data, shortpkt[ETH_ZLEN];
	struct mfhss_priv_ *priv = netdev_priv(dev);
	
	data = skb->data;
	len = skb->len;
	if (len < ETH_ZLEN) {
		memset(shortpkt, 0, ETH_ZLEN);
		memcpy(shortpkt, skb->data, skb->len);
		len = ETH_ZLEN;
		data = shortpkt;
	}
	trans_start = jiffies; 	// save the timestamp
	priv->skb = skb;				// Remember the skb, so we can free it at interrupt time

	// actual deliver of data is device-specific, and not shown here
	tx_pkt_by_hw(data, len, dev);

	return NETDEV_TX_OK;
}

static int mfhss_ioctl(struct net_device *dev, struct ifreq *req, int cmd)
{
	int err = 0;
	struct mfhss_priv_ *priv = netdev_priv(dev);
	struct list_head *p;
	struct kobject *k;
	
	MFHSS_FILE_TypeDef file_descr;
	MFHSS_DIR_TypeDef dir_descr;
	
	if (req == NULL)
	{
		PRINT_ERR(err = -EFAULT);
		return err;
	}

	// can process
	switch (cmd)
	{
	case MFHSS_IORESET:
		// TODO: MFHSSDRV_IORESET not implemented
		PDEBUG("Command 0x%04x (MFHSS_IORESET) @ %s currently not implemented\n", cmd, req->ifr_name);
		err = -ENOTTY;
		break;

	case MFHSS_IOMAKEDIR:
		// PDEBUG("Perform 0x%04x (MFHSS_IOMAKEDIR) @ %s\n", cmd, req->ifr_name);
		// retrieve directory descriptor
		copy_from_user(&dir_descr, (const void __user *)req->ifr_data, sizeof dir_descr);
		// create new directory, based on descriptor
		err = create_dir(priv->dynamic_regs, priv, dir_descr.nodeName);
		break;

	case MFHSS_IOMAKEFILE:
		// забираем описание файла из пространства пользователя
		// PDEBUG("Perform 0x%04x (MFHSS_IOMAKEFILE)@%s", cmd, req->ifr_name);
		// retrieve directory descriptor
		copy_from_user(&file_descr, (const void __user *)req->ifr_data, sizeof file_descr);
		// try to find subdirectory
		list_for_each(p, &priv->dynamic_regs->list) 
		{
		 	k = list_entry(p, struct kobject, entry);
		 	if (!strcmp(k->name, file_descr.targetNode))
		 	{
		 		// success, try to create file
		 		err = create_file(k, file_descr.regName, file_descr.address);
				break;
			}
		}
		break;

	default:
		PERR("unsupported command: 0x%04x@%s\n", cmd, req->ifr_name);
		err = -ENOTTY;
	}
	PRINT_ERR(err);
	return err;
}


static int mfhss_change_mtu(struct net_device *dev, int new_mtu)
{
	// The "change_mtu" method is usually not needed.
	// If you need it, it must be like this.

	unsigned long flags = 0;
	int err = 0;
	struct mfhss_priv_ *priv = netdev_priv(dev);
	spinlock_t *plock = &priv->lock;
    
	// check ranges
	// PDEBUG("set new MTU=%i to %s", newMTU, pDev->name);
	if ((new_mtu < 68) || (new_mtu > 1500))
	{
		// PRINT_STATUS_MSG("MTU is out if range (68, 1500): %i", (err=-EINVAL), new_mtu);
		return err;
	}

	// Do anything you need, and the accept the value
	spin_lock_irqsave(plock, flags);
	dev->mtu = new_mtu;
	spin_unlock_irqrestore(plock, flags);
	// PRINT_STATUS(0);
	return 0;
}

static void mfhss_tx_timeout (struct net_device *dev)
{
	struct mfhss_priv_ *priv = netdev_priv(dev);

	PDEBUG("Transmit timeout at %ld, latency %ld\n", jiffies, jiffies - trans_start);
    
    // Simulate a transmission interrupt to get things moving
	priv->status = DUMMY_NETDEV_TX_INTR;
	// regular_int_handler(0, dev, NULL);
	priv->stats->tx_errors++;
	netif_wake_queue(dev);
	return;
}

static struct net_device_stats *mfhss_stats(struct net_device *dev)
{
	// PDEBUG("getting device stats for %s", pDev->name);
	struct mfhss_priv_ *priv = netdev_priv(dev);
	return priv->stats;
}

/*
 * Header operations
 */
static int mfhss_header(struct sk_buff *skb, struct net_device *dev, unsigned short type, const void *daddr, const void *saddr, unsigned len)
{
	struct ethhdr *eth = (struct ethhdr *)skb_push(skb, ETH_HLEN);

	eth->h_proto = htons(type);
	memcpy(eth->h_source, saddr ? saddr : dev->dev_addr, dev->addr_len);
	memcpy(eth->h_dest, daddr ? daddr : dev->dev_addr, dev->addr_len);
	eth->h_dest[ETH_ALEN-1]   ^= 0x01;   /* dest is us xor 1 */
	return (dev->hard_header_len);
}

/*
 * Platform device support
 */
static int mfhssnet_probe(struct platform_device *pl_dev)
{
	int err = 0;
	struct net_device *dev;
	struct mfhss_priv_ *priv;

	// Allocate the device
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 17, 0))
	dev = alloc_netdev(sizeof (struct mfhss_priv_), "mfhss%d", mfhss_setup);
#else
	dev = alloc_netdev(sizeof (struct mfhss_priv_), "mfhss%d", NET_NAME_UNKNOWN, mfhss_setup);
#endif
	if (dev == NULL)
	{
		PRINT_ERR(err = -ENOMEM);
		return err;
	} 
	priv = netdev_priv(dev);
	
	// Allocate interface stats
	priv->stats = kzalloc(sizeof *(priv->stats), GFP_KERNEL);
	if (priv == NULL)
	{
		PRINT_ERR(err = -ENOMEM);
		mfhss_cleanup(STAGE_DESTROY_NETDEV, dev);
		return err;
	}

	// Allocate pool
	err = pool_create(dev, ETH_DATA_LEN);
	if (err != 0)
	{
		PRINT_ERR(err);
		mfhss_cleanup(STAGE_DESTROY_STATS, dev);
		return err;
	}

	// Create static registers directory in sysfs
	priv->static_regs = kset_create_and_add("mfhss-static", NULL, NULL);
	if (priv->static_regs == NULL)
	{
		PRINT_ERR(err = -ENOMEM);
		mfhss_cleanup(STAGE_DESTROY_POOL, dev);
		return err;
	}
	
	// Create static registers subdirectories and files
	err = create_dma_subdir(priv->static_regs, priv);
	if (err != 0)
	{
		PRINT_ERR(err);
		// ниодной поддиректории не было создано, так что просто сносим каталог
		mfhss_cleanup(STAGE_DESTROY_STATIC_REGS_DIR, dev);
		return err;
	}
	err = create_mlip_subdir(priv->static_regs, priv);
	if (err != 0)
	{
		PRINT_ERR(err);
		// если была создана хотя бы одна поддиректория, нужно вызывать очистку каталога, и только потом его снос
		mfhss_cleanup(STAGE_CLEAN_STATIC_REGS_DIR, dev);
		return err;
	}

	// Create dynamic registers directory in sysfs
	priv->dynamic_regs = kset_create_and_add("mfhss-dynamic", NULL, NULL);
	if (priv->dynamic_regs == NULL)
	{
		PRINT_ERR(err = -ENOMEM);
		mfhss_cleanup(STAGE_CLEAN_STATIC_REGS_DIR, dev);
		return err;
	}
	// all subdirectories will be create dinamically

	// извлечь диапазон физических адресов
	err = of_address_to_resource(pl_dev->dev.of_node, 0, &priv->resource);
	if (err != 0)
	{
		PRINT_ERR(err);
		mfhss_cleanup(STAGE_CLEAN_DYNAMIC_REGS_DIR, dev);
		return err;
	}
	PDEBUG("physical memory range: 0x%x-0x%x\n", priv->resource.start, priv->resource.end);

	// запрос адресного проcтранства у ядра, чтобы не возникло коллизии с другими модулями
	if (request_mem_region(priv->resource.start, resource_size(&priv->resource), DRIVER_NAME) == NULL)
	{
		PRINT_ERR(err = -ENOMEM);
		mfhss_cleanup(STAGE_CLEAN_DYNAMIC_REGS_DIR, dev);
		return err;
	}

	// маппинг физических адресов на виртуальные
	priv->io_base = of_iomap(pl_dev->dev.of_node, 0);
	if (priv->io_base == NULL)
	{
		PRINT_ERR(err = -ENOMEM);
		mfhss_cleanup(STAGE_DESTROY_MEM_REGION, dev);
		return err;
	}
	
	// извлечь номера прерываний
	priv->irq_rx = irq_of_parse_and_map(pl_dev->dev.of_node, 0);
	priv->irq_tx = irq_of_parse_and_map(pl_dev->dev.of_node, 1);
	PDEBUG("irq_rx=%d, irq_tx=%d\n", priv->irq_rx, priv->irq_tx);

	// That's ok! Register the device
	if ((err = register_netdev(dev)) != 0)
	{
		PRINT_ERR(err);
		mfhss_cleanup(STAGE_UNMAP_MEM_REGION, dev);
		return err;
	} 
	PDEBUG("%s successfully registered!", dev->name);

	// now save it
	platform_set_drvdata(pl_dev, dev);

	PRINT_ERR(err);
	return err;
}

static int mfhssnet_remove (struct platform_device *pl_dev)
{
	int err = 0;
	struct net_device *dev = platform_get_drvdata(pl_dev);

	mfhss_cleanup(STAGE_DESTROY_ALL, dev);
	PRINT_ERR(err);
	return err;
}

/*
 * Entry/exit point functions
 */
static int __init mfhss_init(void)
{
	int err = 0;

	err = platform_driver_register(&mfhssnet_driver);
	PRINT_ERR(err);
	return err;
}

static void __exit mfhss_exit(void)
{	
	platform_driver_unregister(&mfhssnet_driver);
}

module_init(mfhss_init);
module_exit(mfhss_exit);
