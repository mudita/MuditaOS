/**
 * Project Untitled
 * @brief The indication primitive is sent to the upper layer when the TS 0710 layer of the receiving unit receives a
 * request to wake up from the power saving state. When the receiving device is ready to resume operation on the
 * multiplexer channel this is indicated to the TS 0710 layer in the receiving unit by means of the response primitive.
 * Sins the wakeup routine is initiated by the transmitting device attempting to communicate, neither request nor
 * confirm primitives are provided for the wakeup service. The transmitting device instead uses the Data services
 * described below.
 */

#ifndef _TS0710_WAKEUP_H
#define _TS0710_WAKEUP_H

class TS0710_WAKEUP
{
  public:
    TS0710_WAKEUP()
    {}
    ~TS0710_WAKEUP()
    {}

  private:
    void indication();

    void response();
};

#endif //_TS0710_WAKEUP_H