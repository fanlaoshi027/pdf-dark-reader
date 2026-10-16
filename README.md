# PDF Dark Reader

A lightweight Windows 10 x64 PDF reader for comfortable reading of math handouts.

## V1 goals

- Open and browse local PDF files.
- Apply a live dark-page color transform to the rendered page.
- Support both vector PDFs and scanned/image-only PDFs through the same post-render pipeline.
- Offer charcoal, dark gray, deep blue, and deep green page themes.
- Keep the original PDF untouched. Reading mode must not create or enlarge a converted PDF.
- Prioritize legible small Chinese text, math symbols, thin strokes, and geometry diagrams.

## Technical direction

PDFium renders each page. The reader then transforms the rendered pixels for display, similar in principle to the website player's display-layer filter. This works whether the source page contains vector text/paths or embedded bitmap images.

V1 prioritizes reading comfort over exact color inversion. Near-white paper maps toward a selected dark background and dark content maps toward a light foreground. Some color shifts in diagrams are acceptable.

## Build

Windows x64 build is provided through GitHub Actions. See `.github/workflows/windows-build.yml`.

## License

MIT
