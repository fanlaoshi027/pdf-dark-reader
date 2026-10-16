# Architecture

## V1 rendering pipeline

1. **PDFium** opens and renders the source document.
2. **Page pixel buffer** receives the rendered page in 32-bit BGRA format.
3. **Dark theme mapper** changes only the in-memory display pixels.
4. **Windows display layer** paints the transformed buffer.
5. The original PDF remains untouched.

## Why this supports both PDF types

PDFium renders vector text, paths, and embedded raster images into the same page bitmap. The color mapper runs after rendering, so it does not need to detect whether a page is vector-based or image-based.

## Theme mapping

The mapping is intentionally not a mathematically exact inverse. Each RGB channel is mapped into a range from the selected dark-background channel to a light-foreground channel. Near-white page pixels approach the dark background; black content approaches the light foreground. Theme presets control both colors.

This is a display transform, not a PDF rewrite. It must not alter source files or create a second PDF.

## Performance

The first implementation favors a simple, predictable in-memory pixel pass. Keep the transform separate from PDF loading and rendering so it can later be moved to a GPU shader if profiling shows that CPU mapping is a bottleneck.

## Future extension points

- Theme and strength settings.
- Optional warm tint.
- Settings persistence.
- Windows Ink annotation layer, only after core PDF reading is stable.
