@class = "3A"
@desc = "Infrared Detection"

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
}

@poll = %{
    //DebugPrint(self->id);
}