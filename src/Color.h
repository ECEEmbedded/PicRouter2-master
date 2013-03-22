#ifndef __Color_H_
#define __Color_H_

    typedef struct DRIVERColorMEMBERS {
        unsigned char id;
        unsigned char class;
  

    unsigned char a;

} DriverColorMembers;

    typedef struct DRIVERColorDATA {
  

  unsigned char input;

} DriverColorData;
void DriverColorInit(Driver_t *driver) {
DriverColorMembers *self = (DriverColorMembers *)driver;

    self->a = 0xE;
    DebugPrint(self->a);

}
void DriverColorRespond(Driver_t *driver, unsigned char *rcvData) {
DriverColorData *data = (DriverColorData *)rcvData;
DriverColorMembers *self = (DriverColorMembers *)driver;

  start_UART_send(8, data);

}
void DriverColorPoll(Driver_t *driver) {
DriverColorMembers *self = (DriverColorMembers *)driver;

    //DebugPrint(self->id);
    i2c_master_recv(self->id, 8);

}

    void DriverColorAdd(unsigned char id) {
      Driver_t *context = (Driver_t *)driverMalloc(sizeof(Driver_t));
      context->id = id;
      context->class = 0x20;
      DriverTable[NumberOfDrivers].context = context;
      DriverTable[NumberOfDrivers].respond = DriverColorRespond;
      DriverTable[NumberOfDrivers].poll = DriverColorPoll;
      ++NumberOfDrivers;

      DriverColorInit(context);
    }
  
#endif __Color_H_
