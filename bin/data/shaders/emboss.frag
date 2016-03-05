/*

#define PI 3.141592653589793

#define KERNEL_SIZE 9

// Emboss kernel
// +2  0  0
//  0 -1  0
//  0  0 -1
float kernel[KERNEL_SIZE];

uniform sampler2DRect src_tex_unit0;
uniform vec2 src_tex_offset0;

vec2 offset[KERNEL_SIZE];

void main(void)
{
    int i = 0;
    vec4 sum = vec4(0.0);
    
    offset[0] = vec2(-src_tex_offset0.s, -src_tex_offset0.t);
    offset[1] = vec2(0.0, -src_tex_offset0.t);
    offset[2] = vec2(src_tex_offset0.s, -src_tex_offset0.t);
    
    offset[3] = vec2(-src_tex_offset0.s, 0.0);
    offset[4] = vec2(0.0, 0.0);
    offset[5] = vec2(src_tex_offset0.s, 0.0);
    
    offset[6] = vec2(-src_tex_offset0.s, src_tex_offset0.t);
    offset[7] = vec2(0.0, src_tex_offset0.t);
    offset[8] = vec2(src_tex_offset0.s, src_tex_offset0.t);
    
    kernel[0] = -8.0;   kernel[1] =  -8.0;   kernel[2] =  0.0;
    kernel[3] = -8.0;   kernel[4] = 24.0;   kernel[5] =  0.0;
    kernel[6] = 0.0;   kernel[7] =  0.0;   kernel[8] = 0.0;
    
    for(i = 0; i < KERNEL_SIZE; i++)
    {
        vec4 tmp = texture2DRect(src_tex_unit0, gl_TexCoord[0].st + offset[i]);
        sum += tmp * kernel[i];
    }
    
    gl_FragColor = vec4(sum.rgb - vec3(0.5, 0.5, 0.5), 1.0);
}

*/

uniform sampler2DRect tex;

const vec3 kernel0 = vec3(-10.0, -10.0, 0.0);
const vec3 kernel1 = vec3(-10.0, 0.0, 10.0);
const vec3 kernel2 = vec3(0.0, 10.0, 10.0);
const float bias = 0.5;

void main()
{
	vec3 sum = vec3(0.);
    
	sum += texture2DRect(tex, gl_TexCoord[0].xy + vec2(-1, -1)).rgb * kernel0[0];
	sum += texture2DRect(tex, gl_TexCoord[0].xy + vec2(0, -1)).rgb * kernel0[1];
	sum += texture2DRect(tex, gl_TexCoord[0].xy + vec2(1, -1)).rgb * kernel0[2];
    
	sum += texture2DRect(tex, gl_TexCoord[0].xy + vec2(-1, 0)).rgb * kernel1[0];
	sum += texture2DRect(tex, gl_TexCoord[0].xy).rgb * kernel1[1];
	sum += texture2DRect(tex, gl_TexCoord[0].xy + vec2(1, 0)).rgb * kernel1[2];
    
	sum += texture2DRect(tex, gl_TexCoord[0].xy + vec2(-1, 1)).rgb * kernel2[0];
	sum += texture2DRect(tex, gl_TexCoord[0].xy + vec2(0, 1)).rgb * kernel2[1];
	sum += texture2DRect(tex, gl_TexCoord[0].xy + vec2(1, 1)).rgb * kernel2[2];
    
	float alpha = texture2DRect(tex, gl_TexCoord[0].xy).a;
    
	gl_FragColor = vec4(sum + bias, alpha);
}
