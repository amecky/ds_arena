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
	vsOut.color = vertex.color;
    return vsOut;
}

float4 PS_Main( PS_Input frag ) : SV_TARGET {
	float2 t = frag.texcoord;
	float w = 0.02;
	float dw = 4.0 * w;
	if ((t.x < dw || t.x >(1 - dw)) && (t.y < dw || t.y >(1.0 - dw))) {
		return frag.color * 0.8;
	}
	if (t.x < w || t.x > (1 - w) || t.y < w || t.y > (1.0 - w)) {
		return frag.color;
	}
	else {
		return float4(0, 0, 0, 0);
	}
}

