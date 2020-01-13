#ifndef _POOL_H
#define _POOL_H

#include "common.h"

//-------------------------------------------------------------------------------------------------
// MACRO
//-------------------------------------------------------------------------------------------------
#define POOL_SIZE	8

//-------------------------------------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------------------------------------
int pool_create(struct net_device *dev, size_t maxlen);
void pool_destroy(void);
struct mfhss_pkt_ *pool_get(void);
size_t pool_put(struct mfhss_pkt_ *pkt);
// void enqueue_pkt(struct net_device *dev, struct mfhss_pkt_ *pkt);
// struct mfhss_pkt_ *dequeue_pkt(struct net_device *dev);

#endif // _POOL_H
