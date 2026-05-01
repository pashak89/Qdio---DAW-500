
#ifdef GL_ES
	#if GLES_Ver<300
		#define in attribute
		#ifdef WEBGL
			#define out varying
		#else
			#define out attribute
		#endif
	#endif
	precision mediump float;
#endif

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
uniform mat4 model;

/// projection * view * model
uniform mat4 mvp;

out vec4 vecColor;
out float dist;
void main()
{
    dist = normalize(model * vec4(aPos, 1.0)).z;
    vecColor=aColor;
    gl_Position = mvp * vec4(aPos, 1.0);
}
