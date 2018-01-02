cbuffer GridBuffer : register(b0) {
	float4 screenCenter;
	matrix wvp;
};

struct VS_Input {
	float3 pos  : POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR0;
};

struct PS_Input {
    float4 pos  : SV_POSITION;
    float2 tex : TEXCOORD0;
	float4 color : COLOR0;
};

PS_Input VS_Main(VS_Input vertex) {
	PS_Input vsOut = (PS_Input)0;
	float4 wp = float4(vertex.pos,1.0);
	wp.z = 0.0;
	wp.x -= screenCenter.x;
	wp.y -= screenCenter.y;
	vsOut.pos = mul(wp, wvp);
    vsOut.tex = vertex.tex;
	vsOut.color = vertex.color;
	return vsOut;
}

Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

float4 PS_Main(PS_Input frag) : SV_TARGET{
	return colorMap.Sample(colorSampler, frag.tex) * frag.color;
}
