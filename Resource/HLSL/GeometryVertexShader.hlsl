#include "GeometryHeader.hlsli"

VSOutput main(float4 pos : POSITION)
{
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = pos;
	/*output.normal = normal;
	output.uv = uv;*/
	return output;
}