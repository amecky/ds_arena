cbuffer cbChangesPerObject : register( b0 ) {
    matrix mvp;
    matrix world;    
};

struct VS_Input {
    float4 position  : POSITION;
    float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

struct PS_Input {
    float4 pos  : SV_POSITION;
    float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};


PS_Input VS_Main( VS_Input vertex ) {
    PS_Input vsOut = ( PS_Input )0;
	vertex.position.w = 1.0;
	float4 center = float4(512, 364, 0, 1.0);
	float4 p = vertex.position - center;
	p.z = 1.0;
	p.w = 1.0;
    vsOut.pos = mul( p, world);
	vsOut.pos = mul(vsOut.pos, mvp);
    vsOut.texcoord = vertex.texcoord;
	vsOut.texcoord.x = vsOut.texcoord.x  / 1024.0;
	vsOut.texcoord.y = vsOut.texcoord.y  / 1024.0; 
	vsOut.color = vertex.color;
    return vsOut;
}

Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

float4 PS_Main( PS_Input frag ) : SV_TARGET {
	return colorMap.Sample(colorSampler, frag.texcoord) * frag.color;	
}

