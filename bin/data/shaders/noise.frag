
/*
uniform float time;
uniform float lac;
uniform float gai;
uniform int oct;
uniform vec2 res;

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
    return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
{
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626,  // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0
    // First corner
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    
    // Other corners
    vec2 i1;
    //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
    //i1.y = 1.0 - i1.x;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    // x0 = x0 - 0.0 + 0.0 * C.xx ;
    // x1 = x0 - i1 + 1.0 * C.xx ;
    // x2 = x0 - 1.0 + 2.0 * C.xx ;
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    
    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
                     + i.x + vec3(0.0, i1.x, 1.0 ));
    
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    
    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)
    
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    
    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    
    // Compute final noise value at P
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}






float fbm(vec2 P, int octaves, float lacunarity, float gain)
{
    float sum = 0.0;
    float amp = 1.5;
    vec2 pp = P;
    
    int i;
    
    for(i = 0; i < octaves; i+=1)
    {
        amp *= gain; 
        sum += amp * snoise(pp);
        pp += lacunarity;
    }
    return sum;
    
}


float pattern(in vec2 p) {
    float l = lac;
    float g = gai;
    int oc = 1;
    
    vec2 q = vec2( fbm( p + vec2(0.0,0.0),oc,l,g),fbm( p + vec2(5.2,1.3),oc,l,g));
    vec2 r = vec2( fbm( p + 4.0*q + vec2(1.7,9.2),oc,l,g ), fbm( p + 4.0*q + vec2(8.3,2.8) ,oc,l,g));
    return fbm( p + 4.0*r ,oc,l,g);    
}

float pattern2( in vec2 p, out vec2 q, out vec2 r , in float time)
{
    float l = lac;
    float g = gai;
    int oc = oct; 
    
    q.x = fbm( p + vec2(time,time),oc,l,g);
    q.y = fbm( p + vec2(3.2*time,1.3*time) ,oc,l,g);
    
    r.x = fbm( p + 4.0*q + vec2(1.7,1.2),oc,l,g );
    r.y = fbm( p + 4.0*q + vec2(1.3,2.8) ,oc,l,g);
    
    return fbm( p + 4.0*r ,oc,l,g);
}

float pattern3( in vec2 p, out vec2 q, out vec2 r , in float time)
{
    float l = lac;
    float g = gai;
    int oc = oct; 
    
    q.x = fbm( p + vec2(time,time),oc,l,g);
    q.y = fbm( p + vec2(3.2*time,1.3*time) ,oc,l,g);
    
    r.x = fbm( p + 4.0*q + vec2(2.7,5.2),oc,l,g );
    r.y = fbm( p + 4.0*q + vec2(5.3,4.8) ,oc,l,g);
    
    return fbm( p + 4.0*r ,oc,l,g);
}

float pattern4( in vec2 p, out vec2 q, out vec2 r , in float time)
{
    float l = lac;
    float g = gai;
    int oc = oct; 
    
    q.x = fbm( p + vec2(time,time),oc,l,g);
    q.y = fbm( p + vec2(3.2*time,1.3*time) ,oc,l,g);
    
    r.x = fbm( p + 4.0*q + vec2(10.7,15.2),oc,l,g );
    r.y = fbm( p + 4.0*q + vec2(10.3,20.8) ,oc,l,g);
    
    return fbm( p + 4.0*r ,oc,l,g);
}

void main() {
    
    vec2 q = gl_FragCoord.xy / res;
    vec2 p = -1.0 + 2.0 * q;
    vec2 qq;
    vec2 r;
    float color = pattern2(p,qq,r,time);
    
    float color2 = pattern3(p,qq,r,time);
    
    float color3 = pattern4(p,qq,r,time);
    
    vec4 c = vec4(color3, color, color2,(color+color3+color2)/3.0 );
    c *= 8.0;
    
    gl_FragColor = c;
}
*/

