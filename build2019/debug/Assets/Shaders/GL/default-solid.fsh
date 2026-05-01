
#ifdef GL_ES
	#if GLES_Ver<300
		#define in
		#define out
	#endif
	precision mediump float;
#endif

#if __VERSION__>=130
	out vec4 dgl_FragColor;
#else
	#define dgl_FragColor gl_FragColor
#endif

in vec4 vecColor;
in float dist;
uniform vec4 currentColor;
uniform bool isRandomColoring;

void main()
{
	if(isRandomColoring)
	{
		dgl_FragColor=vecColor+currentColor;
		dgl_FragColor.xyz-=dist/2.0;
	}
	else
	{
		dgl_FragColor=currentColor;
	}
}
