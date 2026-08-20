#!/usr/bin/env python3
"""Reformat the retained continuous-drive radio maps for the paper.

The source PNGs contain a presentation header, a tall side legend, and a
footer.  This script preserves the map pixels, removes that presentation
chrome, and supplies compact paper-sized labels and color scales.
"""

from pathlib import Path

import matplotlib as mpl
import matplotlib.pyplot as plt


MANUSCRIPT = Path(__file__).resolve().parents[1]
REPO = MANUSCRIPT.parents[1]
SOURCE = (
    REPO
    / "results/real_5g/20260818_19_gnettrack_rsrp_snr/gnettrack"
    / "2026.08.19_three_drives/combined_map"
)
OUT = MANUSCRIPT / "figs/section4_signal_survey.pdf"

mpl.rcParams.update(
    {
        "font.family": "sans-serif",
        "font.size": 8.0,
        "axes.titlesize": 8.4,
        "pdf.fonttype": 42,
        "ps.fonttype": 42,
    }
)


def map_body(path: Path):
    image = plt.imread(path)
    height, width = image.shape[:2]
    # The exported map occupies the left 85.4% between the 4.6% header and
    # 96.2% footer boundary.  Cropping changes layout only; it does not
    # recompute or recolor the measured interpolation.
    return image[int(0.046 * height) : int(0.962 * height), : int(0.854 * width)]


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    panels = [
        (
            SOURCE / "interpolated_rsrp_osm.png",
            "(a) SS-RSRP",
            "SS-RSRP (dBm)",
            -121,
            -88,
            [-121, -115, -105, -95, -88],
        ),
        (
            SOURCE / "interpolated_sinr_osm.png",
            "(b) NR SINR",
            "NR SINR (dB)",
            -20,
            30,
            [-20, -10, 0, 10, 20, 30],
        ),
    ]

    fig, axes = plt.subplots(1, 2, figsize=(7.15, 2.65))
    for ax, (path, title, label, lower, upper, ticks) in zip(axes, panels):
        ax.imshow(map_body(path))
        ax.set_title(title, loc="left", pad=2.5, fontweight="bold")
        ax.set_axis_off()
        scale = mpl.cm.ScalarMappable(
            norm=mpl.colors.Normalize(vmin=lower, vmax=upper),
            cmap="RdYlGn",
        )
        colorbar = fig.colorbar(
            scale,
            ax=ax,
            orientation="horizontal",
            fraction=0.060,
            pad=0.035,
            aspect=24,
        )
        colorbar.set_ticks(ticks)
        colorbar.set_label(label, labelpad=1.5)
        colorbar.ax.tick_params(labelsize=7.2, pad=1.5)

    fig.subplots_adjust(left=0.01, right=0.99, top=0.965, bottom=0.08, wspace=0.10)
    fig.savefig(OUT, bbox_inches="tight", pad_inches=0.01)
    plt.close(fig)


if __name__ == "__main__":
    main()
