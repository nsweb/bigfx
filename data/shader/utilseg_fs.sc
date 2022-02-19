$input v_color0

#include <bgfx_shader.sh>

void main()
{
    // output in gamma space
    gl_FragColor.rgb = v_color0.rgb;//pow( v_color0.rgb, (vec3) 1.0f/2.2f );
    gl_FragColor.a = 1.0;//v_color0.a;
}
