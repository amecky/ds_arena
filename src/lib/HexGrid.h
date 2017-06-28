#pragma once
#include <diesel.h>
#include <cmath>
#include "DataArray.h"

struct Hex {

	int q;
	int r;
	int s;

	Hex() : q(0), r(0) {}
	Hex(int _q, int _r) : q(_q), r(_r) {
		s = -q - r;
	}
	Hex(int _q, int _r, int _s) : q(_q), r(_r), s(_s) {
	}

};

struct FractionalHex {

	float q, r, s;

	FractionalHex(float q_, float r_, float s_)
		: q(q_), r(r_), s(s_) {}
};

struct Orientation {
	float f0, f1, f2, f3;
	float b0, b1, b2, b3;
	float start_angle; // in multiples of 60�
	Orientation(float f0_, float f1_, float f2_, float f3_,
		float b0_, float b1_, float b2_, float b3_,
		float start_angle_)
		: f0(f0_), f1(f1_), f2(f2_), f3(f3_),
		b0(b0_), b1(b1_), b2(b2_), b3(b3_),
		start_angle(start_angle_) {}
};

const Orientation layout_pointy = Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0, sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0, 0.5);
const Orientation layout_flat = Orientation(3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0), 2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0, 0.0);

struct Layout {
	Orientation orientation;
	ds::vec2 size;
	ds::vec2 origin;
	Layout(Orientation orientation_, ds::vec2 size_, ds::vec2 origin_)
		: orientation(orientation_), size(size_), origin(origin_) {}
};

bool operator == (Hex a, Hex b);

bool operator != (Hex a, Hex b);

namespace hex_math {

	ds::vec2 to_pixel(const Hex& hex, float size);

	ds::vec2 hex_to_pixel(const Layout& layout, const Hex& h);

	FractionalHex pixel_to_hex(const Layout& layout, const ds::vec2& p);

	Hex hex_round(const FractionalHex& h);

	Hex neighbor(const Hex& hex, int direction);
}



enum GridItemState {
	GIS_CLOSED,
	GIS_OPEN,
	GIS_MARKED
};

struct GridItem {

	Hex hex;
	ds::vec2 position;
	float timer;
	ds::Color color;
	ds::Color highlightColor;
};

class HexGrid {

public:
	HexGrid();
	~HexGrid();
	void resize(int qMax, int rMax);
	void fill();
	void tick(float dt);
	const GridItem& get(int index) const;
	GridItem& get(int index);
	const GridItem& get(const Hex& hex) const;
	GridItem& get(const Hex& hex);
	const int size() const;
	int select(int x, int y);
	bool isValid(int q, int r) const;
	bool isValid(const Hex& hex) const;
	int neighbors(const Hex& hex,Hex* ret);
	Hex convertFromMousePos();
	Hex convert(const ds::vec2& p);
	ds::vec2 convert(const Hex& h);
	void highlight(const ds::vec2& p, const ds::Color& clr);
	void setOrigin(const ds::vec2& origin);
	int getIndex(const Hex& h)const;
	ds::vec2 convert(int q, int r) const;
	void setBaseColor(const ds::Color& bc) {
		_baseColor = bc;
	}
private:
	int _qMax;
	int _rMax;
	int _total;
	GridItem* _items;
	Layout _layout;
	int _hover;
	ds::Color _baseColor;
};