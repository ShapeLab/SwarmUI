#include "RingBuffer.h"

void RingBuffer_init(RingBuffer *_this)
{
    /*****
      The following clears:
        -> buf
        -> head
        -> tail
        -> count
      and sets head = tail
    ***/
    memset (_this, 0, sizeof (*_this));
}

unsigned int RingBuffer_modulo_inc(const unsigned int value, const unsigned int modulus)
{
    unsigned int my_value = value + 1;
    if (my_value >= modulus)
    {
      my_value  = 0;
    }
    return (my_value);
}

unsigned int RingBuffer_modulo_dec(const unsigned int value, const unsigned int modulus)
{
    unsigned int my_value = (0==value) ? (modulus - 1) : (value - 1);
    return (my_value);
}

uint8_t RingBuffer_empty(RingBuffer *_this)
{
    return (0==_this->count);
}

void RingBuffer_flush(RingBuffer *_this, uint8_t clearBuffer)
{
  _this->count  = 0;
  _this->head   = 0;
  _this->tail   = 0;
  if (clearBuffer)
  {
    memset (_this->buf, 0, sizeof (_this->buf));
  }
}

bool RingBuffer_full(RingBuffer *_this)
{
    return (_this->count>=RINGBUFFER_SIZE);
}

uint8_t RingBuffer_pop(RingBuffer *_this)
{
    uint8_t c;
    if (_this->count>0)
    {
      c = _this->buf[_this->tail];
      _this->tail = RingBuffer_modulo_inc (_this->tail, RINGBUFFER_SIZE);
      --_this->count;
    }
    else
    {
      c = 0xff;
    }
    return c;
}

uint8_t RingBuffer_get(RingBuffer *_this, uint8_t index)
{
    if (_this->count>0)
      return _this->buf[(_this->tail+index) % RINGBUFFER_SIZE];
    else
      return 0xff;
}

void RingBuffer_push(RingBuffer *_this, uint8_t value)
{
    if (_this->count < RINGBUFFER_SIZE)
    {
      _this->buf[_this->head] = value;
      _this->head = RingBuffer_modulo_inc (_this->head, RINGBUFFER_SIZE);
      ++_this->count;
    }
    else
    {
      _this->buf[_this->head] = value;
      _this->head = RingBuffer_modulo_inc (_this->head, RINGBUFFER_SIZE);
      _this->tail = RingBuffer_modulo_inc (_this->tail, RINGBUFFER_SIZE);
    }
}

bool RingBuffer_erase(RingBuffer *_this, uint8_t range)
{
    if (_this->count>=range)
    {
        _this->tail = (_this->tail + range)% RINGBUFFER_SIZE;
        _this->count -= range;
      return true;
    }
    else
      return false;
}
