#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 projection;
uniform mat4 modelview;

in vec3 v_position[];
in vec3 v_normal[];
in vec2 v_tex_coords[];

out vec2 v_tex_coord;

void main(void)
{
  float alpha = 1.5;
 //ECLATEMENT PAR RAPROCHEMENT VERS CENTRE DE GRAVITE
//   vec4 facePosition = vec4( 0, 0, 0, 0 );
//   for (int i = 0; i < 3; i++)
//   {
//     facePosition = facePosition + vec4(v_position[i], 1.0);
//   }
//   facePosition = facePosition / 3.0;
//   
// 
//   for (int i = 0; i < 3; i++)
//     {
//     vec4 directionToCenter = facePosition - vec4(v_position[i], 1.0);
//     gl_Position = projection * modelview * ( vec4(v_position[i], 1.0) + alpha * directionToCenter ) ;
//     v_tex_coord = v_tex_coords[i];
//     EmitVertex();
//     }
//   
//   EndPrimitive();

//  //ECLATEMENT TRANSLATION SELON NORMAL
//   vec4 faceNormal = vec4( 0, 0, 0, 0 );
//   for (int i = 0; i < 3; i++)
//   {
//     faceNormal = faceNormal + vec4(v_normal[i], 1.0);
//   }
//   faceNormal = faceNormal / 3.0;
//   
// 
//   for (int i = 0; i < 3; i++)
//     {
//     vec4 directionToCenter = faceNormal - vec4(v_position[i], 1.0);
//     gl_Position = projection * modelview * ( vec4(v_position[i], 1.0) + alpha * directionToCenter ) ;
//     v_tex_coord = v_tex_coords[i];
//     EmitVertex();
//     }
//   
//   EndPrimitive();

 //DILATATION/EROSION
  for (int i = 0; i < 3; i++)
    {
    gl_Position = projection * modelview * ( vec4(v_position[i], 1.0) - alpha * vec4(v_normal[i],0.0) ) ;
    v_tex_coord = v_tex_coords[i];
    EmitVertex();
    }
  
  EndPrimitive();

}
