#include "../inc/max30102.h"
#include <drv_common.h>
#include "user.h"

rt_int16_t init_max30102_port(void)
{
    struct rt_sensor_config cfg = {
        .intf.dev_name = "i2c2",            // i2c bus name
        .irq_pin.pin = GET_PIN(E, 0),                   // interrupt pin
        .mode = RT_SENSOR_MODE_POLLING,     // must have
    };
    if (rt_hw_max30102_init(&cfg) != RT_EOK)
        return RT_ERROR;
    
    return RT_EOK;
}
//INIT_APP_EXPORT(max30102_port_init);
