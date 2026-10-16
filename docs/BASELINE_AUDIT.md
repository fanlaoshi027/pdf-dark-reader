# PDF Reader Core Cleanup — Baseline Audit

This branch is for simplifying PDF Dark Reader into a focused Windows PDF browser. It is not a video player and should not include VLC/Screenbox code.

## Preserve

- Native Windows x64 application shell and PDFium rendering.
- The display-only page color transform. The source PDF must remain unchanged.
- 2x PDFium supersampling followed by downsampling; this is valuable for Chinese text, fraction bars, and geometry diagrams.
- Existing MIT project license.

## Remove from the V1 critical path

The current `v1-clean-reader` branch still compiles a large amount of Mosuan/inking code: `InkEngine`, `LayerSystem`, `StrokeRenderer`, `InkDocument`, favorites, tool panels, and related input/rendering helpers. These are outside the first PDF reader milestone and make the build harder to stabilize.

V1 should focus on:
1. Open local PDF.
2. Page navigation and scrolling.
3. Zoom / fit width.
4. Theme mapping with configurable dark background and light foreground.
5. Reliable Windows x64 packaging.

Annotations, favorites, and other Mosuan tools should stay out of the minimal reader target until the core is stable.

## Baseline build status

The latest recorded Windows Actions run for `v1-clean-reader` failed during compilation (run 38932232119, commit `830fd39efb955d060ea18f9ce87f17a0bd30a38d`). The reported errors include:

- `AppWindowPaint.cpp` calls missing `InkRenderScheduler::BeginFrame`, `MarkInkLayer`, and `EndFrame`.
- `LayerSystem.cpp` has a const-correctness error and an invalid `InkStroke` aggregate initializer.
- `InkEngine.cpp` includes missing `InkStrokeBuffer.h`.
- `MosuanLayerPanel.cpp` has a `std::max` overload error.

These are symptoms of unrelated ink/UI modules being entangled in the reader target. Prefer separating the PDF reader core over adding more unrelated features.

## Open-source reference

Rune (DanialJaved/rune) is a useful reference for reader UX and night-mode behavior, but its repository is GPL-3.0 and its WinUI 3/.NET architecture differs from this native C++/Win32 project. Treat it as a reference unless a deliberate GPL-compatible fork is chosen. Do not copy code into this MIT project without checking license compatibility.

## Next implementation sequence

1. Establish a minimal target with only the Win32 shell, PDFium document/render path, display transform, and essential navigation.
2. Make the Windows Actions build pass before adding UI features.
3. Add theme presets and an intensity control, then validate with both scanned PDFs and vector math PDFs.
4. Only after that, consider optional warm tint and other reading comfort controls.
