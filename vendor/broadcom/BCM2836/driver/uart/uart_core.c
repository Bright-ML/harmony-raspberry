#include "errno.h"
#include "string.h"
#include "uart_dev.h"
#include "user_copy.h"

int uart_dev_read(struct uart_driver_data *udd, char *buf, size_t count)
{
    struct uart_ioc_transfer *transfer = NULL;
    unsigned int wp, rp;
    unsigned long data;
    int ret = 0;

    if (udd == NULL) {
        uart_error("udd is null");
        return -EFAULT;
    }

    transfer = udd->rx_transfer;
    if (transfer == NULL) {
        uart_error("transfer is null");
        return -EFAULT;
    }
    if (buf == NULL) {
        uart_error("buf is nullptr\n");
        return -EFAULT;
    }

    wp = transfer->wp;
    rp = transfer->rp;
    data = (unsigned long)(uintptr_t)transfer->data;

    if (!(transfer->flags & BUF_CIRCLED)) {
        if (count >= (wp - rp)) {
            count = wp - rp;
        }
        ret = LOS_CopyFromKernel(buf, BUF_SIZE, (void *)(uintptr_t)(data + rp), count);
        if (ret){
            return -EFAULT;
        }
        transfer->rp += count;
        return count;
    } else {
        if (count < (BUF_SIZE - rp)) {
            ret = LOS_CopyFromKernel(buf, BUF_SIZE, (void *)(uintptr_t)(data + rp), count);
            if (ret) {
                return -EFAULT;
            }
            transfer->rp += count;
            return count;
        } else {
            unsigned int copy_size = BUF_SIZE - rp;
            unsigned int left_size = count - copy_size;
            ret = LOS_CopyFromKernel(buf, BUF_SIZE, (void *)(uintptr_t)(data + rp), copy_size);
            if (ret) {
                return -EFAULT;
            }
            rp = 0;
            if (left_size > wp) {
                left_size = wp;
            }
            ret = LOS_CopyFromKernel((void *)(buf + copy_size), BUF_SIZE, (void *)(uintptr_t)(data + rp), left_size);
            if (ret) {
                return -EFAULT;
            }
            transfer->rp = left_size;
            transfer->flags &= ~BUF_CIRCLED;
            return (copy_size + left_size);
        }
    }
}

static int notify(struct wait_queue_head *wait)
{
    if (wait == NULL) {
        return -EINVAL;
    }

    LOS_EventWrite(&wait->stEvent, 0x1);
    notify_poll(wait);
    return 0;
}

int uart_recv_notify(struct uart_driver_data *udd, const char *buf, size_t count)
{//PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    struct uart_ioc_transfer *transfer = NULL;
    unsigned int wp, rp;
    unsigned long data;
    int ret = 0;

    if (udd == NULL) {
        uart_error("udd is null");
        return -EFAULT;
    }

    transfer = udd->rx_transfer;
    if (transfer == NULL) {
        uart_error("transfer is null");
        return -EFAULT;
    }
    wp = transfer->wp;
    rp = transfer->rp;
    data = (unsigned long)(uintptr_t)transfer->data;

    if (!(transfer->flags & BUF_CIRCLED)) {
        if (count < (BUF_SIZE - wp)) {
            ret = LOS_CopyToKernel((void *)(uintptr_t)(data + wp), BUF_SIZE, (void *)(buf), count);
            if (ret) {
                return -EFAULT;
            }
            transfer->wp += count;
            notify(&udd->wait);
            return count;
        } else {
            unsigned int copy_size = BUF_SIZE - wp;
            unsigned int left_size = count - copy_size;
            ret = LOS_CopyToKernel((void *)(uintptr_t)(data + wp), BUF_SIZE, (void *)(buf), copy_size);
            if (ret) {
                return -EFAULT;
            }
            wp = 0;
            if (left_size > rp) {
                /* overflowed. some new data will be discarded */
                uart_error("[%d]rx buf overflow", udd->num);
                left_size = rp;
            }
            ret = LOS_CopyToKernel((void *)(uintptr_t)(data + wp), BUF_SIZE, (void *)(buf + copy_size), left_size);
            if (ret) {
                return -EFAULT;
            }
            transfer->wp = left_size;
            transfer->flags |= BUF_CIRCLED;
            notify(&udd->wait);
            return (copy_size + left_size);
        }
    } else {
        if (count > (rp - wp)) {
            /* overflowed. some new data will be discarded */
            uart_error("[%d]rx buf overflow", udd->num);
            count = rp - wp;
        }
        ret = LOS_CopyToKernel((void *)(uintptr_t)(data + wp), BUF_SIZE, (void *)(buf), count);
        if (ret) {
                return -EFAULT;
        }
        transfer->wp += count;
        notify(&udd->wait);
        return count;
    }
}

int uart_rx_buf_empty(struct uart_driver_data *udd)
{
    struct uart_ioc_transfer *transfer = udd->rx_transfer;
    return (transfer->wp == transfer->rp);
}

