/**
 * @file
 */

#include "PosSampling.h"
#include "color/ColorUtil.h"
#include "core/GLM.h"

namespace voxelformat {

uint8_t PosSampling::getNormal() const {
	if (entries[1].area == 0) {
		return entries[0].normal;
	}
	uint8_t normal = 0;
	uint32_t area = 0;
	for (const PosSamplingEntry &pe : entries) {
		if (pe.area > area) {
			area = pe.area;
			normal = pe.normal;
		}
	}
	return normal;
}

MeshMaterialIndex PosSampling::getMaterialIndex() const {
	if (entries[1].area == 0) {
		return entries[0].materialIdx;
	}
	uint32_t area = 0;
	MeshMaterialIndex materialIdx = -1;
	for (const PosSamplingEntry &pe : entries) {
		if (pe.area > area) {
			area = pe.area;
			materialIdx = pe.materialIdx;
		}
	}
	return materialIdx;

}

bool PosSampling::add(uint32_t area, color::RGBA color, uint8_t normal, MeshMaterialIndex materialIdx) {
	if (area == 0) {
		return false;
	}
	for (int i = 0; i < MaxTriangleColorContributions; ++i) {
		if (entries[i].area > 0 && entries[i].color == color && entries[i].normal == normal) {
			entries[i].area += area;
			return true;
		}
	}
	for (int i = 0; i < MaxTriangleColorContributions; ++i) {
		if (entries[i].area == 0) {
			entries[i].area = area;
			entries[i].color = color;
			entries[i].normal = normal;
			entries[i].materialIdx = materialIdx;
			return true;
		}
	}
	return false;
}

color::RGBA PosSampling::getColor(uint8_t flattenFactor, bool weightedAverage) const {
	if (entries[1].area == 0) {
		return color::flattenRGB(entries[0].color.r, entries[0].color.g, entries[0].color.b, entries[0].color.a,
									   flattenFactor);
	}
	if (weightedAverage) {
		uint32_t sumArea = 0;
		for (const PosSamplingEntry &pe : entries) {
			sumArea += pe.area;
		}
		if (sumArea == 0) {
			return color::RGBA(0, 0, 0, 255);
		}
		float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
		const float invSumArea = 1.0f / (float)sumArea;
		for (const PosSamplingEntry &pe : entries) {
			if (pe.area == 0) {
				break;
			}
			const float weight = (float)pe.area * invSumArea;
			r += (float)pe.color.r * weight;
			g += (float)pe.color.g * weight;
			b += (float)pe.color.b * weight;
			a += (float)pe.color.a * weight;
		}
		const color::RGBA color((uint8_t)glm::round(r), (uint8_t)glm::round(g),
								 (uint8_t)glm::round(b), (uint8_t)glm::round(a));
		return color::flattenRGB(color.r, color.g, color.b, color.a, flattenFactor);
	}
	color::RGBA color(0, 0, 0, AlphaThreshold);
	uint32_t area = 0;
	for (const PosSamplingEntry &pe : entries) {
		if (pe.area == 0) {
			break;
		}
		if (pe.area > area) {
			area = pe.area;
			color = pe.color;
		}
	}
	return color::flattenRGB(color.r, color.g, color.b, color.a, flattenFactor);
}

} // namespace voxelformat
