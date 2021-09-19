#include <stdint.h>

#include "nrf.h"
#include "string.h"
#include "nrfx_twi.h"
#include "queue.h"

#define MAX_I2C_CHANNEL NRFX_TWI_ENABLED_COUNT

struct tx_item {
    uint8_t len; // 发送数据长度
    uint8_t index; // 当前数据index
    uint8_t addr; // 目标地址
    uint8_t cmd;
    struct {
        bool is_pointer : 1; // 数据位是否是指针
        bool data_send : 1; // 数据是否发送完毕
        bool sequence : 1; // 是否以连续方式发送，若否则
    } flag;
    union data_or_pointer {
        const uint8_t* data_pointer; // 数据指针
        uint8_t data[4]; // 数据本体
    } data;
    nrfx_twi_t* channel; // 使用的物理通道
};
QUEUE(struct tx_item, twi_tx_buff, 20);

struct i2c_channel {
    uint8_t sda;
    uint8_t scl;
    nrfx_twi_t twi;
};

static struct i2c_channel channels[MAX_I2C_CHANNEL];
static uint8_t i2c_channel_count = 0;
static uint8_t buff[2];

static void shared_i2c_tx_item_send(struct tx_item* item)
{
    if (item->flag.sequence) {
        nrfx_twi_tx(item->channel, item->addr, item->flag.is_pointer ? item->data.data_pointer : item->data.data, item->len, false);
        item->index = item->len;
    } else {

        buff[0] = item->cmd;
        if (item->flag.is_pointer) {
            buff[1] = item->data.data_pointer[item->index];
        } else {
            buff[1] = item->data.data[item->index];
        }

        nrfx_twi_tx(item->channel, item->addr, buff, 2, false);
        item->index++;
    }
    // 数据长度一致，标志为已发送
    if (item->index >= item->len)
        item->flag.data_send = true;
}

void shared_i2c_write(nrfx_twi_t* channel, uint8_t addr, uint8_t len, const uint8_t* data, uint8_t cmd, bool sequence)
{
    struct tx_item item = {
        .len = len,
        .index = 0,
        .addr = addr,
        .cmd = cmd,
        .channel = channel
    };

    item.flag.data_send = false;
    item.flag.sequence = sequence;

    // 数据较短，尝试直接复制
    if (len <= 4) {
        item.flag.is_pointer = false;
        memcpy(item.data.data, data, len);
    } else {
        item.flag.is_pointer = true;
        item.data.data_pointer = data;
    }

    twi_tx_buff_push(item);

    if (!nrfx_twi_is_busy(channel)) {
        // 如果当前等待队列为空，则尝试发送
        shared_i2c_tx_item_send(twi_tx_buff_peek());
    }
}

/**
 * @brief 事件处理
 * 
 * @param p_event 
 * @param p_context 
 */
static void shared_i2c_handler(nrfx_twi_evt_t const* p_event, void* p_context)
{
    switch (p_event->type) {
    case NRFX_TWI_EVT_ADDRESS_NACK:
    case NRFX_TWI_EVT_DATA_NACK:
    case NRFX_TWI_EVT_DONE:
        if (p_event->xfer_desc.type == NRFX_TWI_XFER_TX) {
            // 当前队列不空
            if (!twi_tx_buff_empty()) {
                struct tx_item* item = twi_tx_buff_peek();
                // 如果这个是已经发送过的
                if (item->flag.data_send) {
                    // 就出队列
                    twi_tx_buff_pop();
                    // 然后尝试发送下一个
                    if (!twi_tx_buff_empty()) {
                        shared_i2c_tx_item_send(twi_tx_buff_peek());
                    }
                } else {
                    // 不然就尝试发送
                    shared_i2c_tx_item_send(item);
                }
            }
        }
        break;
    default:
        break;
    }
}

/**
 * @brief 初始化一个I2C通道
 * 
 * @param sda SDA引脚
 * @param scl SCL引脚
 * @return nrfx_twi_t* 通道操作指针
 */
nrfx_twi_t* shared_i2c_init(uint8_t sda, uint8_t scl)
{
    // 查找已有的通道
    for (uint8_t i = 0; i < i2c_channel_count; i++)
    {
        if (channels[i].scl == scl && channels[i].sda == sda) {
            return &channels[i].twi;
        }
    }

    // 物理通道已经满了，不能继续分配
    if (i2c_channel_count >= MAX_I2C_CHANNEL)
        return NULL;

    // 找不到就新建一个
    nrfx_twi_config_t config = {
        .scl = scl,
        .sda = sda,
        .frequency = NRF_TWI_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH
    };
    
    channels[i2c_channel_count].sda = sda;
    channels[i2c_channel_count].scl = scl;
    switch (i2c_channel_count)
    {
    case 0:
        channels[i2c_channel_count].twi.p_twi = NRF_TWI0;
        channels[i2c_channel_count].twi.drv_inst_idx = NRFX_TWI0_INST_IDX;
        break;
#if NRFX_TWI_ENABLED_COUNT > 1
    case 1:
        channels[i2c_channel_count].twi.p_twi = NRF_TWI1;
        channels[i2c_channel_count].twi.drv_inst_idx = NRFX_TWI1_INST_IDX;
        break;
#endif
    default:
        return NULL;
        break;
    }
    
    uint32_t err_code = nrfx_twi_init(&channels[i2c_channel_count].twi, &config, shared_i2c_handler, NULL);
    if (err_code != NRF_SUCCESS)
        return NULL;

    nrfx_twi_enable(&channels[i2c_channel_count].twi);
    return &channels[i2c_channel_count++].twi;
}