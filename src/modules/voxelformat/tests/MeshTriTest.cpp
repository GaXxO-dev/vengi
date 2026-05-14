/**
 * @file
 */

#include "voxelformat/private/mesh/MeshTri.h"
#include "app/tests/AbstractTest.h"
#include "color/ColorUtil.h"
#include "image/Image.h"
#include "voxelformat/private/mesh/MeshMaterial.h"

namespace voxelformat {

class MeshTriTest : public app::AbstractTest {};

TEST_F(MeshTriTest, testColorAt4x4) {
	constexpr int h = 4;
	constexpr int w = 4;
	constexpr color::RGBA buffer[w * h]{
		{255, 0, 0, 255},	{255, 255, 0, 255},	  {255, 0, 255, 255},	{255, 255, 255, 255},
		{0, 255, 0, 255},	{13, 255, 50, 255},	  {127, 127, 127, 255}, {255, 127, 0, 255},
		{255, 0, 0, 255},	{255, 60, 0, 255},	  {255, 0, 30, 255},	{127, 69, 255, 255},
		{127, 127, 0, 255}, {255, 127, 127, 255}, {255, 0, 127, 255},	{0, 127, 80, 255}};
	static_assert(sizeof(buffer) == (size_t)w * (size_t)h * sizeof(uint32_t), "Unexpected rgba buffer size");
	const image::ImagePtr &texture = image::createEmptyImage("4x4");
	texture->loadRGBA((const uint8_t *)buffer, w, h);
	ASSERT_TRUE(texture);
	ASSERT_EQ(w, texture->width());
	ASSERT_EQ(h, texture->height());

	for (int s = 0; s < 2; ++s) {
		const bool originUpperLeft = s == 0;
		SCOPED_TRACE(s);
		voxelformat::MeshTri meshTri;
		MeshMaterialArray meshMaterialArray;
		meshMaterialArray.emplace_back(createMaterial(texture));
		meshMaterialArray.back()->originUpperLeft = originUpperLeft;
		meshTri.materialIdx = meshMaterialArray.size() - 1;
		for (int x = 0; x < w; ++x) {
			for (int y = 0; y < h; ++y) {
				meshTri.setUVs(image::Image::uv(x, y, w, h, originUpperLeft),
							  image::Image::uv(x, y + 1, w, h, originUpperLeft),
							  image::Image::uv(x + 1, y, w, h, originUpperLeft));
				const glm::vec2 &uv = meshTri.centerUV();
				const color::RGBA color = colorAt(meshTri, meshMaterialArray, uv);
				const int texIndex = y * w + x;
				ASSERT_EQ(buffer[texIndex], color)
					<< "pixel(" << x << "/" << y << "), " << color::print(buffer[texIndex]) << " vs "
					<< color::print(color) << " ti: " << texIndex;
			}
		}
	}
}

TEST_F(MeshTriTest, testColorAtOriginUpperLeftPropagation) {
	constexpr int h = 2;
	constexpr int w = 2;
	constexpr color::RGBA buffer[w * h]{
		{255, 0, 0, 255}, {0, 255, 0, 255},
		{0, 0, 255, 255}, {255, 255, 0, 255}};
	const image::ImagePtr &texture = image::createEmptyImage("2x2");
	texture->loadRGBA((const uint8_t *)buffer, w, h);
	ASSERT_TRUE(texture);

	voxelformat::MeshTri meshTri;
	MeshMaterialArray meshMaterialArray;
	meshMaterialArray.emplace_back(createMaterial(texture));
	meshMaterialArray.back()->originUpperLeft = true;
	meshTri.materialIdx = 0;

	meshTri.setUVs(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
	const glm::vec2 uv = meshTri.centerUV();
	const color::RGBA color = colorAt(meshTri, meshMaterialArray, uv);
	ASSERT_EQ(buffer[0], color) << "originUpperLeft=true should sample pixel at row 0 (top)";
}

} // namespace voxelformat
