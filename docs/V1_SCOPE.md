# V1 Scope — Fast PDF Dark Reader

## Product promise

A small Windows 10 application for reading math handouts with a dark page background. The effect is for on-screen reading only: never rewrite the source PDF during normal viewing.

## Must-have

1. Open a local PDF and show page count/current page.
2. Navigate pages and zoom in/out.
3. Live dark-page mapping with one click to return to the original colors.
4. Presets: charcoal (default), dark gray, deep blue, deep green.
5. Same pipeline for vector text/path content and scanned/image-only pages.
6. Preserve anti-aliasing and avoid making fine mathematical strokes look fuzzy.
7. Do not export a transformed PDF in V1.

## Rendering model

PDFium renders a page to a 32-bit pixel buffer. A post-render pixel transform maps each channel from the source value to the selected theme's dark background/light foreground range. The transformed buffer is displayed; the source PDF and its bytes are never modified.

This deliberately prioritizes reading comfort over colorimetric accuracy. It also means vector and bitmap pages use the same color transform after rendering.

## Acceptance tests

- A text/vector math handout: background darkens, text and equations remain readable.
- A scanned handout: white or off-white paper darkens, printed content becomes light.
- A mixed page with embedded diagrams/images: visible and legible, even if colors shift.
- Toggle dark mode on/off without reopening the document.
- Page navigation and zoom do not write a modified PDF to disk.
- Compare source file size and modification time before/after viewing; both must remain unchanged.

## Out of scope for V1

- Exporting a recolored PDF.
- OCR or editing PDF text.
- Annotation/handwriting.
- Exact preservation of original colors.
- Cloud services or account login.
