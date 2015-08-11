uniform sampler2D colorTexture;

void main (void)
{
	gl_FragColor = texture2D(colorTexture, gl_TexCoord[0].xy).rgba;	

}
/*void main() {
 
// Extract the normal from the normal map
vec3 normal = normalize(texture2D(normal_texture, gl_TexCoord[0].xy).rgb * 2.0 - 1.0);
 
// Determine where the light is positioned
vec3 light_pos = normalize(vec3(1.0, 1.0, 1.5));
 
// Calculate the lighting diffuse value
float diffuse = max(dot(normal, light_pos), 0.0);
 
vec3 color = diffuse * texture2D(colorTexture, gl_TexCoord[0].xy).rgb;
 
// Set the output color of our current pixel
gl_FragColor = vec4(color, 1.0);
}*/