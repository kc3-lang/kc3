defmodule List do

  def cast = cfn List "list_init_cast" (Result, Sym, Tag)

  def map = fn {
    ([], _) do
      [] }
    end
    ([a | b], f) do
      [f(a) | map(b, f)]
    end
  }

  def reverse = fn {
    (x) { reverse(x, []) }
    ([], acc) { acc }
    ([a | b], acc) { reverse(b, [a | acc]) }
  }

end
