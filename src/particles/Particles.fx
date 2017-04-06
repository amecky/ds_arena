cbuffer cbChangesPerFrame : register(b0) {
	float4 screenDimension;
	float4 screenCenter;
	float4 startColor;
	float4 endColor;
	float4 scale;
	float4 tex;
	matrix wvp;
};

struct VS_Input {
	float3 pos  : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 color : COLOR;
};

struct GSPS_INPUT {
	float4 Pos : SV_POSITION;
	float4 Timer : COLOR;
};

struct PS_Input {
	float4 pos  : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color : COLOR0;
};

GSPS_INPUT VS_Main(VS_Input vertex) {
	GSPS_INPUT vsOut = (GSPS_INPUT)0;
	vsOut.Pos = float4(vertex.pos, 1.0) + float4(vertex.normal,0.0) * vertex.color.x + float4(vertex.tangent,0.0) * vertex.color.x * vertex.color.x;
	vsOut.Timer = vertex.color;
	float norm = vertex.color.x / vertex.color.y;
	vsOut.Timer.z = norm;
	return vsOut;
}

[maxvertexcount(4)]
void GS_Main(point GSPS_INPUT gin[1], inout TriangleStream<PS_Input> triStream)
{
	float VP_ARRAY[8] = { -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
	float2 t[4];
	float2 dim = tex.zw;
	float left = tex.x / screenDimension.z;
	float top = tex.y / screenDimension.w;
	float width = tex.z / screenDimension.z;
	float height = tex.w / screenDimension.w;
	float right = left + width;
	float bottom = top + height;
	t[0] = float2(left, top);
	t[1] = float2(right, top);
	t[2] = float2(left, bottom);
	t[3] = float2(right, bottom);
	float4 pos = gin[0].Pos;
	float2 Size = float2(1, 1);
	Size.x = lerp(scale.x, scale.z, gin[0].Timer.z);
	Size.y = lerp(scale.y, scale.w, gin[0].Timer.z);
	float4 color = lerp(startColor, endColor, gin[0].Timer.z);
	pos -= float4(screenCenter.x, screenCenter.y, 0.5, 1.0);
	PS_Input gout;
	triStream.RestartStrip();
	float rot = gin[0].Timer.w;
	float s = sin(rot);
	float c = cos(rot);
	[unroll]
	for (int i = 0; i < 4; ++i) {
		float px = VP_ARRAY[i * 2] * dim.x;
		float py = VP_ARRAY[i * 2 + 1] * dim.y;
		float sx = px * Size.x;
		float sy = py * Size.y;
		float xt = c * sx - s * sy;
		float yt = s * sx + c * sy;
		gout.pos = mul(float4(xt + pos.x, yt + pos.y, 0.0f, 1.0f), wvp);
		gout.pos.z = 1.0;
		gout.texcoord = t[i];
		gout.color = color;
		triStream.Append(gout);
	}
	triStream.RestartStrip();
}

Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

float4 PS_Main(PS_Input frag) : SV_TARGET {
	return colorMap.Sample(colorSampler, frag.texcoord) * frag.color;
	/*	
	float border_size = 0.2;
	float disc_radius = 0.3;
	float dx = 2 * frag.tex0.x - 1;
	float dy = 2 * frag.tex0.y - 1;
	float hyp = sqrt(dx * dx + dy * dy);
	float t = smoothstep(disc_radius + border_size, disc_radius - border_size, hyp);
	float4 outColor = frag.color;
	outColor.a = outColor.a * lerp(0.0, 1.0, t) * frag.tex0.x;
	return pow(outColor, 1.0f / 2.2f); // gamma correction
	*/
}
