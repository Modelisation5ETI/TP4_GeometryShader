#version 330

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

uniform mat4 projection;
uniform mat4 modelview;

uniform float scaleNormal;

in vec3 v_position[];
in vec3 v_normal[];

void main(void)
{
  for (int i = 0; i < 3; i++)
  {
    // Line start point
    gl_Position = projection * modelview * vec4(v_position[i], 1.0);
    EmitVertex();
    // Line end point
    gl_Position = projection * modelview * vec4( v_position[i] + scaleNormal * v_normal[i], 1.0 );
    EmitVertex();
    
    // Send line primitive
    EndPrimitive();
  }
}
