#version 330

uniform sampler2D tex;

in vec2 v_tex_coord;

out vec4 color;
void main(void)
{
  color = texture(tex, v_tex_coord);
}

