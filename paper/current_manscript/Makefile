PDFLATEX ?= pdflatex
BIBTEX ?= bibtex
FIGURE_DIR := figs

.PHONY: all clean

all: main.pdf

main.pdf: main.tex references.bib sections/00_abstract.tex sections/01_introduction.tex sections/02_related_work.tex sections/03_ipi_protocol_design.tex sections/04_system_design_setup.tex sections/05_experiment_results.tex sections/06_future_research_directions.tex sections/07_conclusion.tex sections/08_appendices.tex $(FIGURE_DIR)/figure01_related_work_landscape.pdf $(FIGURE_DIR)/figure02_ipi_protocol_architecture.pdf $(FIGURE_DIR)/figure03_physical_testbed.pdf $(FIGURE_DIR)/figure04_measurement_paths.pdf $(FIGURE_DIR)/section4_signal_survey.pdf $(FIGURE_DIR)/section5_pc5_payload_route.pdf $(FIGURE_DIR)/section5_5g_deadline_envelope.pdf $(FIGURE_DIR)/section5_protocol_completion.pdf $(FIGURE_DIR)/section5_signal_effects.pdf $(FIGURE_DIR)/section5_downlink_latency.pdf $(FIGURE_DIR)/section5_mixed_load_qos.pdf $(FIGURE_DIR)/section5_concurrency_interruption.pdf $(FIGURE_DIR)/section5_tdd_profiles.pdf $(FIGURE_DIR)/section5_directional_workloads.pdf $(FIGURE_DIR)/section5_cross_application_comparison.pdf
	TEXINPUTS=..: $(PDFLATEX) -interaction=nonstopmode -halt-on-error main.tex
	BSTINPUTS=..: $(BIBTEX) main
	TEXINPUTS=..: $(PDFLATEX) -interaction=nonstopmode -halt-on-error main.tex
	TEXINPUTS=..: $(PDFLATEX) -interaction=nonstopmode -halt-on-error main.tex

$(FIGURE_DIR)/section4_signal_survey.pdf: scripts/build_section4_signal_figure.py ../../results/real_5g/20260818_19_gnettrack_rsrp_snr/gnettrack/2026.08.19_three_drives/combined_map/interpolated_rsrp_osm.png ../../results/real_5g/20260818_19_gnettrack_rsrp_snr/gnettrack/2026.08.19_three_drives/combined_map/interpolated_sinr_osm.png
	python3 scripts/build_section4_signal_figure.py

$(FIGURE_DIR)/section5_pc5_payload_route.pdf $(FIGURE_DIR)/section5_5g_deadline_envelope.pdf $(FIGURE_DIR)/section5_protocol_completion.pdf $(FIGURE_DIR)/section5_signal_effects.pdf $(FIGURE_DIR)/section5_downlink_latency.pdf $(FIGURE_DIR)/section5_mixed_load_qos.pdf $(FIGURE_DIR)/section5_concurrency_interruption.pdf $(FIGURE_DIR)/section5_tdd_profiles.pdf $(FIGURE_DIR)/section5_directional_workloads.pdf $(FIGURE_DIR)/section5_cross_application_comparison.pdf: scripts/build_section5_figures.py
	python3 scripts/build_section5_figures.py

clean:
	$(RM) main.aux main.bbl main.blg main.log main.out main.pdf
