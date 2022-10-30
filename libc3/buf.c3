defmodule Buf do
  defstruct do
    Str str
    u64 rpos
    u64 wpos
    sw flush (Buf buf)
    sw refill (Buf buf)
    
