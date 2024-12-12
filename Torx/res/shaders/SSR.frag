#version 460 core

uniform sampler2D gViewPosition;
uniform sampler2D gViewNormal;
uniform sampler2D gRoughness;
uniform sampler2D gFinalImage;

//uniform mat4 invView;
in mat4 Projection;
//uniform mat4 invProjection;
//uniform mat4 view;

in vec2 TexCoords;

out vec4 FragColor;

float maxDistance = 30  ;
float resolution  = 1.0;
int   steps       = 30;
float thickness   = 0.4;

void main()
{

  vec2 texSize  = textureSize(gViewPosition, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec4 uv = vec4(0.0);

  vec4 positionFrom = texture(gViewPosition, texCoord);

  vec3 unitPositionFrom = normalize(positionFrom.xyz);
  vec3 normal           = normalize(texture(gViewNormal, texCoord).xyz);
  vec3 pivot            = normalize(reflect(unitPositionFrom, normal));

  vec4 positionTo = positionFrom;

  vec4 startView = vec4(positionFrom.xyz + (pivot *         0.0), 1.0);
  vec4 endView   = vec4(positionFrom.xyz + (pivot * maxDistance), 1.0);

  vec4 startFrag      = startView;
       startFrag      = Projection * startView;
       startFrag.xyz /= startFrag.w;
       startFrag.xy   = startFrag.xy * 0.5 + 0.5;
       startFrag.xy  *= texSize;

  vec4 endFrag      = endView;
       endFrag      = Projection * endView;
       endFrag.xyz /= endFrag.w;
       endFrag.xy   = endFrag.xy * 0.5 + 0.5;
       endFrag.xy  *= texSize;



  vec2 frag  = startFrag.xy;
       uv.xy = frag / texSize;

  float deltaX    = endFrag.x - startFrag.x;
  float deltaY    = endFrag.y - startFrag.y;
  float useX      = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
  float delta     = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0.0, 1.0);
  vec2  increment = vec2(deltaX, deltaY) / max(delta, 0.001);

  float search0 = 0;
  float search1 = 0;

  int hit0 = 0;
  int hit1 = 0;

  float viewDistance = startView.y;
  float depth        = thickness;

  float i = 0;

  for (i = 0; i < int(delta); ++i) {
    
    if(i > 240)
    {
        break;
    }

    frag      += increment;
    uv.xy      = frag / texSize;
    positionTo = texture(gViewPosition, uv.xy);

    search1 =
      mix
        ( (frag.y - startFrag.y) / deltaY
        , (frag.x - startFrag.x) / deltaX
        , useX
        );

    search1 = clamp(search1, 0.0, 1.0);

    viewDistance = (startView.z * endView.z) / mix(endView.z, startView.z, search1);
    depth        = (viewDistance) - positionTo.z;

    if (depth < 0 && depth > -thickness) 
    {
      hit0 = 1;
      break;
    } 
    else 
    {
      search0 = search1;
    }
  }

  search1 = search0 + ((search1 - search0) / 2.0);

  steps *= hit0;

  for (i = 0; i < steps; ++i) 
  {
    frag       = mix(startFrag.xy, endFrag.xy, search1);
    uv.xy      = frag / texSize;
    positionTo = texture(gViewPosition, uv.xy);

    viewDistance = (startView.z * endView.z) / mix(endView.z, startView.z, search1);
    depth        = (viewDistance) - positionTo.z;

    if (depth < 0 && depth > -thickness ) {
      hit1 = 1;
      search1 = search0 + ((search1 - search0) / 2);
    
    } 
    else 
    {
      float temp = search1;
      search1 = search1 + ((search1 - search0) / 2);
      search0 = temp;
    }
  }

  float visibility =
      hit1
    * positionTo.w
    * ( 1
      - max
         ( dot(-unitPositionFrom, pivot)
         , 0
         )
      )
    * ( 1
      - clamp
          ( depth / thickness
          , 0
          , 1
          )
      )
    * ( 1
      - clamp
          (   length(positionTo - positionFrom)
            / maxDistance
          , 0
          , 1
          )
      )
    * (uv.x < 0 || uv.x > 1 ? 0 : 1)
    * (uv.y < 0 || uv.y > 1 ? 0 : 1);

  visibility = clamp(visibility, 0, 1);

  uv.ba = vec2(visibility);

  float alpha = clamp(uv.b, 0, 1);

  vec4 color = texture(gFinalImage, uv.xy);

  float roughness = texture(gRoughness, texCoord).r;

  FragColor = vec4(mix(vec3(0.0), color.rgb , alpha) * (1 - pow(roughness, 0.5)), alpha);
  
  //vec4(mix(vec3(0.0), color.rgb, alpha), alpha);;

}
