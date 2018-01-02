#include "HexGrid.h"

//const static ds::Color HEX_BASE_COLOR = ds::Color(12, 12, 12, 255);

bool operator == (Hex a, Hex b) {
	return a.q == b.q && a.r == b.r && a.s == b.s;
}

bool operator != (Hex a, Hex b) {
	return !(a == b);
}

const Hex HEX_DIRECTIONS[] = {
	Hex(1, 0), Hex(1, -1), Hex(0, -1), Hex(-1, 0), Hex(-1, 1), Hex(0, 1)
};

ds::Color lerp(const ds::Color& lhs, const ds::Color& rhs, float t) {
	if (t <= 0.0f) {
		return lhs;
	}
	if (t >= 1.0f) {
		return rhs;
	}
	float invT = 1.0f - t;
	return ds::Color(lhs.r * invT + rhs.r * t, lhs.g * invT + rhs.g * t, lhs.b * invT + rhs.b * t, lhs.a * invT + rhs.a * t);
}

namespace hex_math {

	ds::vec2 to_pixel(const Hex& hex, float size) {
		float x = size * 3.0f / 2.0f * hex.q;
		float y = size * sqrt(3.0f) * (hex.r + hex.q / 2.0f);
		return ds::vec2(x, y);
	}

	ds::vec2 hex_to_pixel(const Layout& layout, const Hex& h) {
		const Orientation& M = layout.orientation;
		float x = (M.f0 * h.q + M.f1 * h.r) * layout.size.x;
		float y = (M.f2 * h.q + M.f3 * h.r) * layout.size.y;
		return ds::vec2(x + layout.origin.x, y + layout.origin.y);
	}

	FractionalHex pixel_to_hex(const Layout& layout, const ds::vec2& p) {
		const Orientation& M = layout.orientation;
		ds::vec2 pt = ds::vec2((p.x - layout.origin.x) / layout.size.x, (p.y - layout.origin.y) / layout.size.y);
		float q = M.b0 * pt.x + M.b1 * pt.y;
		float r = M.b2 * pt.x + M.b3 * pt.y;
		return FractionalHex(q, r, -q - r);
	}

	Hex hex_round(const FractionalHex& h) {
		int q = int(round(h.q));
		int r = int(round(h.r));
		int s = int(round(h.s));
		double q_diff = abs(q - h.q);
		double r_diff = abs(r - h.r);
		double s_diff = abs(s - h.s);
		if (q_diff > r_diff && q_diff > s_diff) {
			q = -r - s;
		}
		else if (r_diff > s_diff) {
			r = -q - s;
		}
		else {
			s = -q - r;
		}
		return Hex(q, r, s);
	}

	Hex neighbor(const Hex& hex, int direction) {
		//assert(direction >= 0 && direction < 6);
		Hex dir = HEX_DIRECTIONS[direction];
		return Hex(hex.q + dir.q, hex.r + dir.r);
	}
}

HexGrid::HexGrid() : _qMax(0), _rMax(0), _items(0), _layout(layout_pointy, ds::vec2(24.0f, 24.0f), ds::vec2(0, 60)) {
}

HexGrid::~HexGrid() {
	if (_items != 0) {
		delete[] _items;
	}
}

// -------------------------------------------------------
// resize
// -------------------------------------------------------
void HexGrid::resize(int qMax, int rMax) {
	if (_items != 0) {
		delete[] _items;
	}
	_qMax = qMax;
	_rMax = rMax;
	_total = _qMax * _rMax;
	_items = new GridItem[_total];
	fill();
}

// -------------------------------------------------------
// fill
// -------------------------------------------------------
void HexGrid::fill() {
	//assert(_items != 0);
	for (int r = 0; r < _rMax; r++) {
		int q_offset = r >> 1;
		for (int q = -q_offset; q < _qMax - q_offset; q++) {
			GridItem item;
			item.hex = Hex(q, r);
			item.position = hex_math::hex_to_pixel(_layout, item.hex);
			item.timer = 0.0f;
			item.color = _baseColor;
			int idx = (q + q_offset) + r * _qMax;
			_items[idx] = item;
		}
	}
	_hover = -1;
}

