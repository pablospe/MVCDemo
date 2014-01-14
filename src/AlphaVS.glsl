uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform sampler2DRect tex2;
uniform sampler2DRect tex3;
varying vec4 membrane;
uniform int boundarySize;
uniform int blend;
varying float alpha;

void main()
{
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[1] = gl_MultiTexCoord1;
  gl_TexCoord[2] = gl_MultiTexCoord2;
  gl_TexCoord[3] = gl_MultiTexCoord3;

  float x, y,weight;
  vec4 source, target, err;
  vec4 xy;

  alpha = 0.0;

  membrane = vec4(0.0);
  for (int t=0; t<boundarySize; t++)
  {
    weight = texture2DRect(tex2,gl_TexCoord[2].st+vec2(t,0.0)).x;

    x = texture2DRect(tex3,vec2(t,0.0)).x;
    y = texture2DRect(tex3,vec2(t,1.0)).x;

    xy = gl_ModelViewMatrix * vec4(x,y,0,1);

    target = texture2DRect(tex0,vec2(xy.x,xy.y));
    source = texture2DRect(tex1,vec2(x,y));
    err = target-source;

    membrane += err*weight;

    if (weight>alpha)
      alpha = weight;
  }

  if (blend==1)
    alpha = 1.0-min(1.0, alpha*2.0);
  else
    alpha = 1.0;

  gl_Position = ftransform();
}
