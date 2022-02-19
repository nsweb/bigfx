$input a_position, i_data0, i_data1, i_data2, i_data3, i_data4
$output v_pos, v_viewdir, v_color0

#include <bgfx_shader.sh>

void main()
{
	mat4 model = mtxFromCols(i_data0, i_data1, i_data2, i_data3);
	mat4 model_view = mul(u_view, model);
	vec4 view_pos = mul(model_view, vec4(a_position, 1.0));
	gl_Position = mul(u_proj, view_pos);

	// Transform view vector back to local box
	vec3 view_dir_local = mul(view_pos, ((mat3)model_view));

	v_pos = a_position.xyz;
	v_viewdir = normalize( view_dir_local );
	v_color0 = i_data4;
}
