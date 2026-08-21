#!/usr/bin/env python3
"""Correct terminology in the flattened Figure 4 PDF."""

from io import BytesIO
from pathlib import Path

from pypdf import PdfReader, PdfWriter
from reportlab.pdfgen import canvas


FIGURE = Path(__file__).resolve().parents[1] / "figs" / "figure04_measurement_paths.pdf"


def main() -> None:
    reader = PdfReader(FIGURE)
    page = reader.pages[0]
    width = float(page.mediabox.width)
    height = float(page.mediabox.height)

    overlay_stream = BytesIO()
    overlay_canvas = canvas.Canvas(overlay_stream, pagesize=(width, height))
    overlay_canvas.setFillColorRGB(1.0, 1.0, 1.0)
    overlay_canvas.rect(414, 538, 260, 19, fill=1, stroke=0)
    overlay_canvas.setFillColorRGB(0.93, 0.11, 0.14)
    overlay_canvas.setFont("Helvetica-Bold", 9)
    overlay_canvas.drawCentredString(
        544,
        543,
        "sequence-correlated IPI response (configured frame size)",
    )
    overlay_canvas.setFillColorRGB(1.0, 1.0, 1.0)
    overlay_canvas.rect(185, 253, 260, 25, fill=1, stroke=0)
    overlay_canvas.rect(610, 253, 275, 25, fill=1, stroke=0)
    overlay_canvas.setFont("Helvetica-Bold", 15)
    overlay_canvas.setFillColorRGB(0.05, 0.28, 0.60)
    overlay_canvas.drawCentredString(315, 260, "Uplink-heavy Uu workload")
    overlay_canvas.setFillColorRGB(0.43, 0.14, 0.68)
    overlay_canvas.drawCentredString(747.5, 260, "Downlink-heavy Uu workload")
    overlay_canvas.save()
    overlay_stream.seek(0)

    page.merge_page(PdfReader(overlay_stream).pages[0])
    writer = PdfWriter()
    writer.add_page(page)
    writer.add_metadata(
        {
            "/Title": "untitled",
            "/Author": "anonymous",
            "/Creator": "anonymous",
            "/Subject": "unspecified",
        }
    )
    temporary = FIGURE.with_suffix(".tmp.pdf")
    with temporary.open("wb") as output:
        writer.write(output)
    temporary.replace(FIGURE)


if __name__ == "__main__":
    main()
