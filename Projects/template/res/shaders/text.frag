#version 460 core
out vec4 color;


in vec2 TexCoords;
in flat int index;



uniform sampler2DArray text;
uniform int letterMap[200];
uniform vec3 textColor;

void main()
{    

    float alpha = texture(text, vec3(TexCoords.xy, letterMap[index])).r;
  //  vec4 sampled = vec4(1.0, 1.0, 1.0, );
    color = vec4(textColor, alpha);
}  