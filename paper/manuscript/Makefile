PDFLATEX ?= pdflatex
BIBTEX ?= bibtex

.PHONY: all clean

all: main.pdf

main.pdf: main.tex references.bib sections/00_abstract.tex sections/01_introduction.tex sections/02_related_work.tex sections/03_ipi_protocol_design.tex sections/04_system_design_setup.tex sections/05_experiment_results.tex sections/06_future_research_directions.tex sections/07_conclusion.tex sections/08_appendices.tex ../figs/edge4av_fig1.pptx.pdf ../figs/Fig2_ipi_protocol_architecture_editable.pptx.pdf ../figs/edge4av_figure3_editable.pptx.pdf figures/section4_signal_survey.pdf figures/section5_pc5_payload_route.pdf figures/section5_5g_deadline_envelope.pdf figures/section5_protocol_completion.pdf figures/section5_signal_effects.pdf figures/section5_downlink_latency.pdf figures/section5_mixed_load_qos.pdf figures/section5_concurrency_interruption.pdf figures/section5_tdd_profiles.pdf figures/section5_directional_workloads.pdf figures/section5_cross_application_comparison.pdf
	TEXINPUTS=..: $(PDFLATEX) -interaction=nonstopmode -halt-on-error main.tex
	BSTINPUTS=..: $(BIBTEX) main
	TEXINPUTS=..: $(PDFLATEX) -interaction=nonstopmode -halt-on-error main.tex
	TEXINPUTS=..: $(PDFLATEX) -interaction=nonstopmode -halt-on-error main.tex

figures/section4_signal_survey.pdf: scripts/build_section4_signal_figure.py ../../results/real_5g/20260818_19_gnettrack_rsrp_snr/gnettrack/2026.08.19_three_drives/combined_map/interpolated_rsrp_osm.png ../../results/real_5g/20260818_19_gnettrack_rsrp_snr/gnettrack/2026.08.19_three_drives/combined_map/interpolated_sinr_osm.png
	python3 scripts/build_section4_signal_figure.py

figures/section5_pc5_payload_route.pdf figures/section5_5g_deadline_envelope.pdf figures/section5_protocol_completion.pdf figures/section5_signal_effects.pdf figures/section5_downlink_latency.pdf figures/section5_mixed_load_qos.pdf figures/section5_concurrency_interruption.pdf figures/section5_tdd_profiles.pdf figures/section5_directional_workloads.pdf figures/section5_cross_application_comparison.pdf: scripts/build_section5_figures.py
	python3 scripts/build_section5_figures.py

clean:
	$(RM) main.aux main.bbl main.blg main.log main.out main.pdf
