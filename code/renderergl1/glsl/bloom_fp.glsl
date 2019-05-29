// bloom colour thresholding

uniform sampler2D	u_TextureMap;
uniform float		u_BloomAlpha;
uniform float		u_BloomRamp;
varying vec2		var_TexCoords;

#define LUMAVEC	vec3(0.2126, 0.7152, 0.0722)

void main()
{
	vec4 clr = texture2D(u_TextureMap, var_TexCoords);
	float luma = dot(clr.rgb, LUMAVEC);
	float a = pow(luma, u_BloomRamp)/u_BloomRamp * u_BloomRamp;
	gl_FragColor = vec4(clr.rgb * a * u_BloomAlpha, 1.0);
}
