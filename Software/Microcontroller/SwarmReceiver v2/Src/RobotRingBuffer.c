#include "RobotRingBuffer.h"

void RobotRingBuffer_init(RobotRingBuffer *_this)
{
    /*****
      The following clears:
        -> buf
        -> head
        -> tail
        -> count
      and sets head = tail
    ***/
    memset(_this, 0, sizeof(*_this));
}

unsigned int RobotRingBuffer_modulo_inc(const unsigned int value, const unsigned int modulus)
{
    unsigned int my_value = value + 1;
    if (my_value >= modulus)
    {
        my_value = 0;
    }
    return (my_value);
}

unsigned int RobotRingBuffer_modulo_dec(const unsigned int value, const unsigned int modulus)
{
    unsigned int my_value = (0 == value) ? (modulus - 1) : (value - 1);
    return (my_value);
}

uint8_t RobotRingBuffer_empty(RobotRingBuffer *_this)
{
    if (_this)
        return (0 == _this->count);
    else
        return 0;
}

void RobotRingBuffer_flush(RobotRingBuffer *_this, uint8_t clearBuffer)
{
    if (_this)
    {
        _this->count = 0;
        _this->head = 0;
        _this->tail = 0;
        if (clearBuffer)
        {
            memset(_this->buf, 0, sizeof(_this->buf));
        }
    }
}

bool RobotRingBuffer_full(RobotRingBuffer *_this)
{
    if (_this)
        return (_this->count >= ROBOTRINGBUFFER_SIZE);
    else
        return false;
}

Robot *RobotRingBuffer_pop(RobotRingBuffer *_this)
{
    Robot *r = NULL;
    if (_this)
    {
        if (_this->count > 0)
        {
            r = &(_this->buf[_this->tail]);
            _this->tail = RobotRingBuffer_modulo_inc(_this->tail, ROBOTRINGBUFFER_SIZE);
            --_this->count;
        }
    }
    return r;
}

Robot *RobotRingBuffer_get(RobotRingBuffer *_this, uint8_t index)
{
    Robot *r = NULL;
    if (_this)
    {
        if (_this->count > 0 && index < _this->count)
            r = &(_this->buf[(_this->tail + index) % ROBOTRINGBUFFER_SIZE]);
    }
    return r;
}

Robot *RobotRingBuffer_findWithId(RobotRingBuffer *_this, uint8_t id)
{
    Robot *r = NULL;
    if (_this)
    {
        for (uint16_t i = 0; i < _this->count; i++)
        {
            if (_this->buf[(_this->tail + i) % ROBOTRINGBUFFER_SIZE].id == id)
            {
                r = &(_this->buf[(_this->tail + i) % ROBOTRINGBUFFER_SIZE]);
                break;
            }
        }
    }
    return r;
}

void RobotRingBuffer_push(RobotRingBuffer *_this, Robot value)
{
    if (_this)
    {
        if (_this->count < ROBOTRINGBUFFER_SIZE)
        {
            _this->buf[_this->head] = value;
            _this->head = RobotRingBuffer_modulo_inc(_this->head, ROBOTRINGBUFFER_SIZE);
            ++_this->count;
        }
        else
        {
            _this->buf[_this->head] = value;
            _this->head = RobotRingBuffer_modulo_inc(_this->head, ROBOTRINGBUFFER_SIZE);
            _this->tail = RobotRingBuffer_modulo_inc(_this->tail, ROBOTRINGBUFFER_SIZE);
        }
    }
}

bool RobotRingBuffer_erase(RobotRingBuffer *_this, uint8_t range)
{
    if (_this)
    {
        if (_this->count >= range)
        {
            _this->tail = (_this->tail + range) % ROBOTRINGBUFFER_SIZE;
            _this->count -= range;
            return true;
        }
    }
    return false;
}