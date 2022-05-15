#include <metal_stdlib>
using namespace metal;

struct v2f
{
	float4 position [[position]];
	float2 tex_coord;
};

struct VertexData
{
	float2 position;
	float2 tex_coord;
};

struct InstanceData
{
	float3x3 instanceTransform;
};

v2f vertex rayVertex(device const VertexData* vertexData [[buffer(0)]],
		device const InstanceData* instanceData [[buffer(1)]],
		uint vertexId [[vertex_id]],
		uint instanceId [[instance_id]])
{
	v2f o;
	float3 initial_pos = float3(vertexData[vertexId].position, 1.0);
	float3 pos = instanceData[instanceId].instanceTransform * initial_pos;
	o.position = pos.xyzz;
	o.tex_coord = vertexData[vertexId].tex_coord;
	return o;
}

half4 fragment rayFragment(v2f in [[stage_in]], texture2d<half, access::sample> tex [[texture(0)]])
{
	constexpr sampler s(address::repeat, filter::linear);
	half3 texel = tex.sample(s, in.tex_coord).rgb;
	return half4(texel, 1.0);
}