void HexGrid::tick(float dt) {
	for (int i = 0; i < _total; ++i) {
		GridItem& item = _items[i];
		if (item.timer > 0.0f) {
			item.timer -= dt;
			float n = 1.0f - item.timer / 1.2f;
			item.color = lerp(item.highlightColor, _baseColor, n);
			if (n >= 1.0f) {
				item.timer = 0.0f;
			}
		}
		else {
			item.color = _baseColor;
		}
	}
}

const GridItem& HexGrid::get(const Hex& hex) const {
	int q_offset = hex.r >> 1;
	int idx = (hex.q + q_offset) + hex.r * _qMax;
	return _items[idx];
}

GridItem& HexGrid::get(const Hex& hex) {
	int q_offset = hex.r >> 1;
	int idx = (hex.q + q_offset) + hex.r * _qMax;
	return _items[idx];
}

int HexGrid::neighbors(const Hex& hex, Hex* ret) {
	int cnt = 0;
	for (int i = 0; i < 6; ++i) {
		Hex n = hex_math::neighbor(hex, i);
		if (isValid(n)) {
			ret[cnt++] = n;
		}
	}
	return cnt;
}

Hex HexGrid::convert(const ds::vec2& p) {
	return hex_math::hex_round(hex_math::pixel_to_hex(_layout, p));
}

void HexGrid::highlight(const ds::vec2& p, const ds::Color& clr) {
	Hex h = convert(p);
	GridItem& item = get(h);
	item.highlightColor = clr;
	item.timer = 1.2f;
}

ds::vec2 HexGrid::convert(const Hex & h) {
	return hex_math::hex_to_pixel(_layout, h);
}

Hex HexGrid::convertFromMousePos() {
	ds::vec2 mp = ds::getMousePosition();
	return hex_math::hex_round(hex_math::pixel_to_hex(_layout, mp));
}

ds::vec2 HexGrid::convert(int q, int r) const {
	Hex hex = Hex(q, r);
	return hex_math::hex_to_pixel(_layout, hex);
}
// -------------------------------------------------------
// get
// -------------------------------------------------------
const GridItem& HexGrid::get(int index) const {
	return _items[index];
}

// -------------------------------------------------------
// get
// -------------------------------------------------------
GridItem& HexGrid::get(int index) {
	return _items[index];
}
// -------------------------------------------------------
// size
// -------------------------------------------------------
const int HexGrid::size() const {
	return _rMax * _qMax;
}

// -------------------------------------------------------
// select
// -------------------------------------------------------
int HexGrid::select(int x, int y) {
	ds::vec2 mp = ds::getMousePosition();
	Hex h = hex_math::hex_round(hex_math::pixel_to_hex(_layout, mp));
	int q_offset = h.r >> 1;
	int selected = -1;
	//LOG << "h: " << h.q << " " << h.r << " q_offset: " << q_offset;
	if (!isValid(h)) {
		return -1;
	}
	for (size_t i = 0; i < size(); ++i) {
		if (_items[i].hex == h) {
			selected = i;

		}
	}
	return selected;
}

// -------------------------------------------------------
// is valid
// -------------------------------------------------------
bool HexGrid::isValid(int q, int r) const {
	int q_offset = r >> 1;
	if (r < 0 || r >= _rMax) {
		return false;
	}
	if (q_offset + q < 0 || q_offset + q >= _qMax) {
		return false;
	}
	return true;
}

// -------------------------------------------------------
// is valid
// -------------------------------------------------------
bool HexGrid::isValid(const Hex& hex) const {
	return isValid(hex.q, hex.r);
}

void HexGrid::setOrigin(const ds::vec2& origin) {
	_layout.origin = origin;
}

// -------------------------------------------------------
// get index
// -------------------------------------------------------
int HexGrid::getIndex(const Hex& hex) const {
	int q_offset = hex.r >> 1;
	return (hex.q + q_offset) + hex.r * _qMax;
}