/*
uniform float time;
uniform vec2 resolution;

//
// GLSL textureless classic 3D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author:  Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-10-11
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/ashima/webgl-noise
//

vec3 mod289(vec3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
    return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
    return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P)
{
    vec3 Pi0 = floor(P); // Integer part for indexing
    vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
    Pi0 = mod289(Pi0);
    Pi1 = mod289(Pi1);
    vec3 Pf0 = fract(P); // Fractional part for interpolation
    vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
    vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    vec4 iy = vec4(Pi0.yy, Pi1.yy);
    vec4 iz0 = Pi0.zzzz;
    vec4 iz1 = Pi1.zzzz;
    
    vec4 ixy = permute(permute(ix) + iy);
    vec4 ixy0 = permute(ixy + iz0);
    vec4 ixy1 = permute(ixy + iz1);
    
    vec4 gx0 = ixy0 * (1.0 / 7.0);
    vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
    gx0 = fract(gx0);
    vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
    vec4 sz0 = step(gz0, vec4(0.0));
    gx0 -= sz0 * (step(0.0, gx0) - 0.5);
    gy0 -= sz0 * (step(0.0, gy0) - 0.5);
    
    vec4 gx1 = ixy1 * (1.0 / 7.0);
    vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
    gx1 = fract(gx1);
    vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
    vec4 sz1 = step(gz1, vec4(0.0));
    gx1 -= sz1 * (step(0.0, gx1) - 0.5);
    gy1 -= sz1 * (step(0.0, gy1) - 0.5);
    
    vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
    vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
    vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
    vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
    vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
    vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
    vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
    vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);
    
    vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
    g000 *= norm0.x;
    g010 *= norm0.y;
    g100 *= norm0.z;
    g110 *= norm0.w;
    vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
    g001 *= norm1.x;
    g011 *= norm1.y;
    g101 *= norm1.z;
    g111 *= norm1.w;
    
    float n000 = dot(g000, Pf0);
    float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
    float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
    float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
    float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
    float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
    float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
    float n111 = dot(g111, Pf1);
    
    vec3 fade_xyz = fade(Pf0);
    vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
    vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
    float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
    return 2.2 * n_xyz;
}

float surface3 ( vec3 coord ) {
	
	float frequency = 1.25;
	float n = 0.0;	
    
	n += 1.0	* abs( cnoise( coord * frequency ) );
	n += 0.5	* abs( cnoise( coord * frequency * 2.0 ) );
    //	n += 0.25	* abs( cnoise( coord * frequency * 4.0 ) );
	
	return n;
}

void main( void ) {
	
	vec2 position = vec2(gl_FragCoord.x / resolution.x * 8.0, gl_FragCoord.y / resolution.y * 0.25);
	
	float n = surface3(vec3(position, time * 0.1));
    
	gl_FragColor = vec4(n, n, n, 1.0);
}
*/

uniform float time;
uniform vec2 resolution;

// An attempt at Quilez's warping (domain distortions):
// http://iquilezles.org/www/articles/warp/warp.htm
// 
// Not as good as his, but still interesting.
// @SyntopiaDK, 2012



float rand(vec2 co){
	// implementation found at: lumina.sourceforge.net/Tutorials/Noise.html
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise2f( in vec2 p )
{
	vec2 ip = vec2(floor(p));
	vec2 u = fract(p);
	// http://www.iquilezles.org/www/articles/morenoise/morenoise.htm
	u = u*u*(3.0-2.0*u);
	//u = u*u*u*((6.0*u-15.0)*u+10.0);
	
	float res = mix(
                    mix(rand(ip),  rand(ip+vec2(1.0,0.0)),u.x),
                    mix(rand(ip+vec2(0.0,1.0)),   rand(ip+vec2(1.0,1.0)),u.x),
                    u.y)
	;
	return res*res;
	//return 2.0* (res-0.5);
}

float fbm(vec2 c) {
	float f = 0.0;
	float w = 1.0;
	for (int i = 0; i < 8; i++) {
		f+= w*noise2f(c);
		c*=2.0;
		w*=0.5;
	}
	return f;
}


vec2 cMul(vec2 a, vec2 b) {
	return vec2( a.x*b.x -  a.y*b.y,a.x*b.y + a.y * b.x);
}

float pattern(  vec2 p, out vec2 q, out vec2 r )
{
	q.x = fbm( p  +0.00*time);
	q.y = fbm( p + vec2(1.0));
	
	r.x = fbm( p +1.0*q + vec2(1.7,9.2)+0.15*time );
	r.y = fbm( p+ 1.0*q + vec2(8.3,2.8)+0.126*time);
	//r = cMul(q,q+0.1*time);
	return fbm(p +1.0*r + 0.0* time);
}

const vec3 color1 = vec3(0.101961,0.619608,0.666667);
const vec3 color2 = vec3(0.666667,0.666667,0.498039);
const vec3 color3 = vec3(0,0,0.164706);
const vec3 color4 = vec3(0.666667,1,1);
void main() {
	vec2 q;
	vec2 r;
	vec2 c = 100.0*vec2(gl_FragCoord.x/resolution.x*8.0,gl_FragCoord.y/resolution.y*0.55);
	float f = pattern(c*0.01,q,r);
	vec3 col = mix(color1,color2,clamp((f*f)*4.0,0.0,1.0));
	col = color2;
	col = mix(col,color3,clamp(length(q),0.0,1.0));
	col = mix(col,color4,clamp(length(r.x),0.0,1.0));
	gl_FragColor =  vec4((0.2*f*f*f+0.6*f*f+0.5*f)*col,1.0);
}



