$input v_pos, v_viewdir, v_color0

#include <bgfx_shader.sh>

void main()
{
    // output in gamma space
    gl_FragColor.rgb = pow( v_color0.rgb,  (vec3) 1.0f/2.2f );
    gl_FragColor.a = v_color0.a;
}
