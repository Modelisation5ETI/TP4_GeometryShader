#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 projection;
uniform mat4 modelview;

uniform vec4 cullingPoint;

in vec3 v_position[];
in vec3 v_normal[];
in vec2 v_tex_coords[];

out vec2 v_tex_coord;

void main(void)
{
  // Compute face orientation and position
  vec4 faceOrientation = vec4( 0, 0, 0, 0 );
  vec4 facePosition = vec4( 0, 0, 0, 0 );
  for (int i = 0; i < 3; i++)
    {
    faceOrientation = faceOrientation + vec4( v_normal[i], 0.0 );
    facePosition = facePosition + vec4(v_position[i], 1.0);
    }
  faceOrientation = faceOrientation / 3.0;
  facePosition = facePosition / 3.0;
  
  // Check if the triangle is oriented towards the culling point
  vec4 cullingDirection = cullingPoint - facePosition;
  if( dot( cullingDirection, faceOrientation ) >= 0 )
    {
    for (int i = 0; i < 3; i++)
      {
      gl_Position = projection * modelview * vec4(v_position[i], 1.0);
      v_tex_coord = v_tex_coords[i];
      EmitVertex();
      }
    }
  
  EndPrimitive();
}
