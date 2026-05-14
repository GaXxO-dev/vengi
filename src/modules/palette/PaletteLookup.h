/**
 * @file
 */

#pragma once

#include "color/Distance.h"
#include "color/RGBA.h"
#include "core/collection/Buffer.h"
#include <glm/fwd.hpp>

namespace palette {

class Palette;

class PaletteLookup {
private:
	const palette::Palette &_palette;
	core::Buffer<uint16_t> _cache;
	color::Distance _distance = color::Distance::Approximation;

public:
	PaletteLookup(const palette::Palette &palette, color::Distance distance = color::Distance::Approximation);

	inline const palette::Palette &palette() const {
		return _palette;
	}

	uint8_t findClosestIndex(const glm::vec4 &color);

	uint8_t findClosestIndex(color::RGBA rgba);
};

} // namespace palette
