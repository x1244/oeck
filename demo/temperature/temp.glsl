#pragma vp_function oe_temp_vertex, vertex_view

out vec4 oe_layer_tilec;
out vec2 oe_GeodeticGraticule_coord;
flat out float zvalue;
vec4 oe_tile_key;
void oe_temp_vertex(inout vec4 not_used) 
{
    zvalue = oe_tile_key.z + 1.0;
    vec2 r = (oe_tile_key.xy + oe_layer_tilec.xy)/exp2(oe_tile_key.z);
    oe_GeodeticGraticule_coord = vec2(0.5*r.x, r.y);
}

[break]
#pragma vp_function oe_temp_fragment, fragment_output
uniform sampler1D oe_temp_xfer;
uniform sampler2D oe_temp_src;
uniform float oe_temp_min;
uniform float oe_temp_range;

// GL3 implementation:
in vec2 oe_GeodeticGraticule_coord;
flat in float zvalue;
out vec4 fragColor;

void oe_temp_fragment(inout vec4 color)
{
    //oe_GeodeticGraticule_coord.st [-0.5, 0.5]
    float temp = texture(oe_temp_src, oe_GeodeticGraticule_coord.st + 0.5).r;
    vec4 texel = texture(oe_temp_xfer, temp); //coord [0, 1]
	float alpha = 0.2;
	if(fract(100.0*temp) < 0.1)
	    alpha = 1.0;
	fragColor = vec4(texel.rgb, alpha);

}

