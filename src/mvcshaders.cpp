#include "mvcshaders.h"

std::string getAlphaFS() {
	return "\
		   uniform sampler2DRect tex0;\
		   uniform sampler2DRect tex1;\
		   varying vec4 membrane;\
		   varying float alpha;\
		   uniform int mode;\
		   void main(){\
		   vec4 target = texture2DRect(tex0, gl_TexCoord[0].st);\
		   vec4 source = texture2DRect(tex1, gl_TexCoord[1].st);\
		   if (mode==1)\
		   gl_FragColor = source;\
		   else if(mode==2)\
		   gl_FragColor = membrane + 0.5;\
		   else\
		   gl_FragColor = (membrane+source)*alpha +target*(1.0-alpha);\
		   }\
		   ";
	}

std::string getAlphaVS() {
	return "\
		   uniform sampler2DRect tex0;\
		   uniform sampler2DRect tex1;\
		   uniform sampler2DRect tex2;\
		   uniform sampler2DRect tex3;\
		   varying vec4 membrane;\
		   uniform int boundarySize;\
		   uniform int blend;\
		   varying float alpha;\
		   void main() {\
		   gl_TexCoord[0] = gl_MultiTexCoord0;\
		   gl_TexCoord[1] = gl_MultiTexCoord1;\
		   gl_TexCoord[2] = gl_MultiTexCoord2;\
		   gl_TexCoord[3] = gl_MultiTexCoord3;\
		   \
		   float x, y,weight;\
		   vec4 source, target, err;\
		   vec4 xy;\
		   \
		   alpha = 0.0;\
		   \
		   membrane = vec4(0.0);\
		   for (int t=0; t<boundarySize; t++) {\
		   weight = texture2DRect(tex2,gl_TexCoord[2].st+vec2(t,0.0)).x;\
		   \
		   x = texture2DRect(tex3,vec2(t,0.0)).x;\
		   y = texture2DRect(tex3,vec2(t,1.0)).x;\
		   \
		   xy = gl_ModelViewMatrix * vec4(x,y,0,1);\
		   \
		   target = texture2DRect(tex0,vec2(xy.x,xy.y));\
		   source = texture2DRect(tex1,vec2(x,y));\
		   err = target-source;\
		   \
		   membrane += err*weight;\
		   \
		   if (weight>alpha) {\
		   alpha = weight;\
		   }		\
		   }\
		   \
		   if (blend==1)\
		   alpha = 1.0-min(1.0, alpha*2.0);\
		   else\
		   alpha = 1.0;\
		   \
		   gl_Position = ftransform();	\
		   \
		   }\
		   ";
	}

