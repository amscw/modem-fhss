#ifndef _MFHSSNET_H
#define _MFHSSNET_H

#include "common.h"

//-------------------------------------------------------------------------------------------------
// MACRO
//-------------------------------------------------------------------------------------------------
// These are the flags in the statusword
#define DUMMY_NETDEV_RX_INTR 	0x0001
#define DUMMY_NETDEV_TX_INTR 	0x0002

// hardcoded registers
#define REG_DMA_CR_NAME			"cr"
#define REG_DMA_CR_ADDRESS		0x0004
#define REG_DMA_SR_NAME			"sr"
#define REG_DMA_SR_ADDRESS		0x0008
#define REG_DMA_IR_NAME			"ir"
#define REG_DMA_IR_ADDRESS		0x000C
#define REG_DMA_SA_NAME			"sa"
#define REG_DMA_SA_ADDRESS		0x0010
#define REG_DMA_DA_NAME			"da"
#define REG_DMA_DA_ADDRESS		0x0014
#define REG_DMA_SL_NAME			"sl"
#define REG_DMA_SL_ADDRESS		0x0018
#define REG_DMA_DL_NAME			"dl"
#define REG_DMA_DL_ADDRESS		0x001C
#define REG_MLIP_SR_NAME		"sr"
#define REG_MLIP_SR_ADDRESS		0x0020
#define REG_MLIP_IR_NAME		"ir"
#define REG_MLIP_IR_ADDRESS		0x0024
#define REG_MLIP_RST_NAME		"rst"
#define REG_MLIP_RST_ADDRESS	0x0028
#define REG_MLIP_CE_NAME		"ce"
#define REG_MLIP_CE_ADDRESS		0x002C
#define REG_LINK_SR_NAME		"sr" 
#define REG_LINK_SR_ADDRESS		0x0030
#define REG_LINK_IR_NAME		"ir"
#define REG_LINK_IR_ADDRESS		0x0034

//-------------------------------------------------------------------------------------------------
// Types
//-------------------------------------------------------------------------------------------------
// Incompletes
struct net_device;
struct net_device_stats;
struct sk_buff;
struct kset;

// Payload data packet structure
struct mfhss_pkt_
{
	struct net_device *dev;
	int datalen;
	u8 *data; //ETH_DATA_LEN
	struct list_head list;
};

// Main device structure
struct mfhss_priv_ {
	// platform data
	struct resource resource;
	void __iomem *io_base;
	char *src_addr;
	char *dst_addr;
	dma_addr_t src_handle;
	dma_addr_t dst_handle;
	int irq_rx;
	int irq_tx;
	int irq_link_on;

	// network data
	struct list_head rx_pkts_list;

	struct net_device_stats *stats;
	struct sk_buff *skb;
	int tx_pkt_len;
	
	// locker
	spinlock_t lock;
	
	// sysfs data
	struct kset *static_regs;
	struct kset *dynamic_regs;

	// flags
	union 
	{
		struct {
			unsigned link_on : 1;				// !!! read-only for app, need lock !!!
		} __attribute__ ((__packed__)) bits;
		u32 word;
	} flags;
};


//-------------------------------------------------------------------------------------------------
// Varibles
//-------------------------------------------------------------------------------------------------
extern struct net_device *mfhss_dev;
#endif // _MFHSSNET_H
