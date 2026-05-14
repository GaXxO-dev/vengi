# Cartoon & Stylized Voxelization

When converting 3D mesh models (GLB, glTF, OBJ, FBX, etc.) to voxel formats, you can use several configuration options to achieve cartoon, stylized, or cel-shaded looks instead of photorealistic results.

## Quick Start

If your default conversion produces good geometry but the colors look wrong, first make sure texture UV coordinates are being sampled correctly:

```bash
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_targetcolors 0 \
    -set voxformat_rgbweightedaverage true \
    --input model.glb --output model.vox
```

This gives you the full-color, high-fidelity result. From here, you can apply stylization options on top.

## Color Reduction

### Target Colors

Limit the total number of colors in the output palette. Lower numbers produce a flatter, more cartoony look:

```bash
# 16 colors - strong cartoon style
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_targetcolors 16 \
    -set core_colorreduction KMeans \
    --input model.glb --output model.vox

# 32 colors - moderate stylization
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_targetcolors 32 \
    -set core_colorreduction KMeans \
    --input model.glb --output model.vox

# 64 colors - subtle reduction
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_targetcolors 64 \
    -set core_colorreduction KMeans \
    --input model.glb --output model.vox
```

**Algorithm choices** (`core_colorreduction`):
- `KMeans` - Best for cartoon output. Produces well-separated, perceptually meaningful color clusters.
- `MedianCut` - Good default. Preserves visually important colors.
- `Wu` - Even color distribution. Good for very small palettes (8-16 colors).
- `Octree` - Fast but coarse. Only works well when target colors is a power of 2.
- `NeuQuant` - Neural network based. Smoother transitions, less cartoon-like.

### RGB Flatten Factor

Posterize individual RGB channels. Each channel value is snapped to the nearest multiple of the factor:

```bash
# Strong posterization (steps of 32, ~8 levels per channel)
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_rgbflattenfactor 32 \
    --input model.glb --output model.vox

# Moderate posterization (steps of 16, ~16 levels per channel)
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_rgbflattenfactor 16 \
    --input model.glb --output model.vox
```

**Note:** RGB flattening operates independently per channel and can cause slight hue shifts. For better results, use HSB posterization instead.

## HSB Posterization

Posterize colors in HSB (Hue-Saturation-Brightness) space instead of RGB. This gives more control over the cartoon/stylized look by reducing hue variety, saturation variety, and brightness levels independently.

### Cel Shading (Brightness Steps)

The most impactful single setting for cartoon style. Snaps brightness to a fixed number of discrete levels, creating the flat shading bands characteristic of cel animation:

```bash
# Classic cel shading - 3 brightness levels (dark, mid, light)
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_brightnesssteps 3 \
    --input model.glb --output model.vox

# 5 brightness levels - more nuanced cel shading
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_brightnesssteps 5 \
    --input model.glb --output model.vox
```

### Hue Steps

Reduce the number of distinct hues in the output. Groups similar colors together regardless of brightness:

```bash
# 8 distinct hues - simplified color palette
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_huesteps 8 \
    --input model.glb --output model.vox
```

### Saturation Steps

Reduce saturation variety. Colors snap to fixed saturation levels:

```bash
# 4 saturation levels - flat color look
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_saturationsteps 4 \
    --input model.glb --output model.vox
```

## Saturation Boost

Increase vividness of colors. Values above 1.0 boost saturation, values below 1.0 desaturate (toward grayscale):

```bash
# Vivid, punchy colors
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_saturation 1.5 \
    --input model.glb --output model.vox

# Grayscale
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_saturation 0.0 \
    --input model.glb --output model.vox

# Slightly muted
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_saturation 0.7 \
    --input model.glb --output model.vox
```

## Palette Matching

### Force a Custom Palette

Instead of generating a palette from the model's colors, force all voxels to map to a specific palette. This ensures a consistent art style across different models:

```bash
# Use a built-in palette
vengi-voxconvert -f \
    -set voxformat_createpalette false \
    -set palette built-in:magicavoxel \
    --input model.glb --output model.vox

# Use a Lospec palette (downloads from lospec.com)
vengi-voxconvert -f \
    -set voxformat_createpalette false \
    -set palette lospec:sweetie-16 \
    --input model.glb --output model.vox

# Other popular Lospec palettes for cartoon style:
# lospec:pico-8        - 16 colors, retro classic
# lospec:endesga-32    - 32 colors, vibrant
# lospec:resurrect-64  - 64 colors, balanced
# lospec:nyx8          - 8 colors, minimalist
```

### Palette Match Distance

When mapping colors to a palette (`voxformat_createpalette false`), choose how "closest color" is determined:

