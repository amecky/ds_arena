#pragma once
#include "..\..\..\diesel\diesel.h"

typedef uint8_t BYTE;

struct CubeConstantBuffer {
	ds::matrix viewprojectionMatrix;
	ds::matrix worldMatrix;
};

struct GridVertex {
	ds::vec3 position;
	ds::vec2 texture;
	ds::Color color;
};

struct LinePoint {
	float height;
	float targetHeight;
	float speed;
	ds::vec2 pos;
	ds::Color color;
};

struct Ribbon {
	int num;
	LinePoint* points;
	float* lDeltas;
	float* rDeltas;
	bool vertical;
};

struct ElasticBorderSettings {
	float Tension;
	float Dampening;
	float Spread;
	int numX;
	int numY;
	float thickness;
	ds::vec4 verticalTexture;
	ds::vec4 horizontalTexture;
	RID textureID;
	float targetHeight;
	float splashForce;
	float length;
};
// ---------------------------------------------------
// AbstractBorder
// ---------------------------------------------------
class ElasticBorder {

public:
	ElasticBorder(ElasticBorderSettings* settings);
	virtual ~ElasticBorder();
	void tick(float dt);
	void render();
	bool collides(const ds::vec2& s, float r);
private:
	void splash(int ribbonIndex, int index, float speed);
	RID createStateGroup(int numVertices, RID textureID);
	ElasticBorderSettings* _settings;
	//float _length;
	//float _thickness;
	//ds::vec4 _texture;
	//RID _textureID;	
	CubeConstantBuffer _constantBuffer;
	RID _vertexBufferID;
	RID _drawItem;
	RID _orthoPass;
	GridVertex* _vertices;
	float _timer;
	Ribbon _ribbons[4];
	ds::Camera _orthoCamera;
};
