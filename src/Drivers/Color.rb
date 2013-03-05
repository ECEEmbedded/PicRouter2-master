@class = "20"
@desc = "Color sensing driver"

@members = %{
    unsigned char a;
}

@data = %{
  unsigned char input;
}

@init = %{
    self->a = 0xE;
    DebugPrint(self->a);
}

@respond = %{
  start_UART_send(8, data);
}

@poll = %{
    //DebugPrint(self->id);
    i2c_master_recv(self->id, 0x10, 8);
}