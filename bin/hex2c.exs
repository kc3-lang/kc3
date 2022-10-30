#!/usr/bin/env elixir
defmodule Hex do
  def to_c_(""), do: ""
  def to_c_(<<a, b, rest::binary>>), do: "\\x" <> List.to_string([a]) <> List.to_string([b]) <> to_c_(rest)

  def to_c(hex) when is_binary(hex) do
    IO.puts "\"" <> to_c_(hex) <> "\""
  end
  def to_c([a | b]) do
    to_c(a)
    to_c(b)
  end
  def to_c([]) do
    :ok
  end
end

Hex.to_c(System.argv)