std::string getSampling2DFilterVS() {
	return "\
		   uniform sampler2DRect tex0;\
		   uniform sampler2DRect tex1;\
		   uniform sampler2DRect tex2;\
		   uniform sampler2DRect tex3;\
		   varying vec4 membrane;\
		   \
		   uniform float target_h;\
		   uniform float source_h;\
		   uniform int blend;\
		   \
		   void main() {\
		   \
		   float sourceMipMapOffs[6];\
		   float targetMipMapOffs[6];\
		   float scale[6];\
		   \
		   sourceMipMapOffs[0] = 0.0;\
		   targetMipMapOffs[0] = 0.0;\
		   scale[0] = 1.0;\
		   \
		   float cs = source_h;\
		   float ct = target_h;\
		   for (int i=1; i<=5; i++) {\
		   sourceMipMapOffs[i] = sourceMipMapOffs[i-1]+cs;\
		   targetMipMapOffs[i] = targetMipMapOffs[i-1]+ct;\
		   scale[i] = scale[i-1]/2.0;\
		   \
		   cs = ceil(cs/2.0);\
		   ct = ceil(ct/2.0);\
		   }\
		   \
		   gl_TexCoord[0] = gl_MultiTexCoord0; \
		   gl_TexCoord[1] = gl_MultiTexCoord1; \
		   gl_TexCoord[2] = gl_MultiTexCoord2;\
		   gl_TexCoord[3] = gl_MultiTexCoord3;\
		   \
		   float x, y, weight, index, depth;\
		   \
		   vec4 source, target, err;\
		   vec4 xy;\
		   int boundarySize = int(texture2DRect(tex2,gl_TexCoord[2].st).x);\
		   \
		   vec4 WID;\
		   float sc;\
		   float offset_target;\
		   float offset_source;\
		   int level;\
		   membrane = vec4(0.0);\
		   for (int t=1; t<boundarySize+1; t++) {\
		   WID = texture2DRect(tex2,gl_TexCoord[2].st+vec2(t,0.0));\
		   depth = WID.z;\
		   level = int(5.0-min(depth, 5.0));\
		   \
		   offset_source = sourceMipMapOffs[level];\
		   offset_target = targetMipMapOffs[level];\
		   sc = scale[level];\
		   \
		   x = texture2DRect(tex3,vec2(WID.y,0.0)).x;\
		   y = texture2DRect(tex3,vec2(WID.y,1.0)).x;\
		   \
		   xy = gl_ModelViewMatrix * vec4(x,y,0,1);\
		   \
		   target = texture2DRect(tex0,vec2(xy.x*sc,xy.y*sc + offset_target));\
		   source = texture2DRect(tex1,vec2(x*sc,y*sc + offset_source));\
		   err = target-source;\
		   \
		   membrane += err*WID.x;\
		   }\
		   \
		   gl_Position = ftransform();	\
		   } \
		   ";
	}

std::string getSamplingFS() {
	return "\
		   uniform sampler2DRect tex0;\
		   uniform sampler2DRect tex1;\
		   varying vec4 membrane;\
		   \
		   uniform int mode;\
		   \
		   void main(){\
		   \
		   vec4 target = texture2DRect(tex0, gl_TexCoord[0].st);\
		   vec4 source = texture2DRect(tex1, gl_TexCoord[1].st);\
		   \
		   if (mode==1)\
		   gl_FragColor = source;\
		   else if(mode==2)\
		   gl_FragColor = membrane + 0.5;\
		   else\
		   gl_FragColor = membrane+source;\
		   }\
		   ";
	}

std::string getSamplingVS() {
	return "\
		   uniform sampler2DRect tex0;\
		   uniform sampler2DRect tex1;\
		   uniform sampler2DRect tex2;\
		   uniform sampler2DRect tex3;\
		   varying vec4 membrane;\
		   \
		   void main() {\
		   gl_TexCoord[0] = gl_MultiTexCoord0; \
		   gl_TexCoord[1] = gl_MultiTexCoord1; \
		   gl_TexCoord[2] = gl_MultiTexCoord2;\
		   gl_TexCoord[3] = gl_MultiTexCoord3;\
		   float x, y, weight;\
		   vec4 source, target, err;\
		   vec4 xy;\
		   int boundarySize = int(texture2DRect(tex2,gl_TexCoord[2].st).x);\
		   vec4 WID;\
		   membrane = vec4(0.0);\
		   for (int t=1; t<boundarySize+1; t++) {\
		   \
		   WID = texture2DRect(tex2,gl_TexCoord[2].st+vec2(t,0.0));\
		   \
		   x = texture2DRect(tex3,vec2(WID.y,0.0)).x;\
		   y = texture2DRect(tex3,vec2(WID.y,1.0)).x;\
		   xy = gl_ModelViewMatrix * vec4(x,y,0,1);\
		   target = texture2DRect(tex0,vec2(xy.x,xy.y));\
		   source = texture2DRect(tex1,vec2(x,y));\
		   err = target-source;\
		   membrane += err*WID.x;\
		   }\
		   gl_Position = ftransform();	\
		   } \
		   ";
	}