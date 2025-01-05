#version 460 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform vec3 outlineColor;

uniform float thickness;
uniform float softness;
uniform float outlineThickness;
uniform float outlineSoftness; 

void main()
{    

    float a = texture(text, TexCoords).r;

   
    float outline = smoothstep(outlineThickness - outlineSoftness, outlineThickness + outlineSoftness, a);
    a = smoothstep(1.0 - thickness - softness, 1.0 - thickness + softness , a);
    
    color = vec4(mix(outlineColor, textColor, outline), a);
}   