```bash
# Default: RGB approximation (fast, good general results)
vengi-voxconvert -f \
    -set voxformat_createpalette false \
    -set palette lospec:sweetie-16 \
    -set voxformat_palettematchdistance 0 \
    --input model.glb --output model.vox

# HSB distance (better at preserving perceived color relationships)
vengi-voxconvert -f \
    -set voxformat_createpalette false \
    -set palette lospec:sweetie-16 \
    -set voxformat_palettematchdistance 1 \
    --input model.glb --output model.vox
```

HSB distance (`1`) weighs hue similarity more heavily, which tends to preserve color identity better when mapping to limited palettes.

## Weighted Average

By default, when multiple triangles contribute to the same voxel, their colors are averaged weighted by triangle area. For a flatter, more cartoon-like result, disable this and let the largest triangle win:

```bash
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_rgbweightedaverage false \
    --input model.glb --output model.vox
```

## Complete Recipes

### Full Cartoon Style

A strongly stylized conversion with cel shading, reduced hues, boosted saturation, and limited palette:

```bash
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_targetcolors 24 \
    -set core_colorreduction KMeans \
    -set voxformat_saturation 1.4 \
    -set voxformat_brightnesssteps 4 \
    -set voxformat_huesteps 12 \
    -set voxformat_rgbweightedaverage false \
    --input model.glb --output model_cartoon.vox
```

### Pixel Art Style

Very limited colors with strong posterization:

```bash
vengi-voxconvert -f \
    -set voxformat_scale 64 \
    -set voxformat_targetcolors 8 \
    -set core_colorreduction MedianCut \
    -set voxformat_saturation 1.6 \
    -set voxformat_brightnesssteps 3 \
    -set voxformat_saturationsteps 3 \
    -set voxformat_rgbflattenfactor 32 \
    --input model.glb --output model_pixelart.vox
```

### Flat Color Style

No shading variation, each region gets a single flat color:

```bash
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_targetcolors 16 \
    -set core_colorreduction KMeans \
    -set voxformat_brightnesssteps 2 \
    -set voxformat_saturation 1.3 \
    -set voxformat_rgbweightedaverage false \
    --input model.glb --output model_flat.vox
```

### Lospec Palette with Cel Shading

Use a curated palette combined with cel-shading brightness quantization:

```bash
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_createpalette false \
    -set palette lospec:endesga-32 \
    -set voxformat_brightnesssteps 4 \
    -set voxformat_saturation 1.2 \
    -set voxformat_palettematchdistance 1 \
    --input model.glb --output model_cel.vox
```

### Grayscale with Depth

Desaturated look with brightness levels preserved:

```bash
vengi-voxconvert -f \
    -set voxformat_scale 96 \
    -set voxformat_saturation 0.0 \
    -set voxformat_brightnesssteps 5 \
    --input model.glb --output model_grayscale.vox
```

## Configuration Reference

| CVAR | Type | Range | Default | Description |
|------|------|-------|---------|-------------|
| `voxformat_saturation` | float | 0.0 - 4.0 | 1.0 | Saturation multiplier. 1.0 = no change, 0.0 = grayscale, >1.0 = more vivid |
| `voxformat_brightnesssteps` | int | 0 - 256 | 0 | Number of brightness levels for cel shading. 0 = disabled. Typical: 3-6 |
| `voxformat_huesteps` | int | 0 - 360 | 0 | Number of distinct hue steps. 0 = disabled. Typical: 8-24 |
| `voxformat_saturationsteps` | int | 0 - 256 | 0 | Number of saturation levels. 0 = disabled. Typical: 3-8 |
| `voxformat_rgbflattenfactor` | int | 0 - 255 | 0 | Per-channel posterization factor. 0 = disabled. Typical: 16-64 |
| `voxformat_targetcolors` | int | 0 - 256 | 0 | Target palette size. 0 = no limit. Typical: 8-64 |
| `core_colorreduction` | string | - | MedianCut | Algorithm: KMeans, MedianCut, Wu, Octree, NeuQuant |
| `voxformat_rgbweightedaverage` | bool | - | true | Weight color by triangle area. false = largest triangle wins |
| `voxformat_createpalette` | bool | - | true | Generate palette from model (true) or use existing palette (false) |
| `palette` | string | - | built-in:nippon | Target palette when createpalette=false. Supports built-in:*, lospec:*, file paths |
| `voxformat_palettematchdistance` | int | 0 - 1 | 0 | Distance function for palette matching. 0 = RGB approximation, 1 = HSB |

## Processing Order

Color manipulation is applied in this order during voxelization:

1. **Texture sampling** (UV coordinates -> pixel color)
2. **RGB flatten** (`voxformat_rgbflattenfactor`)
3. **Saturation boost** (`voxformat_saturation`)
4. **HSB posterization** (`voxformat_huesteps`, `voxformat_saturationsteps`, `voxformat_brightnesssteps`)
5. **Palette quantization** (`voxformat_targetcolors` + `core_colorreduction`)
6. **Palette mapping** (when `voxformat_createpalette=false`)