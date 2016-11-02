#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 projection;
uniform mat4 modelview;
uniform int transformID;
uniform float alpha;

in vec3 v_position[];
in vec3 v_normal[];
in vec2 v_tex_coords[];

out vec2 v_tex_coord;

void main(void)
{
  //ECLATEMENT PAR TRANSLATION SELON LE CENTRE DE GRAVITE
  if( transformID == 0 )
    {
    vec4 facePosition = vec4( 0, 0, 0, 0 );
    for (int i = 0; i < 3; i++)
      {
      facePosition = facePosition + vec4(v_position[i], 1.0);
      }
    facePosition = facePosition / 3.0;
  
    for (int i = 0; i < 3; i++)
      {
      vec4 directionToCenter = facePosition - vec4(v_position[i], 1.0);
      gl_Position = projection * modelview * ( vec4(v_position[i], 1.0) + alpha * directionToCenter ) ;
      v_tex_coord = v_tex_coords[i];
      EmitVertex();
      }
    EndPrimitive();
    }

  //ECLATEMENT PAR TRANSLATION SELON LA NORMAL
  if( transformID == 1 )
    {
    vec4 faceNormal = vec4( 0, 0, 0, 0 );
    for( int i = 0; i < 3; i++ )
      {
      faceNormal = faceNormal + vec4(v_normal[i], 0.0);
      }
    faceNormal = faceNormal / 3.0;
  
    for( int i = 0; i < 3; i++ )
      {
      vec4 directionToCenter = faceNormal;
      gl_Position = projection * modelview * ( vec4(v_position[i], 1.0) + alpha * directionToCenter ) ;
      v_tex_coord = v_tex_coords[i];
      EmitVertex();
      }
    EndPrimitive();
    }

  //DILATATION/EROSION
  if( transformID == 2 )
    {
    for (int i = 0; i < 3; i++)
      {
      gl_Position = projection * modelview * ( vec4(v_position[i], 1.0) + alpha * vec4(v_normal[i],0.0) ) ;
      v_tex_coord = v_tex_coords[i];
      EmitVertex();
      }
    EndPrimitive();
    }
}
