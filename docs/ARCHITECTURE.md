# Architecture

## Phase 1
PDFium renders the source PDF. The viewer applies inversion as a display/rendering effect so scanning PDFs and vector PDFs receive the same visual treatment.

Core layers:
- App UI
- PDF document/render service
- Inversion engine
- Export service

## Planned inversion model
The visual transform must remain an inversion, not a general color filter.

White maps to a configurable target dark background (default around 90% black / charcoal).
Black maps toward the corresponding light foreground.
Other colors follow the inversion relationship.

## Future annotation
Reserve an annotation layer above the PDF page:
Pointer/Windows Ink -> pressure-aware stroke engine -> canvas overlay -> optional PDF annotation/export.
