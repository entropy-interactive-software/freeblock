#version 330 core
layout(location = 0) out vec4 f_color;
layout(location = 1) out vec4 f_bloom;

in vec3 v_fcolor;
in vec3 v_fnormal;
in vec3 v_fmpos;
flat in int v_fsurface;

struct light {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 position;
};

uniform sampler2D outlet;

uniform float shininess = 0.0;
uniform vec3 view_position;

void main() {
  vec3 i = normalize(v_fmpos - view_position);
  vec3 r = reflect(i, normalize(v_fnormal));

  float intensity = dot(v_fnormal, normalize(vec3(-0.5, 0.5, 0.5)));
  vec3 result = v_fcolor + vec3(0.5, 0.5, 0.5) * intensity;
  float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));

  vec4 colors[6];
  colors[0] = vec4(0.0, 1.0, 1.0, 1.0);
  colors[1] = vec4(1.0, 0.0, 0.0, 1.0);
  colors[2] = vec4(0.0, 1.0, 0.0, 1.0);
  colors[3] = vec4(0.0, 0.0, 1.0, 1.0);
  colors[4] = vec4(1.0, 0.0, 1.0, 1.0);
  colors[5] = vec4(1.0, 1.0, 0.0, 1.0);
  f_color = colors[v_fsurface];

  // f_color = vec4(result, 1.0);
  f_bloom = vec4(f_color.rgb * brightness, 1.0);
}
