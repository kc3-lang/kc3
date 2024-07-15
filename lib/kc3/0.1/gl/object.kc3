defmodule GL.Object do

  defstruct [vertex: (GL.Vertex[]) {},
             triangle: (GL.Triangle[]) {},
             gl_mode: (U32) 0,
             gl_vao: (U32) 0,
             gl_vbo: (U32) 0,
             gl_ebo: (U32) 0]

  def clean = cfn Void "gl_object_clean" (GL.Object)

end
