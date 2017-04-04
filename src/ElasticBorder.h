#pragma once
#include "..\..\diesel\diesel.h"

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

// ---------------------------------------------------
// AbstractBorder
// ---------------------------------------------------
class ElasticBorder {

public:
	ElasticBorder(float length, float thickness, int numX, int numY, const ds::vec4& texture, RID textureID);// , const ds::AABBox& box);
	virtual ~ElasticBorder();
	void tick(float dt);
	void render();
	void splash(int index, float speed);
	bool collides(const ds::vec2& s, float r);
private:
	void splash(int ribbonIndex, int index, float speed);
	RID createStateGroup(int numVertices, RID textureID);
	float _length;
	float _thickness;
	ds::vec4 _texture;
	RID _textureID;	
	CubeConstantBuffer _constantBuffer;
	RID _vertexBufferID;
	RID _drawItem;
	RID _orthoPass;
	GridVertex* _vertices;
	float _timer;
	Ribbon _ribbons[4];
};
/*
// ---------------------------------------------------
// BottomBorder
// ---------------------------------------------------
class BottomBorder : public AbstractBorder {

public:
	BottomBorder(float target, int num);
	virtual ~BottomBorder() {}
	void buildColumn(int index, v3* positions);
	int getIndex(const v3& pos) const;
};

// ---------------------------------------------------
// TopBorder
// ---------------------------------------------------
class TopBorder : public AbstractBorder {

public:
	TopBorder(float target, int num);
	virtual ~TopBorder() {}
	void buildColumn(int index, v3* positions);
	int getIndex(const v3& pos) const;
};

// ---------------------------------------------------
// LeftBorder
// ---------------------------------------------------
class LeftBorder : public AbstractBorder {

public:
	LeftBorder(float target, int num);
	virtual ~LeftBorder() {}
	void buildColumn(int index, v3* positions);
	int getIndex(const v3& pos) const;
};

// ---------------------------------------------------
// RightBorder
// ---------------------------------------------------
class RightBorder : public AbstractBorder {

public:
	RightBorder(float target, int num);
	virtual ~RightBorder() {}
	void buildColumn(int index, v3* positions);
	int getIndex(const v3& pos) const;
};

// ---------------------------------------------------
// ElasticBorder
// ---------------------------------------------------
class ElasticBorder {

public:
	ElasticBorder();
	~ElasticBorder();
	void tick(float dt);
	void render();
	void splash(const v3& pos, float speed);
	void splash(const v3& pos, float radius, float speed);
private:
	AbstractBorder* _borders[4];
};
*/