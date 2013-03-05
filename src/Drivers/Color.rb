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
  //TRISA = 0;
  LATAbits.LATA2 = !LATAbits.LATA2;
  //i2c_master_recv(data->input, 0x10, 8);
}

@poll = %{
    //DebugPrint(self->id);
    i2c_master_recv(0x3F, 0x10, 8);
}