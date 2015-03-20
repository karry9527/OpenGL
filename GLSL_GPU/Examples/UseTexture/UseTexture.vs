varying vec4 Color;
uniform sampler2D heightTexture;

void main()
{	
	vec4 a = gl_Vertex;
	//a.x = a.x*0.5;
	//gl_Position = gl_ModelViewProjectionMatrix * a;
	gl_TexCoord[0].xy = gl_MultiTexCoord0.xy ;

	
	vec3 hei_tex = texture2D(heightTexture, gl_TexCoord[0].xy).rbg;
	hei_tex *= 2;

	//a = a + texture2D(heightTexture, gl_TexCoord[0].xy) * vec4(gl_Normal, 0) ;
	a = a + vec4 (hei_tex  * gl_Normal, 0) ;
	//a = a + texture2D(heightTexture, gl_TexCoord[0].xy);
	gl_Vertex = a;
	gl_Position = ftransform();	

	/*vec4 t = gl_Vertex;
	t.y = gl_Vertex.y + 0.4*sin(0.1*time);
	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * t;
	
	vec4 color = vec4((sin(0.25*time)+1)*0.5, 1.0, 0.0, 1.0);
	gl_FrontColor = color;*/
 
}