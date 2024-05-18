defmodule GL.Sphere do

  defstruct [object: %GL.Object{},
             segments_u: (Uw) 3,
             segments_v: (Uw) 2]

  def clean = cfn Void "gl_sphere_clean" (GL.Sphere)

end
